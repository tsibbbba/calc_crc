#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <locale.h>
#include<stdlib.h>
#include<string.h>
#include <math.h>

unsigned int crc32b(unsigned char* message, const unsigned  int polynom, int len) {
	unsigned int сrc = 0;
	int i = 0, j;
	int plusone = 0;
	while (len--) {
		for (j = 7; j >= 0; j--) {
			plusone = 0;
			if (сrc & 0x80000000) {
				сrc <<= 1;
				if ((message[i]) & 0x80) {
					plusone++;
					сrc += 1;
				}
				сrc ^= polynom;
			}
			else {
				сrc <<= 1;
			}
			if ((message[i] & 0x80) > 0 && plusone == 0) {
				сrc += 1;
			}
			message[i] <<= 1;
		}
		i = i + 1;
	}
	return сrc;
}
void decToBin(unsigned char symbol, char* ch_to_bin)
{
	int i = 0;
	do
	{
		ch_to_bin[i++] = symbol % 2 + '0';
		symbol = symbol / 2;
	} while (symbol != 0);
	for (int j = i; j <= 8; j++)
	{
		ch_to_bin[j] = '0';
	}
	for (int k = 0, j = 7; k < 4; k++, j--)
	{
		char tmp = ch_to_bin[k];
		ch_to_bin[k] = ch_to_bin[j];
		ch_to_bin[j] = tmp;
	}
	ch_to_bin[8] = 0;
}

void char_to_bin(unsigned char* file_Arr, unsigned char* binary_arr, int SizeA)
{
	char bin[9];
	for (int i = 0; i < SizeA; i++)
	{
		decToBin(file_Arr[i], bin);
		for (int j = i * 8, k = 0; j < i * 8 + 8; j++, k++)
		{
			binary_arr[j] = bin[k];
		}
	}
}


void Index(int* Ind, unsigned char* Bin)
{
	while (Bin[*Ind] != '1' && Bin[*Ind] != '\0')
	{
		(*Ind)++;
	}
}

int CRC32_K(unsigned char* Bin, const char* polinom, char* CRC, int SizeBin)
{
	int index = 0;
	Index(&index, Bin);

	while ((SizeBin - index) >= 33)
	{
		for (int i = index, j = 0; j < 33; i++, j++)
		{
			Bin[i] = (Bin[i] ^ polinom[j]) + '0';
		}
		Index(&index, Bin);
	}
	for (int i = 0, j = index; i <= SizeBin - index; i++, j++)
	{
		CRC[i] = Bin[j];   //не использую memcpy т.к копирание происходит не с начала
	}
	return SizeBin - index;
}


