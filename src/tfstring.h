/*
 * This file is part of tfsavcodec.
 * 
 * Copyright (c) 2016, Oskar Eisemuth
 * 
 * For the full copyright and license information,
 * please view the LICENSE file that was distributed with this source code.
 * 
 */

#ifndef TFSTRING_H
#define TFSTRING_H

#ifdef __cplusplus
extern "C" {
#endif

struct s_TFSTRING {
	uint32_t len;
	char* str;
};

typedef struct s_TFSTRING tfstring;

#ifdef __cplusplus
}
#endif

#endif /* TFSTRING_H */

