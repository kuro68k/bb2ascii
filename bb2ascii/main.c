#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "tokens.h"

char *find_token(uint16_t token)
{
	for (int i = 0; i < sizeof(token_table) / sizeof(token_table[0]); i++)
	{
		if (token_table[i].token == token)
			return token_table[i].string;
	}
	return NULL;
}

int decodebb(FILE *fin, FILE *fout)
{
	while (!feof(fin))
	{
		uint8_t b = fgetc(fin);
		if (b == 0)
		{
			fprintf(fout, "\n");
			continue;
		}

		if ((b > 31) && (b < 127))
		{
			fputc(b, fout);
			continue;
		}

		if (b > 127)
		{
			uint16_t token = fgetc(fin) & 0xFF;
			token |= b << 8;
			char *token_str = find_token(token);
			if (token_str != NULL)
				fprintf(fout, "%s", token_str);
			else
			{
				fprintf(stderr, "Unknown token: 0x%04X\n", token);
				fprintf(fout, "??%04X??", token);
			}
			continue;
		}

		fprintf(stderr, "Unknown character: 0x%02X\b", b);
	}

	return 0;
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("bb2ascii <input.bb2> [output.txt]\n");
		return 0;
	}

	FILE *fin = fopen(argv[1], "rb");
	if (fin == NULL)
	{
		printf("Unable to open \"%s\"\n", argv[1]);
		return -1;
	}

	FILE *fout = NULL;
	if (argc < 3)
		fout = stdout;
	else
	{
		fout = fopen(argv[2], "w");
		if (fout == NULL)
		{
			printf("Unable to open \"%s\"\n", argv[1]);
			return -1;
		}
	}

	int res = decodebb(fin, fout);
	fclose(fin);
	fclose(fout);
	return res;
}
