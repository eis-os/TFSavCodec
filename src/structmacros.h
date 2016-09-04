/*
 * This file is part of tfsavcodec.
 * 
 * Copyright (c) 2013-2016, Oskar Eisemuth
 * 
 * For the full copyright and license information,
 * please view the LICENSE file that was distributed with this source code.
 * 
 */

#ifndef STRUCTMACROS_H
#define	STRUCTMACROS_H

#include "noson/noson.h"
#include "tfstring.h"


#define PASTE(a, b) a ## b
#define CONCATE(a, b) PASTE(a, b)
#define NARG_(_4, _3, _2, _1, n, ...) n
#define NARG(...) NARG_(__VA_ARGS__, _4, _3, _2, _1, _0)


#define DEFINE_u8  uint8_t 
#define DEFINE_u16 uint16_t
#define DEFINE_u32 uint32_t
#define DEFINE_u64 uint64_t

#define DEFINE_s8  int8_t 
#define DEFINE_s16 int16_t
#define DEFINE_s32 int32_t
#define DEFINE_s64 int64_t

#define DEFINE_string char*
#define DEFINE_tfstring tfstring


#define DEFINE_field(type, name) DEFINE_##type name;
#define DEFINE_array(type, name, array) DEFINE_##type name[array];
#define DEFINE_dynarray(type, name, array) DEFINE_##type* name;

#define DEFINE_vector(numtype, numname, type, name) DEFINE_##numtype numname;	\
		type* name;

#define DEFINE_filepos(type, name) uint32_t name;
#define DEFINE_hidden(...) __VA_ARGS__;


#define STRUCT_DEFINE_MEMBER(x) DEFINE_##x

#define FORMAT_(type) FORMAT_##type
#define FORMAT_u8	"0x%02X"
#define FORMAT_u16	"0x%04X"
#define FORMAT_u32	"0x%08X"
#define FORMAT_u64	"0x%" PRIX64

#define FORMAT_s8	"%d"
#define FORMAT_s16	"%d"
#define FORMAT_s32	"%d"
#define FORMAT_s64	"%" PRIi64

#define FORMAT_string	"%s"
#define FORMAT_tfstring	"%d"

#define STRUCT_DEFINE(name, body) \
struct name {                  \
    body(STRUCT_DEFINE_MEMBER) \
} __attribute__((packed));     \
typedef struct name name;

#define _STRUCT_DUMP_IDENT printf("%*s", ident_level*2, "");

//#define STRUCT_DUMP_field(type, member) _STRUCT_DUMP_IDENT printf(#member ": " FORMAT_(type) "\n", obj->member);

#define STRUCT_DUMP_field_default(type, member) _STRUCT_DUMP_IDENT printf(#member ": " FORMAT_(type) "\n", obj->member);


#define STRUCT_DUMP_field_u8(type, member) STRUCT_DUMP_field_default(type, member)
#define STRUCT_DUMP_field_u16(type, member) STRUCT_DUMP_field_default(type, member)
#define STRUCT_DUMP_field_u32(type, member) STRUCT_DUMP_field_default(type, member)
#define STRUCT_DUMP_field_u64(type, member) STRUCT_DUMP_field_default(type, member)

#define STRUCT_DUMP_field_s8(type, member) STRUCT_DUMP_field_default(type, member)
#define STRUCT_DUMP_field_s16(type, member) STRUCT_DUMP_field_default(type, member)
#define STRUCT_DUMP_field_s32(type, member) STRUCT_DUMP_field_default(type, member)
#define STRUCT_DUMP_field_s64(type, member) STRUCT_DUMP_field_default(type, member)

#define STRUCT_DUMP_field_string(type, member) STRUCT_DUMP_field_default(type, member)

