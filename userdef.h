/* ******************************************************************** *
 *                                                                      *
 * This file came from Motorola's FBug program and is in the public     *
 * domain.                                                              *
 *                                                                      *
 * ******************************************************************** */

#ifndef USERDEF_H
#define USERDEF_H
#include "targetsys.h"
#include "structs.h"

#if (COPROCESSOR==TRUE || DEVICE==68040)

typedef struct
{
	char *reg;
	int field;
}COPROCCONTROLREG;

typedef struct
{
	char *condition;
}COPROCCONDITIONALS;

#endif /* COPROCESSOR==TRUE */

typedef struct
{
	char *instruction;
	int extension;
}MONADICDYADIC;

typedef struct
{
	char *reg;
	int field;
}CONTROLREG;

typedef struct
{
	char *size;
}SIZETYPES;

typedef struct
{
	char *type;
}TYPES;

typedef struct
{
	char *condition;
}CONDITIONALS;

typedef struct
{
	int allowableEA;
}EAALLOWED_TYPE;

typedef struct
{
	short option;
}ACTUALFIELD_T_F;

typedef struct
{
	char *errorentry;
}ERRORTABLE;


typedef struct
{
	char EAentry[7];
}EA_TABLE;

typedef struct opst
{
	char *name;
	short sizestr;	/* sizefield[size]   */
	short source;	/* EAtype[source]    */
	short dest;	/* EAtype[source]    */
	char *opwordstr;
	short sizestartbit;
	short sizeendbit;
    int cpulvl;
    int id;
    int (*opfunc)(CMD_ITMS *, int, struct opst *);    
} OPSTRUCTURE;

typedef struct
{
	char *name;
	short sizestr;		/* sizefield[size]   */
	short source;		/* EAtype[source]    */
	short dest;		/* EAtype[source]    */
    int id;
}COPROCSTRUCTURE;

typedef struct
{
	char field[10];
}EAFIELD;

typedef struct 
{
	char *string;
}TESTSTRING;

typedef struct
{
	int dis16;

	int immediatedata;

	int mode;
	int Aregnum;	/* single EA instruction format	*/

	int indexregnum;
	int indexregtype;
	int indexsize;
	int scale;
	int displacement8;	/* MC68020 brief format extension word	*/

	int BS;
	int IS;
	int BDsize;
	int index_indirect_selection;
	int BDdisplacement;	
	int ODdisplacement;	/* MC68020 full format extension word(s)*/
#if (DEVICE==68040 || COPROCESSOR==TRUE)
	short  	s_fp;
	short  s1_fp;/*  8 bits */
	int  	e_fp;/* 32 bits */
	int    f1_fp;/* 32 bits */
	int    f2_fp;/* 32 bits */

	short  	p_fp;/* extra digit on packed string */
#endif
}EASPEC;

/* ********************ADDITIONS****************************** */
/*
structures used throughout the monitor
*/

struct table_element	/* command names, their pointers and syntax */
{
	char *nptr;
	int (*fptr)();
	char *syntax;
};

struct syntax_element  /* command syntax elements */
{
	char *desc;
};

struct port_element	/* ports known for pf command */
{
	char *name;
	int addr;
	int size;
};

struct symbelem		/* symbols for symbol table */
{
	char name[MAXSYMBLEN];
	int value;
};

struct regelem		/* registers for mpu, fpu, cmmu etc */
{
	char *name;
	int value;
};

struct instelem		/* instructions for assembler/disassembler */
{
	char *name;
	int op;
	short addr;
	short alt;
};

struct breakelem	/* break table */
{
	int count;
	int tempcount;
	long address;
	long inst1,inst2;
};

/* ***************************************************************** */

/*
external declerations
*/

extern char PROMPT[];
extern char MPUREGMSG[];
extern char WHICHREGMSG[];
extern char BRKMSG[];
extern char SYMBMSG[];
extern char BREAKMSG[];
extern char TRACEMSG[];
extern char VINTMSG[];
extern char BERRMSG[];
extern char UNKNOWNMSG[];
extern char HITKEYMSG[];
extern char MMASHELPMSG[];

/* ***************************************************************** */
#endif

