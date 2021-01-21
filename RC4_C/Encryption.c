#define _CRT_SECURE_NO_WARNINGS

#define SRC_FILE_PATH "2000x2000.bmp"
#define DEST_FILE_PATH "result.bmp"
#define RESULT_TXT "result.txt"
#define TEXT_IN_PATH "tin.txt"
#define TEXT_OUT_PATH "tout.txt"
#define KEY_PATH "key.txt"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

unsigned int* in10;
unsigned int* in2;
unsigned int* out2;
unsigned int* out10;

//Creates copy of img file & puts first-53 bytes in it.
int copyFile(FILE* ifp, FILE* ofp);

/*converts 
int dec[n] array 
into 
unsigned char bin[n*8] array */
void dec2bin_array(int* dec, unsigned char* bin, int l10, int l2);

/*converts
int unsigned char bin[n*8] array
into
dec[n] array */
void bin2dec_array(int* dec, unsigned char* bin, int l10, int l2);

/*Swaps elements Array[i] & Array[j] of unsigned int Array[]*/
void swap(unsigned int* i, unsigned int* j);

/*  Filling S-block with 0...255 values
	exec "key-scheduling algorithm"
	*/
void init_Sblock(unsigned int* S, unsigned char* key);

void PRGA(unsigned char* S, int len, unsigned char* ext);

void RC4encryption(char* key, unsigned char* in, unsigned char* out, int length);

void main(void)
{
	FILE* KEYfile = fopen(KEY_PATH, "r");
	if (KEYfile == NULL)
	{
		printf("ERROR: unable to open file with message.\n");
		return 0;
	}
	fseek(KEYfile, 0L, SEEK_END);
	int KEYSize = ftell(KEYfile);
	fseek(KEYfile, 0L, SEEK_SET);
	int KEY2Size = KEYSize * 8;
	unsigned int* key10 = (unsigned int*)malloc(sizeof(unsigned int) * KEYSize);
	for (int i = 0; i < KEYSize; i++) key10[i] = fgetc(KEYfile);
	unsigned int* key2 = (unsigned int*)malloc(sizeof(unsigned int) * KEY2Size);

	//dec2bin_array(key10, key2, KEYSize, KEY2Size);

	int sw; //switch
	printf("Choose crypting mode:\nEnter 1 for .bmp\nEnter 2 for .txt\n");
	scanf("%d", &sw); 
	if (sw == 1) 
	{
		//Open Image file
		FILE* imgFile = fopen(SRC_FILE_PATH, "rb");
		if (imgFile == NULL)
		{
			printf("ERROR: unable to open source file.\n");
			return 0;
		}

		fseek(imgFile, 0L, SEEK_END);
		int picSize = ftell(imgFile)-54; // number of bytes in image
		int pic2Size = picSize * 8;   // number of bits in image

		in10 = (unsigned int*)malloc(picSize * sizeof(unsigned int));
		out10 = (unsigned int*)malloc(picSize * sizeof(unsigned int));
		in2 = (unsigned int*)malloc(pic2Size * sizeof(unsigned int));
		out2 = (unsigned int*)malloc(pic2Size * sizeof(unsigned int));
		for (int i = 0; i < pic2Size; i++) in2[i] = 0;
		for (int i = 0; i < picSize; i++) out10[i] = 0;

		//Создать копию файла - изображения
		FILE* destFile = fopen(DEST_FILE_PATH, "wb+");
		if (destFile == NULL) 
		{
			printf("ERROR: unable to create destination file.\n");
			return 0;
		}
		fseek(imgFile, 0L, SEEK_SET);
		fseek(destFile, 0L, SEEK_SET);
		/*copy file
		&
		filling in10[] array beginning from 54th byte*/
		copyFile(imgFile, destFile);
		
		
		dec2bin_array(in10, in2, picSize, pic2Size);

		RC4encryption(key10, in2, out2, pic2Size, KEYSize);

		bin2dec_array(out10, out2, picSize, pic2Size);

		fseek(destFile, 0x36, SEEK_SET);
		for (int i = 0; i < picSize; i++) 
		{ 
			char ch = (char)out10[i];
			fputc(ch, destFile);
		}

		fclose(destFile);

		/*for (int i = 0; i < picSize; i++) 
		{
			printf("%d    %d\n", in10[i], out10[i]);
		}*/
	}
	else
	{
		//Открыть файл-сообщение
		FILE* textINfile = fopen(TEXT_IN_PATH, "r");
		if (textINfile == NULL) 
		{
			printf("ERROR: unable to open file with message.\n");
			return 0;
		}

		//Получить размер текста сообщения в байтах
		fseek(textINfile, 0L, SEEK_END);
		int fileSize = ftell(textINfile);
		fseek(textINfile, 0L, SEEK_SET);
		int file2Size = fileSize * 8;
		
		in10 = (unsigned int*)malloc(fileSize * sizeof(unsigned int));
		out10 = (unsigned int*)malloc(fileSize * sizeof(unsigned int));
		in2 = (unsigned int*)malloc(file2Size * sizeof(unsigned int));
		out2 = (unsigned int*)malloc(file2Size * sizeof(unsigned int));
		for (int i = 0; i < file2Size; i++) in2[i] = 0;
		for (int i = 0; i < fileSize; i++) in10[i] = fgetc(textINfile);
		fclose(textINfile);
		dec2bin_array(in10, in2, fileSize, file2Size);

		RC4encryption(key2, in2, out2, file2Size, KEY2Size); //ENCRYPTION with RC4 algorithm

		for (int i = 0; i < fileSize; i++) out10[i]=0;
		bin2dec_array(out10, out2, fileSize, file2Size);
		
		FILE* textOUTfile = fopen(TEXT_OUT_PATH, "w");
		if (textINfile == NULL) 
		{
			printf("ERROR: unable to create result file.\n");
			return 0;
		}
		for (int i = 0; i < fileSize; i++) 
			putc(out10[i], textOUTfile);
		fclose(textOUTfile);
		//for (int i = 0; i < fileSize; i++) printf("%c", in10[i]);
	}
}

