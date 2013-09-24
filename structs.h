/* **************************************************************** *
 * structs.h - Define structures that are used with oskdis          $
 *                                                                  $
 * $Id::                                                            $
 * **************************************************************** */

#ifndef HAVE_STRUCTS
#   define HAVE_STRUCTS

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

typedef struct symbldef {
    char sname[LBLLEN+1];         /* Symbol name */
    long myaddr;                  /* Address of symbol */
    int stdname;                  /* Flag that it's a std named label */
    int global;                   /* For ROF use... flags that it's global */
    struct symbldef *Next;           /* Next */
//    struct symbldef *More;
    struct symbldef *Prev;           /* Previous entry */
} LBLDEF;

typedef struct {
    char lclass;
    LBLDEF *cEnt;
} LBLCLAS;

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
    int  b_lo,         /* Lower (beginning) boundary address       */
         b_hi;         /* Upper (ending) boundary address          */
    char b_typ;        /* Boundary type for DA's and Lbl Class for AModes */
    struct ofsetree *dofst;
    struct databndaries *BNext,
                        *BPrev;
};


#endif             /*    #define HAVE_STRUCTS */

