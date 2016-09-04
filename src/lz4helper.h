/*
 * This file is part of tfsavcodec.
 * 
 * Copyright (c) 2016, Oskar Eisemuth
 * 
 * For the full copyright and license information,
 * please view the LICENSE file that was distributed with this source code.
 * 
 */

#ifndef LZ4HELPER_H
#define LZ4HELPER_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
bool decompressBuffer(void *inbuffer, size_t inlen, void **outbuffer, size_t* outlen);
bool compressBuffer(void *inbuffer, size_t inlen, void **outbuffer, size_t* outlen);

#ifdef __cplusplus
}
#endif

#endif /* LZ4HELPER_H */