void RC4encryption(unsigned int* key, unsigned int* in, unsigned int* out, int length2, int keylen)
{
	int length10 = length2 / 8;
	unsigned int S[256]; //S-block
	//Must have got the same length that an input/output arrays
	unsigned int* extended10_KEY = (unsigned int*)malloc(sizeof(unsigned int*) * length10);
	unsigned int* extended2_KEY = (unsigned int*)malloc(sizeof(unsigned int*) * length2);

	init_Sblock(S, key, keylen);
	PRGA(S, length10, extended10_KEY);
	dec2bin_array(extended10_KEY, extended2_KEY, length10, length2);
	free(extended10_KEY);

	for (int i = 0; i < length2; i++)
		out[i] = in[i] ^ extended2_KEY[i];

	free(extended2_KEY);
}

void PRGA(unsigned int* S, int len, unsigned int* ext)
{
	if (ext == NULL)
		return NULL;

	int i = 0; int j = 0;
	for (int k = 0; k < len; k++)
	{
		i = (i + 1) % 256;
		j = (j + S[i]) % 256;
		swap(&S[i], &S[j]);
		ext[k] = S[(S[i] + S[j]) % 256];
	}
	
}

void init_Sblock(unsigned int* S, unsigned int* key, int L)
{
	for (int i = 0; i < 256; i++)
		S[i] = i;
	int j = 0;
	for (int i = 0; i < 256; i++)
	{
		j = (j + S[i] + key[i % L]) % 256;
		swap(&S[i], &S[j]);
	}
}

int copyFile(FILE* ifp, FILE* ofp) 
{
	int chread;
	int counter = 0;
	while ((chread = fgetc(ifp)) != EOF)
	{
		char ch = chread;
		if (counter >= 54) in10[counter-54] = chread;
		putc(chread, ofp);
		counter++;
	}
	return counter;
}


void dec2bin_array(unsigned int* dec, unsigned int* bin, int l10, int l2)
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

void bin2dec_array(unsigned int* dec, unsigned int* bin, int l10, int l2)
{
	for (int i = 0; i < l2; i += 8)
	{
		for (int j = 0; j < 8; j++)
		{
			int value = pow(2, j) * (int)bin[i + j];
			dec[i / 8] += value;
		}
	}
}

void swap(unsigned int* i, unsigned int* j)
{
	unsigned int temp;

	temp = *i;
	*i = *j;
	*j = temp;
	return;
}