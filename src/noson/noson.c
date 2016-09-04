/*
 * This file is part of tfsavcodec.
 * 
 * Copyright (c) 2013-2016, Oskar Eisemuth
 * 
 * For the full copyright and license information,
 * please view the LICENSE file that was distributed with this source code.
 * 
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>	//isspace

#include <stdarg.h>
#include <inttypes.h>
#include <basetsd.h>

#include "noson.h"
#include "../memfunc.h"

void var_free(VAR* v) {
	VAR* ptr, *item;
	if (v) {
		if (v->name) {
			free(v->name);
		}
		if (v->vstr) {
			free(v->vstr);
		}
		if (v->comment) {
			free(v->comment);
		}
		ptr = v->children;
		/* while (ptr != 0) {
			item = ptr;
			ptr = ptr->next;
			free(item);
		} */
		
		while (ptr != 0) {
			item = ptr;
			ptr = ptr->next;
			var_free(item);
		}
		free(v);
	}
}

VAR* var_create(char* name, int vtype) {
	VAR* v;
	v = calloc(1, sizeof (VAR));
	if (v) {
		if (name) {
			v->name = memory_strdup(name);
		}
		v->vtype = vtype;
	}
	return v;
}

VAR* var_create_int(char* name, int value) {
	VAR* v;
	v = calloc(1, sizeof (VAR));
	if (v) {
		if (name) {
			v->name = memory_strdup(name);
		}
		v->vtype = VAR_TYPE_INT;
		v->vint = value;
	}
	return v;
}


VAR* var_create_int64(char* name, int64_t value) {
	VAR* v;
	v = calloc(1, sizeof (VAR));
	if (v) {
		if (name) {
			v->name = memory_strdup(name);
		}
		v->vtype = VAR_TYPE_INT64;
		v->vint = value;
	}
	return v;
}



VAR* var_create_string(char* name, char* value) {
	VAR* v;
	v = calloc(1, sizeof (VAR));
	if (v) {
		if (name) {
			v->name = memory_strdup(name);
		}
		v->vtype = VAR_TYPE_STR;
		if (value) {
			v->vstr = memory_strdup(value);
		}
	}
	return v;
}

VAR* var_create_string_n(char* name, char* value, size_t len) {
	VAR* v;
	v = calloc(1, sizeof (VAR));
	if (v) {
		if (name) {
			v->name = memory_strdup(name);
		}
		v->vtype = VAR_TYPE_STR;
		if (value) {
			if (len > 0) {
				v->vstr = calloc(len+1, 1);
				memcpy(v->vstr, value, len);
			}
			
		}
	}
	return v;
}


VAR* var_create_float(char* name, float value) {
	VAR* v;
	v = calloc(1, sizeof (VAR));
	if (v) {
		if (name) {
			v->name = memory_strdup(name);
		}
		v->vtype = VAR_TYPE_FLOAT;
		v->vfloat = value;
	}
	return v;
}


VAR* var_set_name(VAR* v, char* name) {
	if (v) {
		if (name) {
			v->name = memory_strdup(name);
		}
	}
	return v;
}

VAR* var_set_int(VAR* v, int value) {
	if (v) {
		v->vtype = VAR_TYPE_INT;
		v->vint = value;
		v->vfloat = 0;
		if (v->vstr) {
			free(v->vstr);
			v->vstr = NULL;
		}
	}
	return v;
}

VAR* var_set_int64(VAR* v, int64_t value) {
	if (v) {
		v->vtype = VAR_TYPE_INT64;
		v->vint = value;
		v->vfloat = 0;
		if (v->vstr) {
			free(v->vstr);
			v->vstr = NULL;
		}
	}
	return v;
}


VAR* var_set_string(VAR* v, char* value) {
	if (v) {
		v->vtype = VAR_TYPE_STR;
		v->vint = 0;
		v->vfloat = 0;
		if (v->vstr) {
			free(v->vstr);
			v->vstr = NULL;
		}
		if (value) {
			v->vstr = memory_strdup(value);
		}
	}
	return v;
}



