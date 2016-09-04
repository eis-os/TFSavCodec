/*
 * This file is part of tfsavcodec.
 * 
 * Copyright (c) 2013-2016, Oskar Eisemuth
 * 
 * For the full copyright and license information,
 * please view the LICENSE file that was distributed with this source code.
 * 
 */

#ifndef NOSON_H
#define	NOSON_H
#include <stdint.h>

#define VAR_TYPE_UNKNOWN 0
#define VAR_TYPE_INT 1
#define VAR_TYPE_FLOAT 2
#define VAR_TYPE_STR 4
#define VAR_TYPE_INT64 8

#define VAR_TYPE_ARRAY 16
#define VAR_TYPE_MAP 32
#define VAR_TYPE_RAW 64

typedef struct _VAR VAR;
struct _VAR {
	char* name;
	int vtype;
	
	int64_t vint;
	float vfloat;
	char* vstr;

	int style;
	char* comment;
	struct _VAR *children;
	struct _VAR *childrenlast;
	struct _VAR *next;
};

void var_free(VAR* v);
VAR* var_create(char* name, int vtype);
VAR* var_create_int(char* name, int value);
VAR* var_create_int64(char* name, int64_t value);
VAR* var_create_float(char* name, float value);
VAR* var_create_string(char* name, char* value);
VAR* var_create_string_n(char* name, char* value, size_t len);

VAR* var_set_name(VAR* v, char* name);

VAR* var_set_int(VAR* v, int value);
VAR* var_set_int64(VAR* v, int64_t value);

VAR* var_set_float(VAR* v, int value);
VAR* var_set_string(VAR* v, char* value);
VAR* var_set_raw(VAR* v, void* buffer, size_t buffersize);

VAR* var_change_style(VAR* v, int style);
VAR* var_comment(VAR* v, char* comment);

VAR* var_get(VAR* v, char* name);

char* var_asstring(VAR* v);
long var_aslong(VAR* v);

int64_t var_asint64(VAR* v);

float var_asfloat(VAR* v);

VAR* var_add_child(VAR* v, VAR* child);
VAR* var_remove_child(VAR* v, VAR* child);
VAR* var_get_child(VAR* v, char* name);
VAR* var_getornew_child(VAR* v, char* name);
VAR* var_get_children(VAR* v);
VAR* var_get_firstchild(VAR* v);
VAR* var_get_lastchild(VAR* v);

int var_get_children_count(VAR* v);

//VAR* var_move_children(VAR* v, VAR* oldparent);

VAR* var_get_map_children(VAR* v);
VAR* var_get_array_children(VAR* v);

void var_export(VAR *v, FILE* fd, int level);
void var_export_file(VAR *v, char* filename);

VAR* var_import_file(char* filename);
#endif	/* NOSON_H */

