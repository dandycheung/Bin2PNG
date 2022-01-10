#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <sys/stat.h> // the only stuff that break the cross-platform capability

#include "cwalk.h"
#include "lodepng.h"

#ifndef MAX_PATH
#define MAX_PATH 256
#define MAX_PATHx2 512
#endif

#define PNG_FILE "image.png"
#define BIN_IN_FILE "in.exe"
#define BIN_OUT_FILE "out.exe"

int printUsage(int argc, char *argv[]);
int binaryToPng(char *infile, char *outfile);
int pngToBinary(char *infile, char *outfile);

bool isDir(const char *path);

void main(int argc, char *argv[]) {
	char *infile, *outfile;
	int mode = -1;

	bool flag;
	char cwd[MAX_PATH];
	char ifnbuff[MAX_PATHx2];
	char ofnbuff[MAX_PATHx2];

	if (argc < 3 || argc > 4) {
		printUsage(argc, argv);
		return;
	}

	if (strcmp(argv[1], "e") == 0 || strcmp(argv[1], "encrypt") == 0)
		mode = 0;
	else if (strcmp(argv[1], "d") == 0 || strcmp(argv[1], "decrypt") == 0)
		mode = 1;

	if (mode < 0) {
		printUsage(argc, argv);
		return;
	}

	// 需要根据整理出输入文件和输出文件的绝对路径 -->
	// 1. 将输入文件名绝对路径化（如果输入参数为相对路径，则要引入当前工作目录进行拼装，然后绝对化）；
	flag = cwk_path_is_absolute(argv[2]);
	if (flag)
		cwk_path_normalize(argv[2], ifnbuff, MAX_PATHx2);
	else {
		getcwd(cwd, MAX_PATH);
		cwk_path_join(cwd, argv[2], ifnbuff, MAX_PATHx2);
	}

	printf("input file name is %s, a %s path; processed to %s.\n", argv[2], flag ? "absolute" : "relative", ifnbuff);

	// TODO: 如果输入路径不是合法文件（如是个目录），或者该文件不存在/不可读，则报错退出；

	// 2. 检查是否指定了输出
	if (argc == 3) { // 没有指定，则根据输入文件名生成
		// TODO: 其实需要先检查输入文件的扩展名的，应确保不一致，否则输出文件会将输入文件覆盖
		cwk_path_change_extension(ifnbuff, mode == 0 ? "png" : "bin", ofnbuff, MAX_PATHx2);
	} else {         // 指定了输出
		// 首先要绝对化
		flag = cwk_path_is_absolute(argv[3]);
		if (flag)
			cwk_path_normalize(argv[3], ofnbuff, MAX_PATHx2);
		else {
			getcwd(cwd, MAX_PATH);
			cwk_path_join(cwd, argv[3], ofnbuff, MAX_PATHx2);
		}

		// 检查是不是一个目录
		if (isDir(ofnbuff)) { // 是目录
			// 将输入文件名拼接到目录后，然后确定并更改扩展名
			const char *base = NULL;
			size_t length = 0;
			cwk_path_get_basename(ifnbuff, &base, &length);
			cwk_path_join(ofnbuff, base, ofnbuff, MAX_PATHx2);

			// TODO: 其实需要先检查输入文件的扩展名的，应确保不一致，否则输出文件会将输入文件覆盖
			cwk_path_change_extension(ofnbuff, mode == 0 ? "png" : "bin", ofnbuff, MAX_PATHx2);
		}
	}

	printf("output file name is %s, a %s path; processed to %s.\n", argc == 3 ? "not specified" : argv[2], flag ? "absolute" : "relative", ofnbuff);
	// 整理出输入文件和输出文件的绝对路径 <--

	infile = ifnbuff; // mode == 0 ? BIN_IN_FILE : PNG_FILE;
	outfile = ofnbuff ; // mode == 0 ? PNG_FILE : BIN_OUT_FILE;

	if (mode == 0)
		binaryToPng(infile, outfile);
	else if (mode == 1)
		pngToBinary(infile, outfile);
}