VAR* var_set_float(VAR* v, int value) {
	if (v) {
		v->vtype = VAR_TYPE_FLOAT;
		v->vint = 0;
		v->vfloat = value;
		if (v->vstr) {
			free(v->vstr);
			v->vstr = NULL;
		}
	}
	return v;
}

VAR* var_set_raw(VAR* v, void* buffer, size_t buffersize) {
	if (v) {
		v->vtype = VAR_TYPE_RAW;
		v->vint = buffersize;
		v->vfloat = 0;
		if (v->vstr) {
			free(v->vstr);
		}
		v->vstr = buffer;
	}
	return v;
}

VAR* var_get(VAR* v, char* name) {
	while (v) {
		if (v->name != 0) {
			if (strcmp(v->name, name) == 0) {
				return v;
			}
		}
		v = v->next;
	}
	return 0;
}

char* var_asstring(VAR* v) {
	if (v) {
		if (v->vtype == VAR_TYPE_STR) {
			return v->vstr;
		}
	}
	return 0;
}

long var_aslong(VAR* v) {
	if (v) {
		if (v->vtype == VAR_TYPE_INT) {
			return v->vint;
		}
		if (v->vtype == VAR_TYPE_INT64) {
			if (v->vint > UINT32_MAX) {
				return 0;
			} else {
				return (int32_t)v->vint;
			}
		}
	}
	return 0;
}

int64_t var_asint64(VAR* v) {
	if (v) {
		if (v->vtype == VAR_TYPE_INT64 || v->vtype == VAR_TYPE_INT) {
			return v->vint;
		}
	}
	return 0;
}


float var_asfloat(VAR* v) {
	if (v) {
		if (v->vtype == VAR_TYPE_FLOAT) {
			return v->vfloat;
		}
	}
	return 0;
}

VAR* var_add_child(VAR* v, VAR* child) {
	VAR* ptr;
	if (v) {
		if (!v->children) {
			v->children = child;
		}
		if (v->childrenlast) {
			v->childrenlast->next = child;
		}
		ptr = child;
		while (ptr->next != 0) {
			ptr = ptr->next;
		}
		v->childrenlast = ptr;
	}
	return v;
}

VAR* var_remove_child(VAR* v, VAR* child) {
	VAR* ptr;
	VAR** prev_pptr;
	if (!child) return 0;
	if (v) {
		if (!v->children) return 0;
		ptr = v->children;
		prev_pptr = &v->children;
		while (ptr) {
			if (ptr == child) {
				*prev_pptr = ptr->next; 
				ptr->next = 0;
				return ptr;
			}
			prev_pptr = &ptr->next; 
			ptr = ptr->next;
		}
	}
	return 0;
}

VAR* var_replace_child(VAR* v, VAR* vnew, char* name) {
	VAR* ptr;
	if (v) {
		if (!v->children) return 0;
		ptr = v->children;
		while (ptr) {
			if (ptr->name != 0) {
				if (strcmp(ptr->name, name) == 0) {
					vnew->next = ptr->next;
					
				}
			}
			ptr = ptr->next;
		}
		var_add_child(v, vnew);
	}
	return vnew;
}
/* untested
VAR* var_move_children(VAR* v, VAR* oldparent) {
	VAR* ptr;
	if (v) {
		VAR* child = oldparent->children;
		oldparent->children = NULL;
		oldparent->childrenlast = NULL;
		
		if (!v->children) {
			v->children = child;
		}
		if (v->childrenlast) {
			v->childrenlast->next = child;
		}
		ptr = child;
		while (ptr->next != 0) {
			ptr = ptr->next;
		}
		v->childrenlast = ptr;
	}
	return v;
}
*/

VAR* var_get_child(VAR* v, char* name) {
	VAR* ptr;
	if (v) {
		if (!v->children) return 0;
		ptr = v->children;
		while (ptr) {
			if (ptr->name != 0) {
				if (strcmp(ptr->name, name) == 0) {
					return ptr;
				}
			}
			ptr = ptr->next;
		}
	}
	return 0;
}

