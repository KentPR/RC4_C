#define _CRT_SECURE_NO_WARNINGS

#define SRC_FILE_PATH "4x4.bmp"
#define DEST_FILE_PATH "result.bmp"
#define RESULT_TXT "result.txt"
#define TEXT_FILE_PATH "f.txt"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

int* in10;
unsigned char* in2;
unsigned char* out2;
int* out10;

int copyFile(FILE* ifp, FILE* ofp)
{
	int chread;
	int counter = 0;
	while ((chread = fgetc(ifp)) != EOF)
	{
		char ch = chread;
		if (counter >= 54)
			in10[counter - 54] = chread;
		putc(ch, ofp);
		counter++;
	}
	fclose(ofp);
	return counter;
}

void dec2bin_array(int* dec, unsigned char* bin, int l10, int l2)
{
	for (int i = 0; i < l10; i++)
	{
		int dec_b = dec[i];
		for (int j = 0; j < 8; j++)
		{
			bin[i * 8 + j] = dec_b % 2;
			dec_b /= 2;
		}
	}
}

void bin2dec_array(int* dec, unsigned char* bin, int l10, int l2)
{
	for (int i = 0; i < l2; i+=8)
	{
		for (int j = 0; j < 8; j++)
		{
			int value = pow(2, j) * (int)bin[i + j];
			dec[i / 8] += value;
		}
	}
}

void main()
{
	int sw;
	printf("Choose crypting mode:\nEnter 1 for .bmp\nEnter 2 for .txt\n");
	scanf("%d", &sw);
	if (sw == 1) 
	{
		//Открыть файл-изображение
		FILE* picFile = fopen(SRC_FILE_PATH, "r");
		if (picFile == NULL)
		{
			printf("ERROR: unable to open source file.\n");
			return 0;
		}

		fseek(picFile, -54, SEEK_END);
		int picSize = ftell(picFile);

		int pic2Size = picSize * 8;

		in10 = (int*)malloc(picSize * sizeof(int));
		in2 = (unsigned char*)malloc(pic2Size * sizeof(char));

		//Создать копию файла - изображения
		FILE* destFile = fopen(DEST_FILE_PATH, "wb+");
		if (destFile == NULL) {
			printf("ERROR: unable to create destination file.\n");
			return 0;
		}
		fseek(picFile, 0L, SEEK_SET);
		fseek(destFile, 0L, SEEK_SET);
		copyFile(picFile, destFile);
	}
	else
	{
		//Открыть файл-сообщение
		FILE* textFile = fopen(TEXT_FILE_PATH, "r");
		if (textFile == NULL) {
			printf("ERROR: unable to open file with message.\n");
			return 0;
		}

		//Получить размер текста сообщения в байтах
		fseek(textFile, 0L, SEEK_END);
		int fileSize = ftell(textFile);
		fseek(textFile, 0L, SEEK_SET);
		int file2Size = fileSize * 8;
		
		in10 = (int*)malloc(fileSize * sizeof(int));
		in2 = (unsigned char*)malloc(file2Size * sizeof(char));
		for (int i = 0; i < file2Size; i++) in2[i] = 0;
		for (int i = 0; i < fileSize; i++) in10[i] = fgetc(textFile);
		dec2bin_array(in10, in2, fileSize, file2Size);



		for (int i = 0; i < fileSize; i++) in10[i] = 0;
		bin2dec_array(in10, in2, fileSize, file2Size);
	
		//for (int i = 0; i < fileSize; i++) printf("%c", in10[i]);
	}
}