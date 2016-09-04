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
#include <string.h>	// memcpy
#include "memfunc.h"

uint32_t endian_swap(uint32_t val) {
	return ((val & 0xff000000) >> 24) | ((val & 0x00ff0000) >> 8) | ((val & 0x0000ff00) << 8) | ((val & 0x000000ff) << 24);
}
uint16_t endian_swap_16(uint16_t val) {
	return ((val & 0xff00) >> 8) | ((val & 0x00ff) << 8);
}


char* memory_strdup(const char* str) {
	char *dup;
	size_t len = strlen(str) + 1;
	dup = malloc(len);
	if (dup == NULL) {
		printf("Can't alloc string memory");
		exit(1);
	}
	memcpy(dup, str, len);
	return dup;
}

void* memory_alloc(size_t size) {
	void* ptr;
	if (size < 1) {
		printf("Can't alloc zero or negative memory");
		exit(-1);
	}
	if (size < 4) {
		size = 4;
	}
	ptr = calloc(1, size);
	if (ptr == NULL) {
		printf("Can't alloc more memory");
		exit(1);
	}
	return ptr;
}

void* memory_realloc(void* ptr, size_t size) {
	ptr = realloc(ptr, size);
	if (ptr == NULL) {
		printf("Can't realloc more memory, requested %zu", size);
		exit(1);
	}
	return ptr;
}

void memory_free(void* ptr) {
	*(unsigned int*)ptr = 0xDEADBEAF;
	free(ptr);
}







BUFFERIOHANDLE* bufferio_create() {
	BUFFERIOHANDLE* hd = memory_alloc(sizeof(BUFFERIOHANDLE));
	hd->size = hd->remaining = 2048;
	hd->buffer = memory_alloc(hd->size);
	hd->bufferptr = hd->buffer;
	return hd;
}

void bufferio_free(BUFFERIOHANDLE* hd) {
	memory_free(hd->buffer);
	memory_free(hd);
}

void bufferio_write(BUFFERIOHANDLE* hd, void* p, size_t len) {
	if (hd->remaining <= len) {
		size_t blen = hd->bufferptr - hd->buffer;
		size_t new_size = hd->size;
		do {
			new_size = (new_size << 1) + 8;
		} while ( new_size < (int)(blen + len));
		
		hd->buffer = memory_realloc(hd->buffer, new_size);
		hd->bufferptr = hd->buffer+blen;
		hd->remaining += new_size - hd->size;
		hd->size = new_size;
	}
	
	memcpy((void *)hd->bufferptr, (void *)p, len);
	hd->remaining -= len;
	hd->bufferptr += len;
}

void bufferio_align4(BUFFERIOHANDLE* hd) {
	char buffer[] = {0,0,0,0};
	size_t displacement = ((size_t)(hd->bufferptr - hd->buffer) % 4);
	if (displacement > 0) {
		bufferio_write(hd, buffer, 4-displacement);
	}
}

size_t bufferio_getsize(BUFFERIOHANDLE* hd) {
	size_t blen = hd->bufferptr - hd->buffer;
	return blen;
}

void* bufferio_getbuffer(BUFFERIOHANDLE* hd) {
	return hd->buffer;
}