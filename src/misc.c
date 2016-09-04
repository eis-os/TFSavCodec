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
#include <stdarg.h>
#include <strings.h>

void print(unsigned int indent_level, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	printf("%*s", (int)indent_level*2, "");
	vprintf(fmt, ap);
	va_end(ap);
}

void print_err(unsigned int indent_level, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	printf("%*s", (int)indent_level*2, "ERROR: ");
	vprintf(fmt, ap);
	va_end(ap);
}

void print_warn(unsigned int indent_level, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	printf("%*s", (int)indent_level*2, "WARN: ");
	vprintf(fmt, ap);
	va_end(ap);
}
