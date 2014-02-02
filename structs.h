/* **************************************************************** *
 * structs.h - Define structures that are used with oskdis          $
 *                                                                  $
 * $Id::                                                            $
 * **************************************************************** */

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
    /*struct extWbrief extend;*.   /* The extended command (if present) */
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
                  debugsiz;      /* Size of the debug */

};

/* Global definitions */

struct rof_glbl {
            char  name[100];
            char  Type;
            int   Ofst;
};

/* External references */

struct rof_extrn {
            char  name[100];        /* External name                  */
            char  Type;             /* Type Flag                      */
            int   Ofst;             /* Offset into code               */
            int   Extrn;            /* Flag that it's an external ref */
            struct rof_extrn *up,
                             *LNext,
                             *RNext;
};


