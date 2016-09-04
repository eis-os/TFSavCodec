/*
 * This file is part of tfsavcodec.
 * 
 * Copyright (c) 2013-2016, Oskar Eisemuth
 * 
 * For the full copyright and license information,
 * please view the LICENSE file that was distributed with this source code.
 * 
 */
#define TOOLINFO "tfsavcodec " "2016_09_04"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdarg.h>
#include <strings.h>
#include <string.h>

#include <stdbool.h>

#include "misc.h"
#include "memfunc.h"
#include "filefunc.h"

#include <direct.h>

#include "noson/noson.h"
#include "lz4helper.h"


#include "tfsavegamestruct.h"
#include "tfsavegame.h"

int verbose = 25;
int dumpoffsets = 0;
int forcedir = 0;
int onlyassets = 0;
int depends = 0;

char* sep = "----------------------------------\n";


#define MAGICNUMBER_COMPRESSED 0x184D2204
#define MAGICNUMBER_TFUNCOMPRESSED 0x2A2A6674 // tf**


char* createoutputdirectory(char* filename) {
	int i = 0;
	char *directory = NULL;
	char *altdirectory = NULL;
	directory = filename_noext(filename);
	if (fileordir_present(directory)) {
		print(0, "Directory %s exists\n", directory);
		if (forcedir && is_dir(directory)) {
			print(0, "Directory %s will be reused\n", directory);
			return directory;
		}
		altdirectory = memory_alloc(strlen(directory) + 100);
		while (i < 100) {
			sprintf(altdirectory, "%s.%u", directory, i);
			if (!fileordir_present(altdirectory)) {
				print(0, "Creating directory %s\n", altdirectory);	
				mkdir(altdirectory);
				free(directory);
				return altdirectory;
			}
			i++;
		}
		print_err(0, "Output directory (%s) already exists , max tries reached", altdirectory);
		exit(-1);
	}
	print(0, "Creating directory %s\n", directory);	
	mkdir(directory);
	return directory;
}

bool tfsavegame_decompress(FILE* fd, char* filename) {

	void* inbuffer = NULL;
	size_t inbuffer_len = 0;
	void* outbuffer1 = NULL;
	size_t outbuffer1_len = 0;
	void* outbuffer2 = NULL;
	size_t outbuffer2_len = 0;
	
	
	
	fseek(fd, 0, SEEK_END);
	inbuffer_len = ftell(fd);
	fseek(fd, 0, SEEK_SET);
	inbuffer = memory_alloc(inbuffer_len);
	print(0, "Reading in %d Bytes\n", inbuffer_len);
	if (fread(inbuffer, inbuffer_len, 1, fd) != 1) {
		print_err(0, "Reading failed\n");
		goto cleanup_fail;
	}
	

	if (!decompressBuffer(inbuffer, inbuffer_len, &outbuffer1, &outbuffer1_len)) {
		print_err(0, "decompressing Stage 1 failed\n", filename);
		goto cleanup_fail;
	}
	
	if (!decompressBuffer(outbuffer1, outbuffer1_len, &outbuffer2, &outbuffer2_len)) {
		print_err(0, "decompressing Stage 2 failed\n", filename);
		goto cleanup_fail;
	}
	
	FILE* fdout = fopen(filename, "wb");
	if (fdout == NULL) {
		print_err(0, "Writing file %s, %s", filename, strerror(errno));
		goto cleanup_fail;
	}
	fwrite(outbuffer2, outbuffer2_len, 1, fdout);
	fclose(fdout);
	
	return true;
cleanup_fail:
	free(inbuffer);
	free(outbuffer1);
	free(outbuffer2);
	return false;
}


bool tfsavegame_compress(FILE* fd, char* filename) {

	void* inbuffer = NULL;
	size_t inbuffer_len = 0;
	void* outbuffer1 = NULL;
	size_t outbuffer1_len = 0;
	void* outbuffer2 = NULL;
	size_t outbuffer2_len = 0;
	
	
	
	fseek(fd, 0, SEEK_END);
	inbuffer_len = ftell(fd);
	fseek(fd, 0, SEEK_SET);
	inbuffer = memory_alloc(inbuffer_len);
	print(0, "Reading in %d Bytes for compressing\n", inbuffer_len);
	if (fread(inbuffer, inbuffer_len, 1, fd) != 1) {
		print_err(0, "Reading failed\n");
		goto cleanup_fail;
	}
	
	print(0, "Compressing Stage 1:\n", filename);
	if (!compressBuffer(inbuffer, inbuffer_len, &outbuffer1, &outbuffer1_len)) {
		print_err(0, "compressing Stage 1 failed\n", filename);
		goto cleanup_fail;
	}
	free(inbuffer);
	inbuffer = NULL;
	print(1, "OK\n");
	
	print(0, "Compressing Stage 2:\n");
	if (!compressBuffer(outbuffer1, outbuffer1_len, &outbuffer2, &outbuffer2_len)) {
		print_err(0, "compressing Stage 2 failed\n", filename);
		goto cleanup_fail;
	}
	print(1, "OK\n");
	
	FILE* fdout = fopen(filename, "wb");
	if (fdout == NULL) {
		print_err(0, "Writing compressed file %s, %s", filename, strerror(errno));
		goto cleanup_fail;
	}
	fwrite(outbuffer2, outbuffer2_len, 1, fdout);
	fclose(fdout);
	
	return true;
cleanup_fail:
	free(inbuffer);
	free(outbuffer1);
	free(outbuffer2);
	return false;
}



