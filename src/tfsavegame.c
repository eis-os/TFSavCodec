/*
 * This file is part of tfsavcodec.
 * 
 * Copyright (c) 2016, Oskar Eisemuth
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

#include <stdbool.h>

#include "misc.h"
#include "memfunc.h"
#include "filefunc.h"

#include "tfsavegamestruct.h"

extern int verbose;


#define OBJSTRUCT_READER(type)						\
type* type##_read(FILE* fd) {						\
	if(verbose > 20) print(0, "Reading " #type ":\n");		\
	type* obj = type##_new();					\
									\
	type##_unserialize(fd, obj);					\
									\
	if (verbose > 20) {						\
		type##_dump(obj, 1);					\
	}								\
	return obj;							\
}


#define OBJSTRUCT_WRITER(type)						\
void type##_write(FILE* fd, type* obj) {				\
	if(verbose > 20) print(0, "Writing " #type ":\n");		\
	if (verbose > 20) {						\
		type##_dump(obj, 1);					\
	}								\
	type##_serialize(fd, obj);					\
}



OBJSTRUCT_CONSTRUCT(TFModDisplayString)
OBJSTRUCT_DUMP_FUNC(TFModDisplayString)
OBJSTRUCT_UNSERIALIZE_FUNC(TFModDisplayString)
OBJSTRUCT_SERIALIZE_FUNC(TFModDisplayString)
OBJSTRUCT_NSON_EXPORT_FUNC(TFModDisplayString)
OBJSTRUCT_NSON_IMPORT_FUNC(TFModDisplayString)



OBJSTRUCT_CONSTRUCT(TFHeader)
OBJSTRUCT_DUMP_FUNC(TFHeader)
OBJSTRUCT_UNSERIALIZE_FUNC(TFHeader)
OBJSTRUCT_SERIALIZE_FUNC(TFHeader)
OBJSTRUCT_NSON_EXPORT_FUNC(TFHeader)
OBJSTRUCT_NSON_IMPORT_FUNC(TFHeader)


OBJSTRUCT_WRITER(TFHeader)

TFHeader* TFHeader_read(FILE* fd) {
	if(verbose > 20) print(0, "Reading TFHeader:\n");
	TFHeader* obj = NULL;
	obj = TFHeader_new();
	
	TFHeader_unserialize(fd, obj);
	
	if (memcmp(obj->signature, "tf**", 4) != 0) {
		print_err(1, "TF signature not found, are you sure the file is a valid savegame?");
		return NULL;
	}
	if (obj->savegameversion != TFSAVEGAMEVERSION) {
		print_warn(1, "Expected savegame version %u, forund %u", TFSAVEGAMEVERSION, obj->savegameversion);
		return NULL;
	}
	if (verbose > 20) {
		TFHeader_dump(obj, 1);
	}
	return obj;
}



OBJSTRUCT_CONSTRUCT(TFModEntry)
OBJSTRUCT_DUMP_FUNC(TFModEntry)
OBJSTRUCT_UNSERIALIZE_FUNC(TFModEntry)
OBJSTRUCT_SERIALIZE_FUNC(TFModEntry)
OBJSTRUCT_NSON_EXPORT_FUNC(TFModEntry)
OBJSTRUCT_NSON_IMPORT_FUNC(TFModEntry)

OBJSTRUCT_CONSTRUCT(TFMods)
OBJSTRUCT_DUMP_FUNC(TFMods)
OBJSTRUCT_UNSERIALIZE_FUNC(TFMods)
OBJSTRUCT_SERIALIZE_FUNC(TFMods)
OBJSTRUCT_NSON_EXPORT_FUNC(TFMods)
OBJSTRUCT_NSON_IMPORT_FUNC(TFMods)


OBJSTRUCT_READER(TFMods)
OBJSTRUCT_WRITER(TFMods)


// -------------------  TFSettingsConfig


OBJSTRUCT_CONSTRUCT(TFKeyValueString)
OBJSTRUCT_DUMP_FUNC(TFKeyValueString)
OBJSTRUCT_UNSERIALIZE_FUNC(TFKeyValueString)
OBJSTRUCT_SERIALIZE_FUNC(TFKeyValueString)
OBJSTRUCT_NSON_EXPORT_FUNC(TFKeyValueString)
OBJSTRUCT_NSON_IMPORT_FUNC(TFKeyValueString)

OBJSTRUCT_CONSTRUCT(TFSettingsConfig)
OBJSTRUCT_DUMP_FUNC(TFSettingsConfig)
OBJSTRUCT_UNSERIALIZE_FUNC(TFSettingsConfig)
OBJSTRUCT_SERIALIZE_FUNC(TFSettingsConfig)
OBJSTRUCT_NSON_EXPORT_FUNC(TFSettingsConfig)
OBJSTRUCT_NSON_IMPORT_FUNC(TFSettingsConfig)

OBJSTRUCT_READER(TFSettingsConfig)
OBJSTRUCT_WRITER(TFSettingsConfig)


OBJSTRUCT_CONSTRUCT(TFAfterSettings)
OBJSTRUCT_DUMP_FUNC(TFAfterSettings)
OBJSTRUCT_UNSERIALIZE_FUNC(TFAfterSettings)
OBJSTRUCT_SERIALIZE_FUNC(TFAfterSettings)
OBJSTRUCT_NSON_EXPORT_FUNC(TFAfterSettings)
OBJSTRUCT_NSON_IMPORT_FUNC(TFAfterSettings)

OBJSTRUCT_READER(TFAfterSettings)
OBJSTRUCT_WRITER(TFAfterSettings)



// -------------------  TFModelRep


OBJSTRUCT_CONSTRUCT(TFModelRepEntry)
OBJSTRUCT_DUMP_FUNC(TFModelRepEntry)
OBJSTRUCT_UNSERIALIZE_FUNC(TFModelRepEntry)
OBJSTRUCT_SERIALIZE_FUNC(TFModelRepEntry)
OBJSTRUCT_NSON_EXPORT_FUNC(TFModelRepEntry)
OBJSTRUCT_NSON_IMPORT_FUNC(TFModelRepEntry)


OBJSTRUCT_CONSTRUCT(TFModelRep)
OBJSTRUCT_DUMP_FUNC(TFModelRep)
OBJSTRUCT_UNSERIALIZE_FUNC(TFModelRep)
OBJSTRUCT_SERIALIZE_FUNC(TFModelRep)
OBJSTRUCT_NSON_EXPORT_FUNC(TFModelRep)
OBJSTRUCT_NSON_IMPORT_FUNC(TFModelRep)

OBJSTRUCT_READER(TFModelRep)
OBJSTRUCT_WRITER(TFModelRep)

TFHeader* TFHeader_VARImport(TFHeader* obj, VAR* variable) {
	if (obj == NULL) {
		obj = TFHeader_new();
		memcpy(obj->signature, "tf**", 4);
	}
//	STRUCT_IMP_NONSON(TFHeader)
	return obj;
}





void tfsavegame_read(FILE *fd, char* filename, char *outputdir) {
	TFHeader* tf_header = NULL;
	tf_header = TFHeader_read(fd);
	if (!tf_header) {
		print_err(0, "Can't read header\n");
		fclose(fd);
		exit(-1);
	}
	
	TFMods* tf_mods = TFMods_read(fd);
	TFSettingsConfig* tf_sconfig = TFSettingsConfig_read(fd);
	TFAfterSettings* tf_aftersettings = TFAfterSettings_read(fd);
	TFModelRep* tf_modelrep = TFModelRep_read(fd);
	
	
	FILEPATH *ff = filepath_new();
	filepath_relpath(ff, outputdir);
	filepath_filename(ff, "header.json");
	VAR* var_header = TFHeader_noson_export(tf_header);
	var_export_file(var_header, ff->filepath);
	
	filepath_filename(ff, "mods.json");
	VAR* var_tf_mods = TFMods_noson_export(tf_mods);
	var_export_file(var_tf_mods, ff->filepath);
	
	filepath_filename(ff, "settings.json");
	VAR* var_tf_sconfig = TFSettingsConfig_noson_export(tf_sconfig);
	var_export_file(var_tf_sconfig, ff->filepath);
	
	
	filepath_filename(ff, "aftersettings.json");
	VAR* var_tf_aftersettings = TFAfterSettings_noson_export(tf_aftersettings);
	var_export_file(var_tf_aftersettings, ff->filepath);
	
	filepath_filename(ff, "modelrep.json");
	VAR* var_tf_modelrep = TFModelRep_noson_export(tf_modelrep);
	var_export_file(var_tf_modelrep, ff->filepath);
	
	
	
	filepath_filename(ff, "remaining.data");
	FILE* fdremaining = file_open_write(ff->filepath);
	
	size_t currentPos = ftell(fd);
	fseek(fd, 0, SEEK_END);
	size_t remaininglen = ftell(fd) - currentPos;
	fseek(fd, currentPos, SEEK_SET);
	
	file_copy_bytes(fd, fdremaining, remaininglen);
	fclose(fdremaining);
	
	filepath_free(ff);
}





#define tfsavegame_write_type(type, ff, filename)			\
	filepath_filename(ff, filename);				\
	VAR *var_##type = var_import_file(ff->filepath);		\
	type* tf_##type = type##_noson_import(NULL, var_##type);	\
	type##_write(fd, tf_##type);				 


void tfsavegame_write(FILE *fd, char *sourcedir) {
	FILEPATH *ff = filepath_new();
	filepath_relpath(ff, sourcedir);
	
	
	tfsavegame_write_type(TFHeader, ff, "header.json")
	tfsavegame_write_type(TFMods, ff, "mods.json")
	tfsavegame_write_type(TFSettingsConfig, ff, "settings.json")
	tfsavegame_write_type(TFAfterSettings, ff, "aftersettings.json")
	tfsavegame_write_type(TFModelRep, ff, "modelrep.json")
	
		
	filepath_filename(ff, "remaining.data");
	FILE* fdremaining = file_open_read(ff->filepath);
	size_t remaininglen = file_size(fdremaining);
	file_copy_bytes(fdremaining, fd, remaininglen);
	filepath_free(ff);
}