#define STRUCT_DUMP_field_tfstring(type, member)  \
	_STRUCT_DUMP_IDENT printf(#member ": (tfstr: %d) %s\n", obj->member.len, obj->member.str);

#define STRUCT_DUMP_field(type, member) STRUCT_DUMP_field_##type(type, member)




#define STRUCT_DUMP_filepos(type, member) _STRUCT_DUMP_IDENT printf(#member ": 0x%08X\n", obj->member);
#define STRUCT_DUMP_array(type, member, size) \
for (size_t member##_i = 0; member##_i < size; ++member##_i) { \
	_STRUCT_DUMP_IDENT printf(#member "%i: " FORMAT_(type) "\n", member##_i,obj->member[member##_i]); \
}



#define STRUCT_DUMP_vector(numtype, numname, type, name) \
	STRUCT_DUMP_field(numtype, numname) \
	_STRUCT_DUMP_IDENT printf(#name ": \n"); \
	for (size_t member##_i = 0; member##_i < obj->numname; ++member##_i) { \
		type##_dump(&obj->name[member##_i], ident_level+1);	\
	}


#define STRUCT_DUMP_dynarray(type, member, sizevar) \
for (size_t member##_i = 0; member##_i < obj->sizevar; ++member##_i) { \
	_STRUCT_DUMP_IDENT printf(#member "%i: " FORMAT_(type) "\n", member##_i, obj->member[member##_i]); \
}


#define STRUCT_DUMP_hidden(...)
#define STRUCT_DUMP_MEMBER(x) STRUCT_DUMP_##x


#define STRUCT_DUMP_OBJ(body) \
	struct_##body(STRUCT_DUMP_MEMBER)






// Noson Export

#define FORMAT_NONSON_field(type, name, value) FORMAT_NONSON_field_##type(name, value)

#define FORMAT_NONSON_field_u8(name, value)	var_create_int(name, value)
#define FORMAT_NONSON_field_u16(name, value)	var_create_int(name, value)
#define FORMAT_NONSON_field_u32(name, value)	var_create_int(name, value)
#define FORMAT_NONSON_field_u64(name, value)	var_create_int64(name, value)

#define FORMAT_NONSON_field_s8(name, value)	var_create_int(name, value)
#define FORMAT_NONSON_field_s16(name, value)	var_create_int(name, value)
#define FORMAT_NONSON_field_s32(name, value)	var_create_int(name, value)
#define FORMAT_NONSON_field_s64(name, value)	var_create_int64(name, value)

#define FORMAT_NONSON_field_string(name, value) var_create_string(name, value)
#define FORMAT_NONSON_field_tfstring(name, value)  var_create_string(name, value.str)



#define STRUCT_NONSON_field(type, member) var_add_child(v, FORMAT_NONSON_field(type, #member, obj->member));
#define STRUCT_NONSON_array(type, member, size)				\
VAR* member##_array = var_create(#member, VAR_TYPE_ARRAY);		\
for (size_t member##_i = 0; member##_i < size; ++member##_i) {		\
	var_add_child(member##_array, FORMAT_NONSON_field(type, 0, obj->member[ member##_i]));	\
} \
var_add_child(v, member##_array);


#define STRUCT_NONSON_vector(numtype, numname, type, member)		\
VAR* member##_vector = var_create(#member, VAR_TYPE_ARRAY);		\
for (size_t member##_i = 0; member##_i < obj->numname; ++member##_i) {	\
	var_add_child(member##_vector, type##_noson_export(&obj->member[member##_i]) );	\
}									\
var_add_child(v, member##_vector);


#define STRUCT_NONSON_filepos(...) 
#define STRUCT_NONSON_hidden(...)
#define STRUCT_NONSON_MEMBER(x) STRUCT_NONSON_##x


#define STRUCT_NONSON(body) \
	struct_##body(STRUCT_NONSON_MEMBER)


// Noson import
#define FORMAT_IMP_NONSON(type, objvar, var) FORMAT_IMP_NONSON_##type(objvar, var)

#define FORMAT_IMP_NONSON_u8(objvar, var)	objvar = (uint8_t)var_aslong(var)
#define FORMAT_IMP_NONSON_u16(objvar, var)	objvar = (uint16_t)var_aslong(var)
#define FORMAT_IMP_NONSON_u32(objvar, var)	objvar = (uint32_t)var_aslong(var)
#define FORMAT_IMP_NONSON_u64(objvar, var)	objvar = (uint64_t)var_asint64(var)


#define FORMAT_IMP_NONSON_s8(objvar, var)	objvar = (int8_t)var_aslong(var)
#define FORMAT_IMP_NONSON_s16(objvar, var)	objvar = (int16_t)var_aslong(var)
#define FORMAT_IMP_NONSON_s32(objvar, var)	objvar = (int32_t)var_aslong(var)
#define FORMAT_IMP_NONSON_s64(objvar, var)	objvar = (int64_t)var_asint64(var)


#define FORMAT_IMP_NONSON_string(objvar, var)   objvar = var_asstring(var)
#define FORMAT_IMP_NONSON_tfstring(objvar, var)   objvar.str = var_asstring(var); objvar.len = strlen(objvar.str);

#define STRUCT_IMP_NONSON_field(type, member) FORMAT_IMP_NONSON(type, obj->member, var_get_child(variable, #member));
#define STRUCT_IMP_NONSON_array(type, member, size) \
VAR* var_##member##children = var_get_children(var_get_child(variable, #member));	\
if (var_##member##children != 0) {							\
for (size_t member##_i = 0; member##_i < size; ++member##_i) {				\
	FORMAT_IMP_NONSON(type, obj->member[member##_i], var_##member##children);	\
	var_##member##children = var_##member##children->next;				\
	if (var_##member##children == 0) break;						\
}											\
}


#define STRUCT_IMP_NONSON_vector(numtyp, numname, type, name)					\
	obj->numname = var_get_children_count(var_get_child(variable, #name));			\
	VAR* var_##name##children = var_get_children(var_get_child(variable, #name));		\
	obj->name = memory_realloc(obj->name, sizeof(type)*(obj->numname));			\
	for (size_t name##_i = 0; name##_i < obj->numname; ++name##_i) {			\
		type##_noson_import(&(obj->name[name##_i]), var_##name##children);		\
		var_##name##children = var_##name##children->next;				\
		if (var_##name##children == 0) break;						\
	}											\





#define STRUCT_IMP_NONSON_filepos(...) 
#define STRUCT_IMP_NONSON_hidden(...)
#define STRUCT_IMP_NONSON_MEMBER(x) STRUCT_IMP_NONSON_##x


#define STRUCT_IMP_NONSON(body) \
	struct_##body(STRUCT_IMP_NONSON_MEMBER)


// Unserialize

#define _UNSERIALIZE_MEMBER_FD_field(typ, name) \
	file_readinto_##typ(fd, &(obj->name), 1);
#define _UNSERIALIZE_MEMBER_FD_array(typ, name, count) \
	file_readinto_##typ(fd, &(obj->name[0]), count);

#define _UNSERIALIZE_MEMBER_FD_dynarray(typ, name, sizevar) \
	obj->name = memory_realloc(obj->name, sizeof(typ)*obj->sizevar); \
	file_readinto_##typ(fd, obj->name, obj->sizevar);

#define _UNSERIALIZE_MEMBER_FD_vector(numtyp, numname, type, name) \
	_UNSERIALIZE_MEMBER_FD_field(numtyp, numname) \
	obj->name = memory_realloc(obj->name, sizeof(type)*(obj->numname)); \
	for (size_t name##_i = 0; name##_i < obj->numname; ++name##_i) { \
		type##_unserialize(fd, &(obj->name[name##_i]));	\
	}


#define _UNSERIALIZE_MEMBER_FD_filepos(typ, name) \
	obj->name = ftell(fd);
#define _UNSERIALIZE_MEMBER_FD_hidden(...) 

#define _UNSERIALIZE_MEMBER_FD(x) _UNSERIALIZE_MEMBER_FD_##x
	


#define OBJSTRUCT_UNSERIALIZE_FUNC(body)		\
void body##_unserialize(FILE* fd, body* obj) {		\
	if (obj == NULL) { print_err(0, "Trying to read into unalloced struct"); } \
	struct_##body(_UNSERIALIZE_MEMBER_FD)		\
}





// Serialize

#define _SERIALIZE_MEMBER_FD_field(typ, name) \
	file_write_##typ(fd, &(obj->name), 1);

#define _SERIALIZE_MEMBER_FD_array(typ, name, count) \
	file_write_##typ(fd, &(obj->name[0]), count);

#define _SERIALIZE_MEMBER_FD_vector(numtyp, numname, type, name) \
	_SERIALIZE_MEMBER_FD_field(numtyp, numname) \
	for (size_t name##_i = 0; name##_i < obj->numname; ++name##_i) { \
		type##_serialize(fd, &(obj->name[name##_i]));	\
	}

#define _SERIALIZE_MEMBER_FD_filepos(typ, name) \
	obj->name = ftell(fd);
#define _SERIALIZE_MEMBER_FD_hidden(...) 

#define _SERIALIZE_MEMBER_FD(x) _SERIALIZE_MEMBER_FD_##x

#define STRUCT_SERIALIZE_FD_OBJ(body) \
	if (obj == NULL) { print_err(0, "Trying to write unalloced struct"); } \
	struct_##body(_SERIALIZE_MEMBER_FD)


#define OBJSTRUCT_SERIALIZE_FUNC(body)			\
void body##_serialize(FILE* fd, body* obj) {		\
	if (obj == NULL) { print_err(0, "Trying to write unalloced struct"); }	\
	STRUCT_SERIALIZE_FD_OBJ(body)						\
}



#define OBJSTRUCT_CONSTRUCT(body) \
	body* body##_new() { return memory_alloc(sizeof(body)); }

#define OBJSTRUCT_ENSURE(body) \
	if (obj == 0) { obj = (body*)memory_alloc(sizeof(body)); }


#define OBJSTRUCT_DUMP_FUNC(body)	\
void body##_dump(body* obj, size_t ident_level) { \
	if (obj == NULL) return;	\
	STRUCT_DUMP_OBJ(body)		\
}


#define OBJSTRUCT_NSON_EXPORT_FUNC(body)	\
VAR* body##_noson_export(body* obj) {		\
	if (obj == NULL) return NULL;		\
	VAR* v = var_create(0, VAR_TYPE_MAP);	\
	STRUCT_NONSON(body)			\
	return v;				\
}

#define OBJSTRUCT_NSON_IMPORT_FUNC(body)		\
body* body##_noson_import(body* obj, VAR* variable) {	\
	if (variable == NULL) return NULL;		\
	if (obj == NULL) obj = body##_new();		\
	STRUCT_IMP_NONSON(body)				\
	return obj;					\
}


#endif	/* STRUCTMACROS_H */

