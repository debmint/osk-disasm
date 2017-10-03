/* ******************************************************************** *
 * structs.h - Define structures that are used with oskdis              *
 *                                                                      *
 * ******************************************************************** *
 *                                                                      *
 * Copyright (c) 2017 David Breeding                                    *
 *                                                                      *
 * This file is part of osk-disasm.                                     *
 *                                                                      *
 * osk-disasm is free software: you can redistribute it and/or modify   *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 * osk-disasm is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * (see the file "COPYING") along with osk-disasm.  If not,             *
 * see <http://www.gnu.org/licenses/>.                                  *
 *                                                                      *
 * ******************************************************************** */

#ifndef HAVE_STRUCTS
#   define HAVE_STRUCTS

typedef struct cmditems {
    int cmd_wrd;        /* The single effective address word (the command) */
    char *lblname;
    char mnem[50];
    short code[10];
    int wcount;         /* The count of words in the instrct/.(except sea) */
    char opcode[200];   /* Possibly ovesized, but just to be safe */
    char *comment;     /* Inline comment - NULL if none */
    /*struct extWbrief extend;*/   /* The extended command (if present) */
} CMD_ITMS;


/*struct extended020 {
};*/

#define LBLLEN 40

/* Defines a Label */

typedef struct symbldef {
    char sname[LBLLEN+1];         /* Symbol name */
    long myaddr;                  /* Address of symbol */
    int stdname;                  /* Flag that it's a std named label */
    int global;                   /* For ROF use... flags that it's global */
    struct symbldef *Next;           /* Next */
/*    struct symbldef *More;*/
    struct symbldef *Prev;           /* Previous entry */
} LBLDEF;

typedef struct {
    char lclass;
    LBLDEF *cEnt;
} LBLCLAS;

/* Offset 9 (-L00xx) - type stuff ] */

struct ofsetree {
    char oclas_maj;        /* Class to use in offset addressing          */
    int  of_maj;           /* The main offset value                      */
    char incl_pc;          /* Flag to include PC offset mode             */
    int add_to;            /* Flag: if set, add to offset, else subtract */
    /*int of_sec;          Secondary offset (0 if none) */
    /*char oclas_sec;      Class of secondary offset */
};

/* Data areas/Label Addressing Modes tree structure */

struct databndaries {
    int  b_lo,         /* Lower (beginning) boundary address       */
         b_hi,         /* Upper (ending) boundary address          */
         b_siz;        /* Size of one unit in the set              */
    char b_typ;        /* Boundary type for DA's and Lbl Class for AModes */
    char b_class;      /* Class for class 'L' and 'S' boundaries          */
    struct ofsetree *dofst;
    struct databndaries *DLess,
                        *DMore,
                        *DPrev;
};


#endif             /*    #define HAVE_STRUCTS */

/* ******************************************** *
 * xtndcmnt structures                          *
 * These are comments that are appended to the  *
 * end of the assembly line                     *
 * ******************************************** */

struct apndcmnt {
    int adrs;
    struct apndcmnt *apLeft;
    struct apndcmnt *apRight;
    char *CmPtr;
};

/* ************************************ *
 * Stand-alone Comment structures       *
 * ************************************ */

/* Single line of a comment */
struct cmntline {
    struct cmntline *nextline;
    char *ctxt;
};

/* Main tree */

struct commenttree {
    int adrs;
    struct commenttree *cmtUp;
    struct commenttree *cmtLeft;
    struct commenttree *cmtRight;
    struct cmntline *commts;
};

/* ***************************************** *
 * Structures for ROF files                  *
 * ***************************************** */

/* ROF header structure */

struct rof_hdr {
            int   sync;
            short ty_lan,        /* Type/Language */
                  att_rev,       /* Attribute/Revision word */
                  valid,         /* Nonzero if valid */
                  series;        /* Assembler version used to compile */
            char  rdate[6];
            short edition;
            int   statstorage,   /* Size of static variable storage */
                  idatsz,        /* Size of initialized data */
                  codsz,         /* Size of the object code  */
                  stksz,         /* Size of stack required   */
                  code_begin,    /* Offset to entry point of object code   */
                  utrap,         /* Offset to unitialized trap entry point */
                  remotestatsiz, /* Size of remote static storage   */
                  remoteidatsiz, /* Size of remote initialized data */
                  debugsiz;      /* Size of the debug   */
            char *rname;         /* Ptr to module name  */
};

/* Global definitions */

struct rof_glbl {
            char  name[100];
            char  Type;
            int   Ofst;
};

/* ************************* *
 *  External references      *
 *  -------------------      *
 *  We will attempt to place *
 *  all in one set           *
 * ************************* */

struct rof_extrn {
    union {
        char *nam;
        LBLDEF *lbl;
    } EName;
/*            void *EName;*/                /* External name                    */
            char  dstClass;             /* Class for referenced item NUll if extern */
            int   Type;                 /* Type Flag                        */
            int   Ofst;                 /* Offset into code                 */
            int   Extrn;                /* Flag that it's an external ref   */
            struct rof_extrn *EUp,      /* Previous Ref for entire list     */
                             *ENext,    /* Next Reference for All externs   */
                             *MyNext;   /* Next ref for this name.  If NULL, we can free EName */
};


