/*
 * This file is part of tfsavcodec.
 * 
 * Copyright (c) 2013-2016, Oskar Eisemuth
 * 
 * For the full copyright and license information,
 * please view the LICENSE file that was distributed with this source code.
 * 
 */

#ifndef MEMFUNC_H
#define	MEMFUNC_H

#include <stdint.h>
#include <stdbool.h>


uint32_t endian_swap(uint32_t val);
uint16_t endian_swap_16(uint16_t val);

char* memory_strdup(const char* str);

void* memory_alloc(size_t size);
void* memory_realloc(void* ptr, size_t size);
void memory_free(void* ptr);


typedef struct _BUFFERIOHANDLE BUFFERIOHANDLE;
struct _BUFFERIOHANDLE {
	char* buffer;
	char* bufferptr;
	size_t size;
	size_t remaining;
};

BUFFERIOHANDLE* bufferio_create();
void bufferio_free(BUFFERIOHANDLE* hd);
void bufferio_write(BUFFERIOHANDLE* hd, void* p, size_t len);
void bufferio_align4(BUFFERIOHANDLE* hd);

size_t bufferio_getsize(BUFFERIOHANDLE* hd);
void* bufferio_getbuffer(BUFFERIOHANDLE* hd);
#endif	/* MEMFUNC_H */

