/*
 * This file is part of tfsavcodec.
 * 
 * Copyright (c) 2013-2016, Oskar Eisemuth
 * 
 * For the full copyright and license information,
 * please view the LICENSE file that was distributed with this source code.
 * 
 */

#ifndef FILEFUNC_H
#define	FILEFUNC_H

#if defined(_WIN32)
#define PATH_SEP_STR "\\"
#define PATH_SEP_C '\\'
#else
#define PATH_SEP_STR "/"
#define PATH_SEP_C '/'
#endif
#include "tfstring.h"

char* filename_noext(const char *file);
char* filename_directory(const char *file);
char* filename_replace(const char *filepath, const char* filename);

int fileordir_present(const char *filedir);
int file_exists(const char *file);
int is_dir(const char *directory);

void file_align(FILE* fd, size_t align);

FILE* file_open_read(char* filename);
FILE* file_open_write(char* filename);
size_t file_size(FILE* fd);
void file_copy_bytes(FILE* fdsrc, FILE* fddest, size_t len);


#define FILE_READ_TYP(typname, typ) \
typ file_read_##typname(FILE* fd);

#define FILE_READINTO_TYP(typname, typ) \
void file_readinto_##typname(FILE* fd, typ* buffer, int times);

FILE_READ_TYP(u8,  uint8_t)
FILE_READ_TYP(u16, uint16_t)
FILE_READ_TYP(u32, uint32_t)
FILE_READ_TYP(u64, uint64_t)

FILE_READ_TYP(s8,  int8_t)
FILE_READ_TYP(s16, int16_t)
FILE_READ_TYP(s32, int32_t)
FILE_READ_TYP(s64, int64_t)

FILE_READINTO_TYP(u8, uint8_t)
FILE_READINTO_TYP(u16, uint16_t)
FILE_READINTO_TYP(u32, uint32_t)
FILE_READINTO_TYP(u64, uint64_t)

FILE_READINTO_TYP(s8, int8_t)
FILE_READINTO_TYP(s16, int16_t)
FILE_READINTO_TYP(s32, int32_t)
FILE_READINTO_TYP(s64, int64_t)


void file_readinto_bytes(FILE* fd, void* buffer, int size);

void file_readinto_u8_be(FILE* fd, uint8_t* buffer, int times);
void file_readinto_u16_be(FILE* fd, uint16_t* buffer, int times);
void file_readinto_u32_be(FILE* fd, uint32_t* buffer, int times);

void file_readinto_string(FILE* fd, char** string, int times);
#define file_readinto_string_be file_readinto_string
void file_readinto_filepos(FILE* fd, uint32_t* filepos, int times);
#define file_readinto_filepos_be file_readinto_filepos


void file_readinto_tfstring(FILE* fd, tfstring* string, int times);

#define FILE_WRITE_TYP(typname, typ) \
void file_write_##typname(FILE* fd, typ* buffer, int times);

FILE_WRITE_TYP(u8,  uint8_t)
FILE_WRITE_TYP(u16, uint16_t)
FILE_WRITE_TYP(u32, uint32_t)
FILE_WRITE_TYP(u64, uint64_t)

FILE_WRITE_TYP(s8,  int8_t)
FILE_WRITE_TYP(s16, int16_t)
FILE_WRITE_TYP(s32, int32_t)
FILE_WRITE_TYP(s64, int64_t)


void file_write_u8_be(FILE* fd, uint8_t* buffer, int times);
void file_write_u16_be(FILE* fd, uint16_t* buffer, int times);
void file_write_u32_be(FILE* fd, uint32_t* buffer, int times);

void file_write_string(FILE* fd, char** string, int times);
void file_write_tfstring(FILE* fd, tfstring* string, int times);


struct s_FILEPATH {
	char* filepath;
	char* rel_path;
	char* filename;
};
typedef struct s_FILEPATH FILEPATH;


FILEPATH* filepath_new();
FILEPATH* filepath_clone(FILEPATH* obj);
void filepath_free(FILEPATH* obj);
void filepath_basepath(FILEPATH* obj, char* basepath);
void filepath_relpath(FILEPATH* obj, char* relpath);
void filepath_filename(FILEPATH* obj, char* filename);
void filepath_filename_printf(FILEPATH* obj, const char* fmt, ...);
void filepath_rebase(FILEPATH* obj);
#endif	/* FILEFUNC_H */

