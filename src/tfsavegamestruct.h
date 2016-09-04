/*
 * This file is part of tfsavcodec.
 * 
 * Copyright (c) 2016, Oskar Eisemuth
 * 
 * For the full copyright and license information,
 * please view the LICENSE file that was distributed with this source code.
 * 
 */

#ifndef TFSAVEGAMESTRUCT_H
#define TFSAVEGAMESTRUCT_H

#include "structmacros.h"
#include <stdint.h>

#define MAGICNUMBER_TFUNCOMPRESSED 0x2A2A6674
#define TFSAVEGAMEVERSION 0x12

#define struct_TFModDisplayString(_)				\
	_(field)(tfstring, name)				\
	_(field)(u32, severity)
STRUCT_DEFINE(TFModDisplayString, struct_TFModDisplayString)		

#define struct_TFHeader(_)					\
	_(array)(u8, signature, 4) /* 'tf**' */			\
	_(field)(s32, savegameversion)				\
	_(field)(s32, difficulty)				\
	_(field)(s32, startYear)				\
	_(field)(s32, numTiles)					\
	_(field)(u32, date)					\
	_(field)(s64, money)					\
	_(vector)(s32, num_mods, TFModDisplayString, mods)	\
	_(field)(u8, achievementsEarnable)			
STRUCT_DEFINE(TFHeader, struct_TFHeader)

#define struct_TFModEntry(_)					\
	_(field)(tfstring, name)				\
	_(field)(u32, unknown)
STRUCT_DEFINE(TFModEntry, struct_TFModEntry)			

#define struct_TFMods(_)					\
	_(vector)(s32, num_mods, TFModEntry, mods)		
STRUCT_DEFINE(TFMods, struct_TFMods)	

#define struct_TFKeyValueString(_)				\
	_(field)(tfstring, key)					\
	_(field)(tfstring, value)				
STRUCT_DEFINE(TFKeyValueString, struct_TFKeyValueString)

#define struct_TFSettingsConfig(_)				\
	_(filepos)(filepos, _filepos)				\
	_(vector)(u32, numentries, TFKeyValueString, entries)			
STRUCT_DEFINE(TFSettingsConfig, struct_TFSettingsConfig)	



#define struct_TFAfterSettings(_)				\
	_(filepos)(filepos, _filepos)				\
	_(field)(u8, unknownA1)					\
	_(field)(u16, unknownA2)				\
	_(field)(u32, unknownA3)				\
	_(field)(u32, unknownA4)				\
	_(field)(u32, unknownA5)				\
	_(field)(u32, unknownA6)				\
	_(field)(s32, startYear)				\
	_(field)(u32, unknownY1)				\
	_(field)(u32, unknownY2)				\
	_(field)(u32, unknownY3)				\
	_(field)(u32, unknownY4)				\
	_(field)(u32, unknownY5)				\
	_(field)(u32, unknownY6)				\
	_(field)(u32, date)					
STRUCT_DEFINE(TFAfterSettings, struct_TFAfterSettings)	



#define struct_TFModelRepEntry(_)				\
	_(field)(tfstring, key)					\
	_(field)(u32, unknown)				
STRUCT_DEFINE(TFModelRepEntry, struct_TFModelRepEntry)

#define struct_TFModelRep(_)					\
	_(filepos)(filepos, _filepos)				\
	_(vector)(u32, numentries, TFModelRepEntry, entries)	\
	_(filepos)(filepos, _fileposEnd)					
STRUCT_DEFINE(TFModelRep, struct_TFModelRep)	


#define struct_TFTerrain(_)					\
	_(filepos)(filepos, _filepos)				\
	_(field)(u16, width)					\
	_(field)(u32, height)					\
	_(field)(u32, baseLevels)				\
	_(field)(u32, vec1)					\
	_(field)(u32, vec2)					\
	_(field)(u32, vec3)					\
	_(field)(u32, highLevels)				\
	_(field)(u32, unk)				
STRUCT_DEFINE(TFTerrain, struct_TFTerrain)	


#if 0
Savegame Aufbau:
	
TFHeader
TFMods
TFSettingsConfig
TFAfterSettings  => vielleicht CGameTime?
TFModelRep

#endif

#endif
