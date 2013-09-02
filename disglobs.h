/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * disglobs.h - All the global variables used in the OSK disassembler  $
 *      All globals are defined in this file, then one code file will  $
 *      define _MAIN_, causing them to be defined, then all other      $
 *      files will simply have them defined "extern"                   $
 *                                                                     $
 * $Id::                                                               $
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "proto.h"

#ifdef _MAIN_
#   define xt
#else
#   define xt extern
#endif

xt int Pass;    /* The disassembler is a two-pass assembler */
xt char *ModFile;   /* The module file to read */