VAR* var_getornew_child(VAR* v, char* name) {
	VAR* ptr = var_get_child(v,name);
	if (ptr) {
		return ptr;
	}
	ptr = var_create(name, 0);
	var_add_child(v, ptr);
	return ptr;
}

VAR* var_get_children(VAR* v) {
	if (v) {
		if (v->vtype == VAR_TYPE_MAP || v->vtype == VAR_TYPE_ARRAY) {
			return v->children;
		}
	}
	return 0;
}

VAR* var_get_firstchild(VAR* v) {
	if (v) {
		if (v->vtype == VAR_TYPE_MAP || v->vtype == VAR_TYPE_ARRAY) {
			return v->children;
		}
	}
	return 0;
}
VAR* var_get_lastchild(VAR* v) {
	if (v) {
		if (v->vtype == VAR_TYPE_MAP || v->vtype == VAR_TYPE_ARRAY) {
			if (!v->children) return 0;
			return v->childrenlast;
		}
	}
	return 0;
}


int var_get_children_count(VAR* v) {
	VAR* ptr;
	int i = 0;
	if (v) {
		if (!v->children) return 0;
		ptr = v->children;
		while (ptr) {
			++i;
			ptr = ptr->next;
		}
		return i;
	}
	return 0;
}

VAR* var_get_map_children(VAR* v) {
	if (v) {
		if (v->vtype == VAR_TYPE_MAP) {
			return v->children;
		}
	}
	return 0;
}

VAR* var_get_array_children(VAR* v) {
	if (v) {
		if (v->vtype == VAR_TYPE_ARRAY) {
			return v->children;
		}
	}
	return 0;
}

VAR* var_change_style(VAR* v, int style) {
	v->style = style;
	return v;
}

VAR* var_comment(VAR* v, char* comment) {
	v->comment = memory_strdup(comment);
	return v;
}






void _var_writestring(FILE* fd, char* str);

void _var_export_export_hex(FILE* fd, char* buffer, size_t bufferlen) {
	const char* hex = "0123456789ABCDEF";
	char* outbuffer;
	char* pout;
	char* pin;
	 
	outbuffer = malloc(bufferlen*3+1);
	pin = buffer;
	pout = outbuffer;
	
	for(int i = 0; i < bufferlen; ++i){
		*pout = hex[(*pin>>4)&0xF];
		pout++;
		*pout = hex[(*pin)&0xF];
		pout++;
		pin++;
	}
	fwrite(outbuffer, 1, pout-outbuffer, fd);
	free(outbuffer);
}


void var_export(VAR *v, FILE* fd, int level) {
	while (v) {
		if (v->comment) {
			fprintf(fd, "%*s/* %s */\n", level, "", v->comment);
		}
		if (v->name) {
			fprintf(fd, "%*s\"%s\": ", level, "", v->name);
		} else {
			fprintf(fd, "%*s", level, "");
		}
		switch (v->vtype) {
			case VAR_TYPE_INT64:
				if (v->style == 0) {
					fprintf(fd, "%" PRIi64, v->vint);
					break;
				}
				if (v->style == 8) {
					fprintf(fd, "0x%016" PRIX64, v->vint);
					break;
				}
			case VAR_TYPE_INT:
				if (v->style == 4) {
					fprintf(fd, "0x%08X", (int)v->vint);
					break;
				}
				if (v->style == 2) {
					fprintf(fd, "0x%04X", (int)v->vint);
				}
				if (v->style == 1) {
					fprintf(fd, "0x%02X", (int)v->vint);
				}
				fprintf(fd, "%i", (int)v->vint);
				break;
			case VAR_TYPE_FLOAT:
				fprintf(fd, "%.20f", v->vfloat);
				break;
			case VAR_TYPE_STR:
				if (v->vstr) {
					//fprintf(fd, "\"%s\"", v->vstr);
					_var_writestring(fd, v->vstr);
				} else {
					fprintf(fd, "null");
				}
				break;
			case VAR_TYPE_ARRAY:
				fprintf(fd, "[\n");
				var_export(v->children, fd, level + 2);
				fprintf(fd, "%*s]", level, "");
				break;
			case VAR_TYPE_MAP:
				fprintf(fd, "{\n");
				var_export(v->children, fd, level + 2);
				fprintf(fd, "%*s}", level, "");
				break;
			case VAR_TYPE_RAW:
				fprintf(fd, "hex(\"");
				_var_export_export_hex(fd, v->vstr, v->vint);
				fprintf(fd, "\")");
				break;
			default:
				fprintf(fd, "UNKNOWN TYPE");
		}
		v = v->next;
		if (v) {
			fprintf(fd, ",\n");
		} else {
			fprintf(fd, "\n");
		}
	}
}