uint32_t tfsavegame_getMagic(FILE* fd, char *filename) {
	uint32_t signature;
	fseek(fd, 0, SEEK_SET);
	if (fread(&signature, 4, 1, fd) == 1) {
		if (signature == MAGICNUMBER_COMPRESSED || signature == MAGICNUMBER_TFUNCOMPRESSED) {
			fseek(fd, 0, SEEK_SET);
			return signature;
		}
	}
	print_err(0,"Can't determine type of file %s\n", filename);
	fclose(fd);
	exit(-1);
}

void tfsavegame_readCompressed(char *filename) {
	uint32_t signature;	
	size_t filesize;
	
	FILE* fd = file_open_read(filename);
	
	
	signature = tfsavegame_getMagic(fd, filename);
	char* directory = createoutputdirectory(filename);
	if (signature == MAGICNUMBER_COMPRESSED) {
		print(0, "Compressed file found\n");
		FILEPATH *ff = filepath_new();
		filepath_basepath(ff, directory);
		filepath_filename(ff, "uncompressed.data");
		filename = memory_strdup(ff->filepath);
		filepath_free(ff);
			
		if (!tfsavegame_decompress(fd, filename)) {
			fclose(fd);
			exit(-1);
		}
		
		
		fclose(fd);
		fd = file_open_read(filename);
		signature = tfsavegame_getMagic(fd, filename);
	}
	print(0, "Processing file %s to %s\n", filename, directory);
	
	
	fseek(fd, 0xFF, SEEK_SET);
	uint32_t a = file_read_u32(fd);
	print(0, "0xFF = %d", a);
	fseek(fd, 0, SEEK_SET);
	
	
	tfsavegame_read(fd, filename, directory);
	
	
	free(directory);
	free(filename);
	fclose(fd);
}



void tfsavegame_writeCompressed(char* directory) {
	char *outfilename;
	outfilename = memory_alloc(strlen(directory)+20);
	strcpy(outfilename, directory);
	strcat(outfilename, "_new.sav");

	printf("\nCreating new savegame %s from %s\n%s", outfilename, directory, sep);
	
	if (is_dir(directory) != 1) {
		print_err(0, "%s doesn't seem to be a directory", directory);
		exit(-1);
	}
	
	FILEPATH* ff = filepath_new();
	filepath_basepath(ff, directory);
	
	
	/*
	filepath_filename(ff, "uncompressed.data");
	
	
	if (!file_exists(ff->filepath)) {
		print_err(0, "%s missing", ff->filepath);
		goto cleanup;
	
	}
	
	FILE* fdold = file_open_read(ff->filepath);
	tfsavegame_compress(fdold, outfilename);
	fclose(fdold);
	*/
	filepath_filename(ff, "uncompressed.tmp");
	FILE* fd = file_open_write(ff->filepath);

	tfsavegame_write(fd, directory);
	fflush(fd);
	fclose(fd);
	
	fd = file_open_read(ff->filepath);
	tfsavegame_compress(fd, outfilename);
		
	fclose(fd);
	
cleanup:
	filepath_free(ff);
}

void usage(char *name) {
	printf("Usage: %s <options> file\n"
	" -x            extract file\n"
	" -c directory  compress directory\n"
	" -v            verbose\n"
	" -vv           extra verbose\n"
//	" -o            dump offsets\n"
	" --forcedir    force reusage of dir\n"	
	" \n", name);
}
int main(int argc, char** argv) {
	int i;
	int extract = 0;
	int import = 0;
	char* filename = NULL;
	char* directory = NULL;
	
	printf(TOOLINFO "\n");
	printf("Copyright (c) 2013-2016 Oskar Eisemuth\n");
	printf(sep);
	printf("LZ4 Library, Copyright (c) 2011-2015, Yann Collet\n"
"All rights reserved.\n"
	);
	printf(sep);
	printf("\n");
	for(i=1; i<argc; i++) {
		char *arg = argv[i];
		if(arg[0] == '-') {
			switch(arg[1]) {
				case 'h':
					usage(argv[0]);
				case 'x': 
					extract = 1;
					if (i<argc) {
						i++;
						filename = memory_strdup(argv[i]);
					} else {
						usage(argv[0]);
					}
					break;
				case 'c':
					import = 2;
					if (i<argc) {
						i++;
						directory = memory_strdup(argv[i]);
					} else {
						usage(argv[0]);
					}
					break;
				case 'v':
					verbose = 20;
					if (arg[2] == 'v') {
						verbose += 10;
					}
					break;
				case 'o':
					dumpoffsets = 1;
					break;
				case '-':
					if (strcmp(arg, "--forcedir") == 0) {
						forcedir = 1;
						break;
					}
					if (strcmp(arg, "--depends") == 0) {
						depends = 1;
						break;
					}
				default:
					printf("Unknown option %s \n", argv[i]);
					return EXIT_FAILURE;
			}
		} else {
			filename = memory_strdup(argv[i]);
		}
	}

	if (extract == 1) {
		if (filename == NULL) {
			printf("Missing filename \n");
			return EXIT_FAILURE;
		}
		tfsavegame_readCompressed(filename);
	}
	if(import == 2) {
		tfsavegame_writeCompressed(directory);
	} 
	
	if (extract == 0 && import == 0) {
		usage(argv[0]);
	}

	return EXIT_SUCCESS;
}

