/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * disglobs.h - All the global variables used in the OSK disassembler  $
 *      All globals are defined in this file, then one code file will  $
 *      define _MAIN_, causing them to be defined, then all other      $
 *      files will simply have them defined "extern"                   $
 *                                                                     $
 * $Id::                                                               $
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef HAVE_GLOBALS
#define HAVE_GLOBALS

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

typedef struct cmditems {
    char mnem[50];
    short code[10];
    int wcount;
    char opcode[200];              /* Possibly ovesized, but just to be safe */
} CMD_ITMS;

#ifdef _MAIN_
#   define xt
#else
#   define xt extern
#endif

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

xt  CMD_ITMS Instruction;
xt int Pass;    /* The disassembler is a two-pass assembler */
xt char *ModFile;   /* The module file to read */
xt FILE *ModFP;
xt int PCPos;
xt int CmdEnt;   /* The Entry Point for the Command */
xt int ExtBegin; /* The position of the begin of the extended list (for PC-Relative addressing) */

/* Module header variables */
xt int M_ID, M_SysRev, M_Size, M_Owner, M_Name, M_Accs;
xt char M_Type, M_Lang, M_Attr, M_Revs;
xt int M_Edit, M_Usage, M_Symbol, M_Parity,
    M_Exec, M_Except, M_Stack, M_IData,
    M_IRefs, M_Init, M_Term, M_Port, M_Vector, M_IRQLvl,
    M_Prior, M_Mode, M_FMgr, M_PDev, M_DevCon, M_Opt,
    M_DType, M_PollSize, M_DevCnt, M_Procs, M_Paths, M_Params;
xt int M_Sysgo, M_SysDev, M_Consol, M_Extens, M_Clock, M_Slice,
    M_IPID, M_Site, M_Instal, M_CPUTyp, M_OS9Lvl, M_OS9Rev,
    M_SysPri, M_MinPri, M_MaxAge, M_MDirSiz, M_Events, M_Compat,
    M_Compat2, M_MemLst, M_IRQStk, M_ColdTrys, M_Id12;

xt int HdrEnd;   /* The first byte past end of header, usefule for begin of Pass 2 */
xt int ModType;   /* The type of module */

#include "proto.h"              /* Do this last so that structures, etc will be defined */
#endif                     /* #ifdef HAVE_GLOBALS */