void var_export_file(VAR *v, char* filename) {
	FILE* fd;
	printf("Writing file %s\n", filename);
	fd = fopen(filename, "wb");
	if (fd == NULL) {
		printf("Writing file %s, %s\n", filename, strerror(errno));
		exit(-1);
	}
	var_export(v, fd, 0);
	fclose(fd);
}


static int _var_parser(VAR* parent, char** s, int *line);

VAR* var_import_file(char* filename) {
	VAR* v = NULL;
	FILE* fd;
	size_t filesize;
	int line;
	char* buffer;
	char* bufferptr;

	printf("Reading file %s\n", filename);
	fd = fopen(filename, "rb");
	if (fd == NULL) {
		printf("Reading file %s, %s\n", filename, strerror(errno));
		exit(-1);
	}
	fseek(fd, 0, SEEK_END);
	filesize = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	buffer = memory_alloc(filesize + 1024);
	bufferptr = buffer;
	if (fread(buffer, 1, filesize, fd) == filesize) {
		line = 1;
		VAR* vx = var_create(0, VAR_TYPE_UNKNOWN);
		if (_var_parser(vx, &bufferptr, &line)) {
			exit(-1);
		}
		v = vx->children;
		vx->children = NULL;
		var_free(vx);
	}
	memory_free(buffer);
	fclose(fd);
	return v;
}



/* Parsing Files */

static char* _var_readstring(char** s, int *line);
static void spaceskip(char** s, int *line);
static void skipcomment_toend(char** s, int *line);


int _var_buffer_resize(char** bufferptr, size_t *size, size_t new_size) {
	char* buffernew = NULL;
	//size_t new_size = *size<<1;

	buffernew = realloc(*bufferptr, new_size);
	if (!buffernew) {
		printf("Can't realloc buffer, oldsize: %u, newsize: %zu\n", *size, new_size);
		return 1;
	}
	*size = new_size;
	*bufferptr = buffernew;
	return 0;
}

void _var_parser_printinfo(int line, char *s, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	printf("Parser error at line %i: ", line);
	vprintf(fmt, ap);
	if (s) {
		printf(">>>>%.*s<<<<\n\n", 100, s);
	}
	va_end(ap);
}


