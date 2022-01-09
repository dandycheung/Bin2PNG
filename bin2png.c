#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "cwalk.h"
#include "lodepng.h"

#define PNG_FILE "image.png"
#define BIN_IN_FILE "in.exe"
#define BIN_OUT_FILE "out.exe"

int printUsage(int argc, char *argv[]);
int binaryToPng(char *infile, char *outfile);
int pngToBinary(char *infile, char *outfile);

void main(int argc, char *argv[]) {
	char *infile, *outfile;
	int mode = -1;

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

	// TODO:
	// 此处需要根据输入参数，整理出输入文件和输出文件的绝对路径。规则如下。
	// 1. 如果参数完整，首先要将输入文件名和输出文件名绝对路径化（如果输入参数为相对路径，则要引入当前工作目录进行拼装，然后绝对化）；
	// 1.1 如果输入路径不是合法文件，或者改文件不存在，则报错退出；
	// 1.2 如果输出路径中没有文件名部分（也即以 / 结尾），则文件名根据输入文件名生成；如果输入文件名具有扩展名，则将该扩展名变更为 png，否则追加 png 扩展名；
	// 2. 如果参数少一个，则认为缺少了输出文件名，将根据 mode 和输入文件名进行生成；路径保持与输入文件相同，文件名生成规则与 1.2 中的规则相同。

	infile = mode == 0 ? BIN_IN_FILE : PNG_FILE;
	outfile = mode == 0 ? PNG_FILE : BIN_OUT_FILE;

	if (mode == 0)
		binaryToPng(infile, outfile);
	else if (mode == 1)
		pngToBinary(infile, outfile);
}

int printUsage(int argc, char *argv[]) {
	printf("Usage: %s (encrypt|decrypt)", argv[0]);
	puts("");
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

	puts("Starting conversion to PNG file.");

	printf("Writing PNG file to %s...\n", outfile);

	// Write PNG file
	error = lodepng_encode32_file(outfile, pngData, imageWidth, imageHeight);

	// Free memory
	free(pngData);

	if (error) {
		printf("error %u: %s.\n", error, lodepng_error_text(error));
		return -1;
	} else {
		puts("Success!");
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

	printf("Reading PNG file (%s)...\n", PNG_FILE);

	// Decode PNG file to pngData array
	error = lodepng_decode32_file(&pngData, &imageWidth, &imageHeight, infile);
	if (error) {
		printf("error %u: %s.\n", error, lodepng_error_text(error));
		return 0;
	}

	fileSize = *(unsigned int *)pngData;
	binaryBuff = pngData + sizeof(fileSize);

	printf("Size of input image: %d x %d px.\n", imageWidth, imageHeight);

	puts("Starting conversion to binary file.");

	printf("Writing binary file to %s...\n", BIN_OUT_FILE);

	// Write data to binary file
	binaryFile = fopen(outfile, "wb");
	if (binaryFile == NULL) {
		printf("Error writing '%s' file.", outfile);

		free(pngData);

		return -1;
	} else {
		fwrite(binaryBuff, fileSize, 1, binaryFile);

		puts("Success!");

		free(pngData);
		fclose(binaryFile);

		return 0;
	}
}

