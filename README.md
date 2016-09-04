tfsavcodec
================================

A program to edit/extract/compress Train Fever savegames

Notes
--------------------------------
For compiling a C99 compiler is necessary.
Includes a pached lz4 library to allow decompression of block checksums.

Known Issues / Bugs
--------------------------------
* Big savegames won't work
* Limited to version 18 savegames
* Memory allocations not always checked for failure
