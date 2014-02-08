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

#define VERSION "0.1.0"

#ifdef _OSK
#   include <osk.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "structs.h"

#ifdef _MAIN_
#   define xt
#   define xtst(s) s
#else
#   define xt extern
#   define xtst
#endif

/*#ifdef _OSK
#   define strchr index
#   define strrchr rindex
#endif
*/
#ifdef _WIN32
#   define BINREAD "rb"
#   define BINWRITE "wb"
#   define PATHSEP '\\'
#else
#   define BINREAD "r"
#   define BINWRITE "w"
#   define PATHSEP '/'
#endif

#define MAX_LBFIL 32

#ifndef SEEK_SET
#   define SEEK_SET 0
#   define SEEK_CUR 1
#   define SEEK_END 2
#endif

#define MC68000 0
#define MC68008 8
#define MC68010 10
#define MC68020 20
#define MC68030 30
#define MC68040 40

enum {
    SIZ_BYTE,
    SIZ_WORD,
    SIZ_LONG
};

/* Addressing Modes */

enum {
    AM_A0 = 1,
    AM_A1,
    AM_A2,
    AM_A3,
    AM_A4,
    AM_A5,
    AM_A6,
    AM_A7,
    AM_IMM,
    AM_SHORT,   /* Absolute Short */
    AM_LONG,    /* Absolute Long */
    AM_REL,
    AM_MAXMODES   /* Count of Modes */
};

/* The following two structures define
 * the extended word
 */
struct extWbrief {
    int isFull;      /* 1 = 68020-type */
    char regNam;     /* Index Register ('D' or 'A' */
    int regno;      /* Register # */
    int isLong;      /* Index size (W/L, 0 if sign-extended word */
    int scale;      /* Scale  00 = 1, 01 = 2, 03 = 4, 11= 8 */
    int displ;      /* Displacement (lower byte) */
    int is;         /* Index Suppress */
    int bs;         /* Base Displacement Suppress */
    int bdSize;     /* BD Size */
    int iiSel;      /* Index/Indirect Selection */
};

/* The following is part of the description for the full extended word */

/* I/IS combinations
 * IS   I/IS    Operation
         * 0    000 No memory indirection
         * 0    001 Indirect preindexed with null outer displacement
         * 0    010 Indirect preindexed with word od
         * 0    011 Indirect preindexed with long od
         * 0    101 Indirect postindexed with null od
         * 0    110 Indirect postindexed with word od
         * 0    111 Indirect postindexed with long od
         * 1    000 No memory indirection
         * 1    001 Memory indirect with null od
         * 1    010 Memory indirect with word od
         * 1    011 Memory indirect with long od
         */ 

xt int cpu;
xt int PrintAllCode;
xt  CMD_ITMS Instruction;
xt int Pass;    /* The disassembler is a two-pass assembler */
xt char *ModFile;   /* The module file to read */
xt FILE *ModFP;
xt FILE *AsmPath;
xt int WrtSrc;
xt int IsROF;
xt int IsUnformatted;
xt int PCPos;
xt int CmdEnt;   /* The Entry Point for the Command */
xt int ExtBegin; /* The position of the begin of the extended list (for PC-Relative addressing) */
/* EaString is storage for the primary Effective Address of a command.  Storage for any further
 * strings need to be provided by the function */
xt char EaString[100];

/* Module header variables */
xt int M_ID, M_SysRev;
xt long M_Size, M_Owner, M_Name;
xt int M_Accs;
xt char M_Type, M_Lang, M_Attr, M_Revs;
xt int M_Edit, M_Usage, M_Symbol, M_Parity,
    M_Exec, M_Except, M_Mem, M_Stack, M_IData,
    M_IRefs, M_Init, M_Term;
/* The following are for the Device Descriptor.. (Do we need these?) */
/*xt int M_Port, M_Vector, M_IRQLvl,
    M_Prior, M_Mode, M_FMgr, M_PDev, M_DevCon, M_Opt,
    M_DType, M_PollSize, M_DevCnt, M_Procs, M_Paths, M_Params;
xt int M_Sysgo, M_SysDev, M_Consol, M_Extens, M_Clock, M_Slice,
    M_IPID, M_Site, M_Instal, M_CPUTyp, M_OS9Lvl, M_OS9Rev,
    M_SysPri, M_MinPri, M_MaxAge, M_MDirSiz, M_Events, M_Compat,
    M_Compat2, M_MemLst, M_IRQStk, M_ColdTrys, M_Id12; */

xt int IDataBegin;
xt int IDataCount;
xt int HdrEnd;   /* The first byte past end of header, usefule for begin of Pass 2 */
xt int LinNum;
xt int PgWidth
#ifdef _MAIN_
    = 80
#endif
;
xt int PgDepth
#ifdef _MAIN_
    = 66
#endif
;
xt char *DefDir;
xt int AMode;
xt int NowClass;
xt int PBytSiz;

/* Comments tree */

xt struct commenttree *Comments[33];
xt struct apndcmnt *CmntApnd[33];

xt char *lblorder
#ifdef _MAIN_
= "!^$&@%ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#endif
;

xt char DfltLbls[16]
#ifdef _MAIN_
="&&&&&&DLLLLLLLL"
#endif
;

xt char DorA[]
#ifdef _MAIN_
= {'d','a'}
#endif
;

xt char dispRegNam[2]
#ifdef _MAIN_
= {'d','a'}
#endif
;
xt char *stdSiz[]
#ifdef _MAIN_
= {"b", "w", "l"}
#endif
;

xt struct rof_hdr ROFHD;

#endif                     /* #ifdef HAVE_GLOBALS */