static int _var_parser(VAR* parent, char** s, int *line) {
	VAR *v = NULL;
	char *varname = NULL;
	char *varstring = NULL;
	char type;
	
	while (**s != 0) {
		spaceskip(s, line);	
		// Is this numeric?
		if (**s == '-' || (**s >= '0' && **s <= '9')) {
			/* char* sold = *s;
			errno = 0;
			long value = strtoul(*s, s, 0);
			if (sold == *s) {
				// failing to convert, try skip
				while (**s && ((**s >= '0' && **s <= '9') || **s == '.')) {
					(*s)++;
				}
				return 1;
			} else {
				if (errno == 0) {
					v = var_create_int(varname, value);
					free(varname);
					varname = NULL;
					var_add_child(parent, v);
					spaceskip(s, line);
				}
			}
			*/
			char* sold = *s;
			int numfloat = 0;
			int numhex = 0;
			int isneg = (**s == '-');
			errno = 0;
			
			// first char is 0..9 or -
			(*s)++;
			// could be hex
			if (**s == 'x') {
				numhex = 1;
				(*s)++;
			}
			
			while (**s && ((**s >= '0' && **s <= '9') || **s == '.')) {
				if (**s == '.') {
					numfloat = 1;
				}
				(*s)++;
			}
			
			if (numfloat && numhex) {
				//printf("Parser error at line %i, unexpected mixed hex/float value\n", *line);
				_var_parser_printinfo(*line, *s, "unexpected mixed hex/float value\n");
				return 1;
			}
			*s = sold;
			
			if (numfloat) {
				v = var_create_float(varname, strtof(*s, s));
			} else {
				if (isneg) {
					v = var_create_int64(varname, strtoll(*s, s, 0));
				} else {
					v = var_create_int64(varname, strtoull(*s, s, 0));
				}
			}
			if ( (sold == *s) || (errno != 0) ) {
				//printf("Parser error at line %i, unexpected mixed value\n", *line);
				_var_parser_printinfo(*line, *s, "unexpected mixed value\n");
				return 1;
			}
			free(varname);
			varname = NULL;
			var_add_child(parent, v);
			spaceskip(s, line);
		}
		switch (**s) {
			case ',':
				if (varname != NULL) {
					//printf("Parser error at line %i, unexpected comma without variable value after :\n", *line);
					_var_parser_printinfo(*line, *s, "unexpected comma without variable value after :\n");
					return 1;
				}
				if (parent->vtype == VAR_TYPE_ARRAY || parent->vtype == VAR_TYPE_MAP) {

				} else {
					//printf("Parser error at line %i, unexpected comma without array or object\n", *line);
					_var_parser_printinfo(*line, *s, "unexpected comma without array or object\n");
				}
				break;
			case ':':
				//printf("Parser error at line %i, unexpected :\n", *line);
				_var_parser_printinfo(*line, *s, "unexpected :\n");
				return 1;
			case '"':
			case '\'':
				varstring = _var_readstring(s, line);
				if (varstring) {
					spaceskip(s, line);
					//printf("Found string: %s\n", varstring);
					if (**s == ':') {
						//printf("Found comma after %s\n", varstring);
						// VAR_TYPE_UNKNOWN for files starting with "varname": {
						if ((varname == NULL) && (parent->vtype == VAR_TYPE_MAP || parent->vtype == VAR_TYPE_ARRAY || parent->vtype == VAR_TYPE_UNKNOWN)) {
							varname = varstring;
							(*s)++;
						} else {
							//printf("Parser error at line %i, unexpected key definition, unexpected :,\n%.*s\n", *line, 100, *s);
							_var_parser_printinfo(*line, *s, "unexpected key definition, unexpected :,\n");
							return 1;
						}
					} else {
						v = var_create_string(varname, varstring);
						free(varname);
						varname = NULL;
						var_add_child(parent, v);
					}
				}
				//printf("Character after string ##%.20s##\n", *s);
				(*s)--;
				break;
			case 'n':
				if (strncmp(*s, "null", 4) != 0) {
					// jump to default case
					goto parseerror;
				}
				(*s)+= 4;
				v = var_create_string(varname, 0);
				free(varname);
				varname = NULL;
				var_add_child(parent, v);
				break;
			case '{':
			case '[':
				type = **s;
				(*s)++;
				spaceskip(s, line);
				v = var_create(varname, (type == '{' ? VAR_TYPE_MAP : VAR_TYPE_ARRAY));
				switch (type) {
					case '{':
						type = '}';
						break;
					case '[':
						type = ']';
						break;
				}
				
				//printf("Creating Array/Map at line %i, expecting %c for closing, %.20s\n", *line, type, *s);
				free(varname);
				varname = NULL;
				_var_parser(v, s, line);
				if (**s == 0) {
					//printf("EOF, missing %c\n", *line, type);
					_var_parser_printinfo(*line, *s, "EOF, missing %c\n",type);
					exit(-1);
				} else if (type != **s) {
					
					_var_parser_printinfo(*line, *s, "missing %c\n",type);
					
					//printf("Parser error at line %i, missing %c\n", *line, type);
					//printf("Following bytes: ##>%.10s<##\n",*s);
					
					//exit(-1);
					return 1;
				}
				//printf("Closing Array/Map at line %i with type ##%c##\n", *line, type);
				//printf("Parsing @ line %i, %.20s\n", *line, *s);
				var_add_child(parent, v);
				break;
			case '}':
				//printf("Parser error at line %i, unexpected }\n", *line);
				return 0;
			case ']':
				//printf("Parser error at line %i, unexpected ]\n", *line);
				return 0;
			case '\0':
				return 0;
			case 'h':
				if (strncmp(*s, "hex(", 4) == 0) {
					(*s)+=4;
					VAR* _var_import_hex(char** s, int *line);
					v = _var_import_hex(s, line);
					if (v) {		
						if (**s != ')') {
							_var_parser_printinfo(*line, *s, "expected closing bracket ) for opening hex(\n");
							return 1;
						}
						var_set_name(v, varname);
						free(varname);
						varname = NULL;
						var_add_child(parent, v);
						break;
					}
				}
				
			default:
parseerror:
				_var_parser_printinfo(*line, *s, "unexpected 0x%02X\n",**s);
				return 1;

		}
		(*s)++;
	}
	return 0;	
}