int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "RUS");
	const char polynom[34] = { '1','0','1','1','1','0','1','0','0','0','0','0','1','1','0','1','1','1','0','0','0','1','1','0','0','1','1','0','1','0','1','1','1','\0' };
	const unsigned int polynom1 = 0b1110100000110111000110011010111;
	if (argc != 2)
	{
		printf("Используйте %s <имя файла>", argv[0]);
		return -1;
	}
	FILE* input = fopen(argv[1], "rb");
	if (input == NULL)
	{
		return -1;
	}
	int T = 0;
	unsigned char c;
	int len = 0;
	while ((c = fgetc(input)) != EOF && !feof(input)) {
		T += c;
		len++;
	}
	fclose(input);
	int MaximumAllowableValue = 255;
	int C = T % (MaximumAllowableValue + 1);
	printf("Контрольная сумма : С = %d\n", C);
	printf("Полином делитель в 16-ричном виде: 0xBA0DC66B\n");

	unsigned char* ArrayForFile;
	ArrayForFile = (unsigned char*)malloc((len + 1) * sizeof(unsigned char));
	if (ArrayForFile == NULL)
	{
		return -1;
	}
	int counter = 0;
	input = fopen(argv[1], "rb");
	if (input == NULL)
	{
		return -1;
	}
	while (counter < len)
	{
		ArrayForFile[counter++] = fgetc(input);
	}
	ArrayForFile[counter] = '\0';

	int SizeBin = counter * 8 + 33;
	unsigned char* ArrayForBin = (unsigned char*)malloc((SizeBin) * sizeof(unsigned char));
	if (ArrayForBin == NULL)
	{
		return EXIT_FAILURE;
	}
	memset(ArrayForBin, '0', SizeBin);

	ArrayForBin[SizeBin] = 0;
	char_to_bin(ArrayForFile, ArrayForBin, counter);
	char CRC[128] = { 0 };
	unsigned char* ArrayForBinCpy = (unsigned char*)malloc((SizeBin + 1) * sizeof(unsigned char));
	memcpy(ArrayForBinCpy, ArrayForBin, (size_t)SizeBin);
	ArrayForBinCpy[SizeBin] = '\0';
	char Symbol_crc[128] = { '0' };
	int SizeCRC = CRC32_K(ArrayForBin, polynom, CRC, SizeBin - 1);
	CRC[SizeCRC] = '\0';
	unsigned char* arr_with_null = (unsigned char*)malloc(strlen(ArrayForFile) + 4);
	memset(arr_with_null, 0, strlen(ArrayForFile) + 4);
	memcpy(arr_with_null, ArrayForFile, strlen(ArrayForFile));
	//printf("\nCRC %u", crc32b(arr_with_null, polynom1, (int)strlen(ArrayForFile) + 4)); 2-ой способ
	for (int i = SizeBin - SizeCRC - 1, j = 0; i < SizeBin; j++, i++)
	{
		ArrayForBinCpy[i] = CRC[j];
	}
	ArrayForBinCpy[SizeBin + SizeCRC] = '\0';
	int Dopnul = 8 - SizeCRC % 8 + 1;
	int l = 0;
	for (int i = Dopnul; i < (int)strlen(CRC) + Dopnul; i++)
	{
		Symbol_crc[i] = CRC[l++];
	}
	printf("\n");
	puts(CRC);
	printf("\n");
	unsigned long long int DEC = 0;
	for (int i = SizeCRC-1; i >= 0; i--)
	{
		if (CRC[i] == '1') {
			DEC += (unsigned long long int)pow(2, (SizeCRC-1) - i);
		}
	}
	printf("Десятичный вид %llu\n", DEC);

	 int newSizeCRC = SizeCRC + Dopnul;

	unsigned char* SimvolCRC = (unsigned char*)malloc(((newSizeCRC / 8 + 1)) * sizeof(unsigned char));
	if (SimvolCRC == NULL)
	{
		printf("malloc Error");
		return -1;
	}
	memset(SimvolCRC, 0, (size_t)newSizeCRC / 8);
	printf("Значение CRC в символьном виде: ");
	for (int k = 0; k < newSizeCRC / 8; k++)
	{
		int j = k * 8 + 7;
		for (int i = 0; i < 8; i++)
		{
			if (Symbol_crc[j] == '1')
			{
				SimvolCRC[k] = SimvolCRC[k] + (unsigned char)pow(2, i);
			}
			j--;
		}
		printf("%c", SimvolCRC[k]);
	}
	free(SimvolCRC);
	printf("\nЗначение CRC в 16-ричном виде: %llX\n", DEC);

	printf("\n");
	int Action;
	printf("Нажмите'1' , если хотите дописать значение CRC в конец файла\nНажмите '2', если хотите завершить работу программы\n");
	if (scanf("%d", &Action) != 1) {
		printf("Scanf Error");
		return -1;
	}
	int SizeArrayForBin = 0;
	while (ArrayForBinCpy[SizeArrayForBin++] != '\0');

	if (Action == 1)
	{
		printf("Значение CRC: ");
		CRC32_K(ArrayForBinCpy, polynom, CRC, SizeArrayForBin);
		if (CRC[0] == 0)
		{
			printf("0");
		}
	}
	fclose(input);
	free(arr_with_null);
	free(ArrayForBinCpy);
	free(ArrayForFile);
	return 0;
}