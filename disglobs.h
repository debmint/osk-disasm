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

#ifdef _MAIN_
#   define xt
#else
#   define xt extern
#endif

#ifndef SEEK_SET
#   define SEEK_SET 0
#   define SEEK_CUR 1
#   define SEEK_END 2
#endif

enum {
    SIZ_BYTE,
    SIZ_WORD,
    SIZ_LONG
};

/* The following two structures define
 * the extended word
 */
struct extWbrief {
    int is020;      /* 1 = 68020-type */
    char d_a;     /* Index Register ('D' or 'A' */
    int regno;      /* Register # */
    int isize;      /* Index size (W/L, 0 if sign-extended word */
    int scale;      /* Scale  00 = 1, 01 = 2, 03 = 4, 11= 8 */
    int displ;      /* Displacement (lower byte) */
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


typedef struct cmditems {
    int cmd_wrd;        /* The single effective address word (the command) */
    char mnem[50];
    short code[10];
    int wcount;         /* The count of words in the instrct/.(except sea) */
    char opcode[200];   /* Possibly ovesized, but just to be safe */
    //struct extWbrief extend;   /* The extended command (if present) */
} CMD_ITMS;


//struct extended020 {
//};

#define LBLLEN 40

/* Defines a Label */

struct symbldef {
    char sname[LBLLEN+1];         /* Symbol name */
    long myaddr;                  /* Address of symbol */
    int stdname;                  /* Flag that it's a std named label */
    int global;                   /* For ROF use... flags that it's global */
    struct nlist *Next;           /* Next */
    struct nlist *Prev;           /* Previous entry */
};

/* Offset 9 (-L00xx) - type stuff ] */

struct ofsetree {
    char oclas_maj;        /* Class to use in offset addressing          */
    char of_maj;           /* The main offset value                      */
    char incl_pc;          /* Flag to include PC offset mode             */
    int add_to;            /* Flag: if set, add to offset, else subtract */
    /*int of_sec;          Secondary offset (0 if none) */
    /*char oclas_sec;      Class of secondary offset */
};

/* Data areas/Label Addressing Modes tree structure */

struct databndaries {
    long b_lo,         /* Lower (beginning) boundary address       */
         b_hi;         /* Upper (ending) boundary address          */
    char b_typ;        /* Boundary type for DA's and Lbl Class for AModes */
    struct ofsetree *dofst;
    struct databndaries *BNext,
                        *BPrev;
};

xt int cpu;
xt  CMD_ITMS Instruction;
xt int Pass;    /* The disassembler is a two-pass assembler */
xt char *ModFile;   /* The module file to read */
xt FILE *ModFP;
xt int PCPos;
xt int CmdEnt;   /* The Entry Point for the Command */
xt int ExtBegin; /* The position of the begin of the extended list (for PC-Relative addressing) */

/* Module header variables */
xt int M_ID, M_SysRev;
xt long M_Size, M_Owner, M_Name;
xt int M_Accs;
xt char M_Type, M_Lang, M_Attr, M_Revs;
xt int M_Edit, M_Usage, M_Symbol, M_Parity,
    M_Exec, M_Except, M_Stack, M_IData,
    M_IRefs, M_Init, M_Term;
/* The following are for the Device Descriptor.. (Do we need these?) */
xt int M_Port, M_Vector, M_IRQLvl,
    M_Prior, M_Mode, M_FMgr, M_PDev, M_DevCon, M_Opt,
    M_DType, M_PollSize, M_DevCnt, M_Procs, M_Paths, M_Params;
xt int M_Sysgo, M_SysDev, M_Consol, M_Extens, M_Clock, M_Slice,
    M_IPID, M_Site, M_Instal, M_CPUTyp, M_OS9Lvl, M_OS9Rev,
    M_SysPri, M_MinPri, M_MaxAge, M_MDirSiz, M_Events, M_Compat,
    M_Compat2, M_MemLst, M_IRQStk, M_ColdTrys, M_Id12;

xt int HdrEnd;   /* The first byte past end of header, usefule for begin of Pass 2 */
xt int ModType;   /* The type of module */

xt char DorA[]
#ifdef _MAIN_
= {'d','a'}
#endif
;

#include "proto.h"              /* Do this last so that structures, etc will be defined */
#endif                     /* #ifdef HAVE_GLOBALS */