unsigned char _var_import_hex_digittoint(char *s, int *line, int len) {
	unsigned char d = *s;
	if (d >= '0' && d <= '9') {
		return d-'0';	
	}
	if (d >= 'A' && d <= 'H') {
		return d-('A'-10);
	}
	if (d >= 'a' && d <= 'h') {
		return d-('a'-10);
	}
	// Unknown hex digit

	//_var_parser_printinfo(*line, s, "Can't read hex data, unknown char 0x%02X\n", *s);
	_var_parser_printinfo(*line, s, "Can't read hex data, unknown char 0x%02X\n", *s);
	exit(-1);
	
	//return (d & 0x1f) + ((d >> 6) * 0x19) - '0';
}

VAR* _var_import_hex(char** s, int *line) {
	VAR* v;
	char* outbuffer = NULL;
	size_t outbuffersize = 2048;
	int len = 0;
	if (**s != '"') {
		_var_parser_printinfo(*line, *s, "Expected start of string\n");
		return 0;
	}
	(*s)++;
	outbuffer = malloc(outbuffersize);
	while (**s && (**s != '"')) {
		outbuffer[len] = (_var_import_hex_digittoint(*s, line, len) << 4) | (_var_import_hex_digittoint((*s)+1, line, len) );
		(*s)+=2;
		len++;
		if (len >= outbuffersize) {
			/* outbuffersize+=2048;
			outbuffer = realloc(outbuffer, outbuffersize);
			if (outbuffer == NULL) {
				_var_parser_printinfo(*line, *s, "Can't import hex data, no enough memory to alloc\n");
			} */
			if (_var_buffer_resize(&outbuffer, &outbuffersize, outbuffersize+2048)) {
				_var_parser_printinfo(*line, *s, "Can't import hex data, no enough memory to alloc\n");
			}
		}
	}
	if (**s != '"') {
		free(outbuffer);
		_var_parser_printinfo(*line, *s, "Can't import hex data, closing string missing\n");
	}
	(*s)++;
	v = var_create(0, VAR_TYPE_RAW);
	v->vstr = outbuffer;
	v->vint = len;
	return v;
}

char* _var_readstring_old(char** s, int *line) {
	const char* start = *s + 1;
	char* ret = NULL;
	char c, qchar;
	size_t len;

	qchar = **s;
	if (qchar != '\"' && qchar != '\'') return NULL;
	(*s)++;

	while ( (c = **s) ) {
		if (c == '\n') (*line)++;
		if (c == '\\') {
			(*s)++;
		} else {
			if (c == qchar) {
				(*s)++;
				break;
			}
		}

		(*s)++;
	}
	len = (*s - start);
	if (len > 1) {
		ret = calloc(len + 1, 1);
		memcpy(ret, start, len - 1);
		return ret;
	}
	return NULL;
}


