#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lodepng.h"

#define PNG_FILE "image.png"
#define BIN_IN_FILE "in.exe"
#define BIN_OUT_FILE "out.exe"

int binaryToPng();
int pngToBinary();

void main(int argc, char *argv[]) {
	if (argc > 1) {
		if (strcmp(argv[1], "e") == 0 || strcmp(argv[1], "encrypt") == 0) {
			binaryToPng();
			return;
		} else if (strcmp(argv[1], "d") == 0 || strcmp(argv[1], "decrypt") == 0) {
			pngToBinary();
			return;
		}
	}

	printf("Usage : %s (encrypt|decrypt)", argv[0]);
	puts("");
}

// Convert a binary file to PNG
int binaryToPng() {
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
	binaryFile = fopen(BIN_IN_FILE, "rb");
	if (binaryFile == NULL) {
		printf("Error reading '%s' file.", BIN_IN_FILE);
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

	printf("Writing PNG file to %s...\n", PNG_FILE);

	// Write PNG file
	error = lodepng_encode32_file(PNG_FILE, pngData, imageWidth, imageHeight);

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
int pngToBinary() {
	FILE *binaryFile;
	unsigned long fileSize;
	unsigned char *binaryBuff;
	unsigned char *pngData;
	unsigned int error;

	int imageWidth, imageHeight;

	printf("Reading PNG file (%s)...\n", PNG_FILE);

	// Decode PNG file to pngData array
	error = lodepng_decode32_file(&pngData, &imageWidth, &imageHeight, PNG_FILE);
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
	binaryFile = fopen(BIN_OUT_FILE, "wb");
	if (binaryFile == NULL) {
		printf("Error writing '%s' file.", BIN_IN_FILE);

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

