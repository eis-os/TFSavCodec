/*
 * This file is part of tfsavcodec.
 * 
 * Copyright (c) 2013-2016, Oskar Eisemuth
 * 
 * For the full copyright and license information,
 * please view the LICENSE file that was distributed with this source code.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdarg.h>
#include <strings.h>
#include <string.h>

#include "memfunc.h"
#include "filefunc.h"
#include "tfstring.h"

#include <errno.h>
#include <sys/stat.h>

#define FILEPATH_BUFFERSIZE 2048

char* filename_noext(const char *file) {
	char *basename, *dotpos;
	int len;
	dotpos = strrchr(file, '.');
	if (dotpos != NULL) {
		len = dotpos - file;
		basename = malloc(len + 1);
		basename[len] = 0;
		strncpy(basename, file, len);
	} else {
		basename = memory_strdup(file);
	}
	return basename;
}

char* filename_directory(const char *file) {
	char *directory, *pointer;
	int len;
	pointer = strrchr(file, PATH_SEP_C);
	if (pointer != NULL) {
		len = pointer - file;
		directory = malloc(len + 1);
		directory[len] = 0;
		strncpy(directory, file, len);
	} else {
		directory = memory_strdup("");
	}
	return directory;
}


char* filename_replace(const char *filepath, const char* filename) {
	char *newfilepath;
	char *pointer;
	size_t len;
	size_t filepath_len = strlen(filepath);
	size_t filename_len = strlen(filename);
	
	newfilepath = malloc(filepath_len+filename_len+10);
	pointer = strrchr(filepath, PATH_SEP_C);
	if (pointer != NULL) {
		len = pointer - filepath;
		memcpy(newfilepath, filepath, len+1);
		memcpy(newfilepath+len+1, filename, filename_len+1);
	} else {
		memcpy(newfilepath, filename, filename_len+1);
	}
	return newfilepath;
}

int fileordir_present(const char *filedir) {
	struct stat st;
	if (stat(filedir, &st) == -1) {
		if (errno == ENOENT) return 0;
		return 1;
	}
	return 1;
}

int file_exists(const char *file) {
	struct stat st;
	if (stat(file, &st) == 0
		&& S_ISREG(st.st_mode)) return 1;
	return 0;
}

int is_dir(const char *directory) {
	struct stat st;
	if (stat(directory, &st) == 0
		&& S_ISDIR(st.st_mode)) return 1;
	return 0;
}




void file_align(FILE* fd, size_t align) {
	size_t poscur;
	poscur = ftell(fd);
	if ( (poscur % align) > 0) {
		poscur += align - (poscur % align);
		fseek(fd, poscur, SEEK_SET);
	}
}

size_t file_size(FILE* fd) {
	size_t len = 0;
	size_t currentPos = ftell(fd);
	fseek(fd, 0, SEEK_END);
	len = ftell(fd);
	fseek(fd, currentPos, SEEK_SET);
	return len;
}

void _file_print_error_eof(FILE* fd, char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	printf("Unexpected end of file:");
	vprintf(fmt, ap);
	va_end(ap);
}


FILE* file_open_read(char* filename) {
	FILE* fd;
	fd = fopen(filename, "rb");
	if (fd == NULL) {
		printf("Opening file %s, %s", filename, strerror(errno));
		exit(-1);
	}
	return fd;
}


FILE* file_open_write(char* filename) {
	FILE* fd;
	printf("Creating file %s\n", filename);
	fd = fopen(filename, "wb");
	if (fd == NULL) {
		printf("ERROR %s, %s", filename, strerror(errno));
		exit(-1);
	}
	return fd;
}



void file_copy_bytes(FILE* fdsrc, FILE* fddest, size_t len) {
	size_t buffermaxsize = 4*1024;
	uint8_t buffer[4*1024];
	
	if (len < buffermaxsize) {
		buffermaxsize = len;
	}
	while(len > 0) {
		size_t n = fread(buffer, 1, buffermaxsize, fdsrc);
		if (n != buffermaxsize) {
			_file_print_error_eof(fdsrc, "Reading file with size %i\n", len);
			exit(-1);
		}
		fwrite(buffer, 1, n, fddest);
		len -= n;
		if (len < buffermaxsize) {
			buffermaxsize = len;
		}
	}
}



#define FILE_READ_TYP_DEF(typname, typ) \
typ file_read_##typname(FILE* fd) {					\
	typ data;							\
	if (fread(&data, sizeof(data), 1, fd) != sizeof(data)) {	\
		if (feof(fd)) {						\
			_file_print_error_eof(fd, "Reading "#typname);	\
			exit(-1);					\
		}							\
	}								\
	return data;							\
}								

FILE_READ_TYP_DEF(u8,  uint8_t)
FILE_READ_TYP_DEF(u16, uint16_t)
FILE_READ_TYP_DEF(u32, uint32_t)
FILE_READ_TYP_DEF(u64, uint64_t)

FILE_READ_TYP_DEF(s8,  int8_t)
FILE_READ_TYP_DEF(s16, int16_t)
FILE_READ_TYP_DEF(s32, int32_t)
FILE_READ_TYP_DEF(s64, int64_t)

uint32_t file_read_u32_bigendian(FILE* fd) {
	return endian_swap(file_read_u32(fd));
}

void file_readinto_bytes(FILE* fd, void* buffer, int size) {
	if (fread(buffer, size, 1, fd) != 1) {
		//if (feof(fd)) {
		_file_print_error_eof(fd, "Reading bytestream with size %i\n", size);
		exit(-1);
		//}
	}
}

void file_readinto_u16_be(FILE* fd, uint16_t* buffer, int times) {
	file_readinto_bytes(fd, buffer, times*sizeof(uint16_t));
	for(int i = 0; i < times ;++i) {
		*buffer = endian_swap_16(*buffer);
		buffer++;
	}
}

void file_readinto_u32_be(FILE* fd, uint32_t* buffer, int times) {
	file_readinto_bytes(fd, buffer, times*sizeof(uint32_t));
	for(int i = 0; i < times ;++i) {
		*buffer = endian_swap(*buffer);
		buffer++;
	}
}

void file_readinto_u8_be(FILE* fd, uint8_t* buffer, int times) {
	file_readinto_u8(fd, buffer, times);
}

#define FILE_READINTO_TYP_DEF(typname, typ)					\
void file_readinto_##typname(FILE* fd, typ* buffer, int times) {		\
	file_readinto_bytes(fd, buffer, times*sizeof(typ));		\
}

FILE_READINTO_TYP_DEF(u8, uint8_t)
FILE_READINTO_TYP_DEF(u16, uint16_t)
FILE_READINTO_TYP_DEF(u32, uint32_t)
FILE_READINTO_TYP_DEF(u64, uint64_t)

FILE_READINTO_TYP_DEF(s8, int8_t)
FILE_READINTO_TYP_DEF(s16, int16_t)
FILE_READINTO_TYP_DEF(s32, int32_t)
FILE_READINTO_TYP_DEF(s64, int64_t)






void file_readinto_string(FILE* fd, char** string, int times) {
	if (times != 1)  {
		_file_print_error_eof(0, "Array of strings not supported");
	}
	char* buffer = NULL;
	int c = 0, count = 0;
	size_t buffer_size = 2048;
	buffer = memory_alloc(buffer_size);
	do {
		if (count >= buffer_size) {
			buffer_size = buffer_size * 2;
			buffer = memory_realloc(buffer, buffer_size);
		}
		c = fgetc(fd);
		if (c == EOF) {
			_file_print_error_eof(fd, "Reading string failed\n");
			exit(-1);
			buffer[count] = '\0';
		} else {
			buffer[count] = (char) c;
		}
	} while (buffer[count++] != '\0');
	*string = buffer;
}
#define file_readinto_string_be file_readinto_string



void file_readinto_tfstring(FILE* fd, tfstring* string, int times) {
	if (times != 1)  {
		_file_print_error_eof(0, "Array of strings not supported");
	}
	for (int i = 0; i < times; i++) {
		size_t buffer_size = file_read_u32(fd);
		char* buffer = memory_alloc(buffer_size+1);
		if (buffer_size > 0) {
			file_readinto_bytes(fd, buffer, buffer_size);
		}
		buffer[buffer_size] = '\0';
		string->len = buffer_size;
		string->str = buffer;
	}
}

void file_readinto_filepos(FILE* fd, uint32_t* filepos, int times) {
	*filepos = ftell(fd);
}
#define file_readinto_filepos_be file_readinto_filepos





#define FILE_WRITE_TYP_DEF(typname, typ) \
void file_write_##typname(FILE* fd, typ* buffer, int times) {			\
	fwrite(buffer, sizeof(typ), times, fd);					\
}

FILE_WRITE_TYP_DEF(u8,  uint8_t)
FILE_WRITE_TYP_DEF(u16, uint16_t)
FILE_WRITE_TYP_DEF(u32, uint32_t)
FILE_WRITE_TYP_DEF(u64, uint64_t)

FILE_WRITE_TYP_DEF(s8,  int8_t)
FILE_WRITE_TYP_DEF(s16, int16_t)
FILE_WRITE_TYP_DEF(s32, int32_t)
FILE_WRITE_TYP_DEF(s64, int64_t)


void file_write_u8_be(FILE* fd, uint8_t* buffer, int times) {
	fwrite(buffer, sizeof(uint8_t), times, fd);
}

void file_write_u16_be(FILE* fd, uint16_t* buffer, int times) {
	for (int i = 0; i < times; ++i) {
		uint16_t temp = endian_swap_16(buffer[i]);
		fwrite(&temp, sizeof(uint16_t), 1, fd);
	}
}

void file_write_u32_be(FILE* fd, uint32_t* buffer, int times) {
	for (int i = 0; i < times; ++i) {
		uint32_t temp = endian_swap(buffer[i]);
		fwrite(&temp, sizeof(uint32_t), 1, fd);
	}
}




void file_write_string(FILE* fd, char** string, int times) {
	if (times != 1) {
		_file_print_error_eof(0, "Array of strings not supported");
	}
	
	if (*string == NULL) return;
	int len = strlen(*string);
	fwrite(*string, 1, len+1, fd);
}


void file_write_tfstring(FILE* fd, tfstring* string, int times) {
	if (times != 1) {
		_file_print_error_eof(0, "Array of tfstrings not supported");
	}
	uint32_t len = 0;
	
	if (string != NULL) {
		len = string->len;
	}
	file_write_u32(fd, &len, 1);
	
	if (len > 0) {
		fwrite(string->str, 1, string->len, fd);
	}
}



char* file_read_cstr(FILE* fd) {
	char* buffer;
	int c, count;
	size_t buffer_size = 2048;
	buffer = memory_alloc(buffer_size);
	do {
		if (count >= buffer_size) {
			buffer_size = buffer_size * 2;
			buffer = memory_realloc(buffer, buffer_size);
		}
		c = fgetc(fd);
		if (c == EOF) {
			buffer[count] = '\0';
		} else {
			buffer[count] = (char) c;
		}
		count++;
	} while (buffer[count] != '\0');
	return buffer;
}







FILEPATH* filepath_new() {
	FILEPATH* filepath;
	filepath = malloc(sizeof(FILEPATH));
	filepath->filepath = calloc(FILEPATH_BUFFERSIZE, 1);
	filepath->rel_path = filepath->filepath;
	filepath->filename = filepath->filepath;
	return filepath;
}
void filepath_free(FILEPATH* obj) {
//	FILEPATH* filepath;
//	filepath = malloc(sizeof(FILEPATH));
	free(obj->filepath);
	free(obj);
}

FILEPATH* filepath_clone(FILEPATH* old) {
	FILEPATH* filepath;
	filepath = filepath_new();
	memcpy(filepath->filepath, old->filepath, FILEPATH_BUFFERSIZE);
	filepath->rel_path += (old->rel_path - old->filepath);
	filepath->filename += (old->filename - old->filepath);
	return filepath;
}

void filepath_debug(FILEPATH* obj) {
	printf("Filepath obj: \n");
	for (int i = 0; i < FILEPATH_BUFFERSIZE; ++i) {
		if (&obj->filepath[i] == obj->rel_path) {
			printf("|");
		}
		if (&obj->filepath[i] == obj->filename) {
			printf("!");
		} 
		printf("%02x ", (unsigned char)obj->filepath[i]);
	}
	printf("\n");
}

void filepath_basepath(FILEPATH* obj, char* basepath) {
	size_t len = strlen(basepath);
	memcpy(obj->filepath, basepath, len);
	if (obj->filepath[len-1] != PATH_SEP_C) {
		obj->filepath[len] = PATH_SEP_C;
		len++;
	}
	obj->filepath[len] = '\0';
	obj->rel_path = obj->filepath+len;
	obj->filename = obj->rel_path;
}

void filepath_relpath(FILEPATH* obj, char* relpath) {
	size_t len = strlen(relpath);
	memcpy(obj->rel_path, relpath, len);
	if (obj->rel_path[len-1] != PATH_SEP_C) {
		obj->rel_path[len] = PATH_SEP_C;
		len++;
	}
	obj->rel_path[len] = '\0';
	obj->filename = obj->rel_path+len;
}
void filepath_filename(FILEPATH* obj, char* filename) {
	if (filename == 0) {
		obj->filename[0] = '\0';
		return;
	}
	strcpy(obj->filename, filename);
}

void filepath_filename_printf(FILEPATH* obj, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vsprintf(obj->filename, fmt, args);
	va_end(args);
}


void filepath_rebase(FILEPATH* obj) {
	char* pointer = strrchr(obj->filepath, PATH_SEP_C);
	if (pointer != NULL) {
		obj->filename = pointer+1;
	}
}