char* _var_readstring(char** s, int *line) {
	char* ret = NULL;
	char c, qchar;
	size_t len = 0;
	size_t buffersize = 512;

	qchar = **s;
	if (qchar != '\"' && qchar != '\'') return NULL;
	(*s)++;

	ret = malloc(buffersize);
	if (ret == 0) {
		_var_parser_printinfo(*line, *s, "Can't read string, can't alloc memory\n");
		exit(-1);
	}
	while ( (c = **s) ) {
		if (len >= buffersize) {
			if (_var_buffer_resize(&ret, &buffersize, buffersize<<1)) {
				_var_parser_printinfo(*line, *s, "Can't read string, can't realloc memory\n");
				exit(-1);
			}
		}
		if (c == '\n') {
			(*line)++;
		} else if (c == qchar) {
			(*s)++;
			break;
		} else if (c == '\\') {
			(*s)++;
			c = **s;
			switch(c) {
				case '"':
				case '\\':
				case '/':
					break;
				case 'b':
					c = '\b';
					break;
				case 'f':
					c = '\f';
					break;
				case 'n':
					c = '\n';
					break;
				case 'r':
					c = '\r';
					break;
				case 't':
					c = '\t';
					break;
				case 'u':
					_var_parser_printinfo(*line, *s, "Can't read string, unicode escapes not supported\n");
					exit(-1);
					break;
			}
		}
		ret[len] = c;
		len++;
		(*s)++;
	}
	// either expand or shrink
	if (_var_buffer_resize(&ret, &buffersize, len+8)) {
		_var_parser_printinfo(*line, *s, "Can't read string, can't realloc memory\n");
		exit(-1);
	}
	ret[len++] = 0;
	
	//TODO: CHECK EMTPY STRING AND NULL STRINGS working correct!
	if (len > 0) {
		return ret;
	}
	free(ret);
	return NULL;
}


void _var_writestring(FILE* fd, char* str) {
	char* buffer;
	char c;
	size_t len = 0;
	size_t buffersize = 512;
	
	buffer = malloc(buffersize);
	
	buffer[len++] = '"';
	while ( (c = *str) ) {
		if (len+8 > buffersize) {
			if (_var_buffer_resize(&buffer, &buffersize, buffersize<<1)) {
				exit(-1);
			}
		}
		switch(c) {
			case '"':
			case '\\':
			case '/':
				buffer[len++] = '\\';
				break;
			case '\b':
				buffer[len++] = '\\';
				c = 'b';
				break;
			case '\f':
				buffer[len++] = '\\';
				c = 'f';
				break;
			case '\n':
				buffer[len++] = '\\';
				c = 'n';
				break;
			case '\r':
				buffer[len++] = '\\';
				c = 'r';
				break;
			case '\t':
				buffer[len++] = '\\';
				c = 't';
				break;
		}
		buffer[len] = c;
		len++;
		str++;
	}
	buffer[len++] = '"';
	fwrite(buffer, len, 1, fd);
	free(buffer);
}


static void skipline(char** s, int *line) {
	while (**s) {
		if (**s == '\n') {
			(*line)++;
			(*s)++;
			break;
		}
		(*s)++;
	}
}

static void skipcomment_toend(char** s, int *line) {
	if (**s == '*') {
		(*s)++;
		while (**s) {
			if (**s == '\n')
				(*line)++;
			if (**s == '*' && *(*s + 1) == '/') {
				(*s) += 2;
				break;
			}
			(*s)++;
		}
	} else if (**s == '/') {
		skipline(s, line);
	}
}

static void spaceskip(char** s, int *line) {
	while (**s) {
		if (**s == '\n') {
			(*line)++;
			(*s)++;
			continue;
		} else if (isspace(**s)) {
			(*s)++;
			continue;
		} else if (**s == '/') {
			(*s)++;
			// maybe comment
			skipcomment_toend(s, line);
			continue;
		}
		break;
	}
}