int printUsage(int argc, char *argv[]) {
	printf("Usage: %s <encrypt|decrypt> input [output]\n", argv[0]);
}

// Convert a binary file to PNG
int binaryToPng(char *infile, char *outfile) {
	FILE *binaryFile;
	unsigned long fileSize;
	unsigned char *fileBuff;

	unsigned int binarySize;
	unsigned int pixelCount;
	unsigned int imageWidth;
	unsigned int imageHeight;

	unsigned int pngDataSize;
	unsigned char *pngData;
	unsigned int i, error;

	// Open binary file
	binaryFile = fopen(infile, "rb");
	if (binaryFile == NULL) {
		printf("Error reading '%s' file.", infile);
		return 0;
	}

	// Get binary file length
	fseek(binaryFile, 0, SEEK_END);
	fileSize = ftell(binaryFile);
	fseek(binaryFile, 0, SEEK_SET);

	binarySize = fileSize + sizeof(fileSize);

	pixelCount = binarySize / 4;
	if (binarySize % 4)
		pixelCount++;

	imageWidth = (int)ceil(sqrt((double) pixelCount));
	imageHeight = pixelCount / imageWidth;
	if (pixelCount % imageWidth)
		imageHeight++;

	pngDataSize = imageWidth * imageHeight * 4;

	// Print various informations
	printf("Size of input file: %ld bytes.\n", fileSize);
	printf("Size of output image: %d x %d px.\n", imageWidth, imageHeight);

	// Allocate memory for the buffer
	pngData = (unsigned char *)malloc(pngDataSize);
	// TODO: check returned buffer

	*(unsigned long *)pngData = fileSize;

	// Read binary file to buffer
	fileBuff = pngData + sizeof(fileSize);
	fread(fileBuff, fileSize, 1, binaryFile);
	fclose(binaryFile);

	for (i = binarySize; i < pngDataSize - 1; i++)
		pngData[i] = 0;

	printf("Starting conversion to PNG file.\n");

	printf("Writing PNG file to %s...\n", outfile);

	// Write PNG file
	error = lodepng_encode32_file(outfile, pngData, imageWidth, imageHeight);

	// Free memory
	free(pngData);

	if (error) {
		printf("error %u: %s.\n", error, lodepng_error_text(error));
		return -1;
	} else {
		printf("Success!\n");
		return 0;
	}
}

// Convert a PNG file to binary
int pngToBinary(char *infile, char *outfile) {
	FILE *binaryFile;
	unsigned long fileSize;
	unsigned char *binaryBuff;
	unsigned char *pngData;
	unsigned int error;

	int imageWidth, imageHeight;

	printf("Reading PNG file (%s)...\n", infile);

	// Decode PNG file to pngData array
	error = lodepng_decode32_file(&pngData, &imageWidth, &imageHeight, infile);
	if (error) {
		printf("error %u: %s.\n", error, lodepng_error_text(error));
		return 0;
	}

	fileSize = *(unsigned int *)pngData;
	binaryBuff = pngData + sizeof(fileSize);

	printf("Size of input image: %d x %d px.\n", imageWidth, imageHeight);

	printf("Starting conversion to binary file.\n");

	printf("Writing binary file to %s...\n", outfile);

	// Write data to binary file
	binaryFile = fopen(outfile, "wb");
	if (binaryFile == NULL) {
		printf("Error writing '%s' file.", outfile);

		free(pngData);

		return -1;
	} else {
		fwrite(binaryBuff, fileSize, 1, binaryFile);

		printf("Success!\n");

		free(pngData);
		fclose(binaryFile);

		return 0;
	}
}

bool isDir(const char *path) {
	struct stat buf;
	stat(path, &buf);
	return S_ISDIR(buf.st_mode);
}

