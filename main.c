/**
 * @file main.c
 *
 * @date 3/27/2018
 * @author Anthony Hilyard
 * @brief A simple program to output the decompressed size of a given zstd-compressed file.
 */

#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define ZSTD_STATIC_LINKING_ONLY
#include <zstd.h>

static off_t fsize_orDie(const char *filename)
{
	struct stat st;
	if (stat(filename, &st) == 0)
	{
		return st.st_size;
	}

	/* error */
	fprintf(stderr, "stat: %s : %s \n", filename, strerror(errno));
	exit(1);
}

static FILE* fopen_orDie(const char *filename, const char *instruction)
{
	FILE* const inFile = fopen(filename, instruction);
	if (inFile)
	{
		return inFile;
	}

	/* error */
	fprintf(stderr, "fopen: %s : %s \n", filename, strerror(errno));
	exit(2);
}

static void* malloc_orDie(size_t size)
{
	void* const buff = malloc(size + !size);   /* avoid allocating size of 0 : may return NULL (implementation dependent) */
	if (buff)
	{
		return buff;
	}

	/* error */
	fprintf(stderr, "malloc: %s \n", strerror(errno));
	exit(3);
}

static void* loadFile_orDie(const char* fileName, size_t* size)
{
	off_t const buffSize = fsize_orDie(fileName);
	FILE* const inFile = fopen_orDie(fileName, "rb");
	void* const buffer = malloc_orDie(buffSize);
	size_t const readSize = fread(buffer, 1, buffSize, inFile);
	if (readSize != (size_t)buffSize)
	{
		fprintf(stderr, "fread: %s : %s \n", fileName, strerror(errno));
		exit(4);
	}
	fclose(inFile);   /* can't fail (read only) */
	*size = buffSize;
	return buffer;
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Usage: %s FILE\n", argv[0]);
		exit(1);
	}

	size_t cSize;
	void* const cBuff = loadFile_orDie(argv[1], &cSize);
	unsigned long long const rSize = ZSTD_findDecompressedSize(cBuff, cSize);
	if (rSize == ZSTD_CONTENTSIZE_ERROR)
	{
		fprintf(stderr, "%s : it was not compressed by zstd.\n", argv[1]);
		exit(5);
	}
	else if (rSize == ZSTD_CONTENTSIZE_UNKNOWN)
	{
		fprintf(stderr, "%s : original size unknown.\n", argv[1]);
		exit(6);
	}

	fprintf(stdout, "%llu", rSize);
	return 0;
}

