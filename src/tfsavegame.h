/*
 * This file is part of tfsavcodec.
 * 
 * Copyright (c) 2016, Oskar Eisemuth
 * 
 * For the full copyright and license information,
 * please view the LICENSE file that was distributed with this source code.
 * 
 */

#ifndef TFSAVEGAME_H
#define TFSAVEGAME_H

#ifdef __cplusplus
extern "C" {
#endif

void tfsavegame_read(FILE *fd, char* filename, char *outputdir);
void tfsavegame_write(FILE *fd, char *sourcedir);



#ifdef __cplusplus
}
#endif

#endif /* TFSAVEGAME_H */

