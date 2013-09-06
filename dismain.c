/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * dismain.c - The main disassembler routine.  This handles a single   $
 *      pass for the disassembler routine. For each command, it passes $
 *      off the job of disassembling that command.                     $
 *                                                                     $
 * $Id::                                                               $
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "disglobs.h"
#include "modtypes.h"
#include <string.h>

static int HdrLen;

static int get_asmcmd(
#ifdef __STDC__
    void
#endif
);

extern void errexit(
#ifdef __STDC__
char *
#endif
);

typedef struct modestrs {
    const char *str;
    int CPULvl;
} MODE_STR;

MODE_STR ModeStrings[] = {
    {"D%d", 0},
    {"A%d", 0},
    {"(A%d)", 0},
    {"(A%d)+", 0},
    {"-(A%d)", 0},
    {"%s(A%d)", 0},
    {"(%s,A%d,%s%d)", 0}
};
/* Need to add for 68020-up modes.  Don't know if they can be included in these two arrays or not..*/
MODE_STR Mode07Strings[] = {
    {"(%s).w", 0},
    {"(%s).l", 0},
    {"(%s},pc)",0},
    {"(%s,pc},%s%d", 0},
    {"#%s", 0}
};

/* ****************************************************** *
 * Get the module header.  We will do this only in Pass 1 *
 * ****************************************************** */

static int
get_modhead()
{
    /* Get standard (common to all modules) header fields */

    M_ID = fread_w(ModFP);
    M_SysRev = fread_w(ModFP);
    M_Size = fread_l( ModFP);
    M_Owner = fread_l(ModFP);
    M_Name = fread_l(ModFP);
    M_Accs = fread_w(ModFP);
    M_Type = fread_b(ModFP);
    M_Lang = fread_b(ModFP);
    M_Attr = fread_b(ModFP);
    M_Revs = fread_b(ModFP);
    M_Edit = fread_w(ModFP);
    M_Usage = fread_l(ModFP);
    M_Symbol = fread_l(ModFP);

    /* We have 14 bytes that are not used */
    if (fseek(ModFP, 14, SEEK_CUR) != 0)
    {
        errexit("Cannot Seek to file location");
    }

    M_Parity = fread_w(ModFP);

    switch (M_ID)
    {
    case 0x4afc:
        ModType = MT_PROGRAM;
        break;
    case 0xdead:
        errexit("Disassembly of ROF files is not yet implemented.");
        break;
    default:
        errexit("Unknown module type");
    }

    /* Now get any further Mod-type specific headers */

    switch (M_Type)
    {
    case MT_PROGRAM:
    case MT_SYSTEM:
    case MT_TRAPLIB:
    case MT_FILEMAN:
    case MT_DEVDRVR:
        M_Exec = fread_l(ModFP);
        M_Except = fread_l(ModFP);

        HdrEnd = ftell(ModFP); /* We'll keep changing it if necessary */

        if ((M_Type != MT_SYSTEM) && (M_Type != MT_FILEMAN))
        {

            M_Stack = fread_l(ModFP);
            M_IData = fread_l(ModFP);
            M_IRefs = fread_l( ModFP);

            HdrEnd = ftell(ModFP);  /* Update it again */

            if (M_Type == MT_TRAPLIB)
            {
                M_Init = fread_l(ModFP);
                M_Term = fread_l(ModFP);
                HdrEnd = ftell(ModFP);  /* The final change.. */
            }
        }
    }

    return 0;
}

/* ******************************************************************* *
 * dopass() - Do a complete single pass through the module.            *
 *      The first pass establishes the addresses of necessary labels   *
 *      On the second pass, the desired label names have been inserted *
 *      and printing of the listing and writing of the source file is  *
 *      done, if either or both is desired.                            *
 * ******************************************************************* */

int
#ifdef __STDC__
dopass(int argc,char **argv,int mypass)
#else
dopass(argc,argv,mypass)
    int argc;
    int argv;
    int mypass;
#endif
{
    int sval = 0;
    int lval = 0;
    int wval = 0;

    if (!(ModFP = fopen(ModFile, "r")))
    {
        errexit("Cannot open Module file for read");
    }

    if (Pass == 1)
    {
        PCPos = 0;
        get_modhead();
        PCPos = ftell(ModFP);
    }

    /* NOTE: This is just a temporary kludge The begin _SHOULD_ be
             the first byte past the end of the header, but until
             we get bounds working, we'll do  this. */
    if (fseek(ModFP, M_Exec, SEEK_SET) != 0)
    {
        errexit("FIle Seek Error");
    }

    PCPos = M_Exec;


    while (PCPos < M_Size)
    {
        CmdEnt = PCPos;
        
        if (get_asmcmd())
        {
            if (Pass == 2)
            {
                printf("%08x %04.4x %s %s\n", CmdEnt, Instruction.code[0], Instruction.mnem, Instruction.opcode);
            }
        }
        else
        {
            if (Pass == 2)
            {
                printf ("%08x %04.4x %s %08x\n", CmdEnt, (short)Instruction.code[0]&0xffff, "ds.w", Instruction.code[0]);
                //PCPos += 2;
                //CmdEnt = PCPos;
            }
        }
    }

    return 0;
}

static struct cmditems *
initcmditems ()
{
    Instruction.mnem[0] = 0;
    Instruction.wcount = 0;
    return &Instruction;
}

static int
get_asmcmd()
{
    struct cmditems *CodePtr;

    Instruction.mnem[0] = 0;
    Instruction.wcount = 0;

    Instruction.code[0] = getnext_w(&Instruction);
    ExtBegin = CmdEnt+2;

    switch (Instruction.code[0] >> 12)
    {
    case 0:
        opcode0000(&Instruction);
        break;
    case 1:
    case 2:
    case 3:
        return move_instr(&Instruction) ? 1 : 0;
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
        break;
    }

    return 0;    /*  No cmd developed */
}

/* *********************************************************************************** *
 * fread_* functions - These are partly convenience functions but mostly are used to   *
 *     enable retrieving multi-byte values regardless of the "ENDIAN"ness of the CPU   *
 * If the read fails, the program is immediately exited.                               *
 * *********************************************************************************** */

char
#ifdef __STDC__
fread_b(FILE *fp)
#else
fread_b(dst, fp)
    FILE *fp;
#endif
{
    char b;

    if (fread(&b, 1, 1, fp) < 1)
    {
        filereadexit();
    }

    return b;
}

/* **********************************************************
 * getnext_w() - Fetches the next word from the module
 * Passed: The cmditems pointer
 * Returns: the word retrieved
 *
 * The PCPos is updated, and the count of words in the instruction is updated
 * ********************************************************** */

short
#ifdef __STDC__
getnext_w(CMD_ITMS *ci)
#else
getnext_w(ci)
    CMD_ITMS *ci;
#endif
{
    short w;

    w = fread_w(ModFP);
    PCPos += 2;
    ci->wcount += 1;
    return w;
}

/* *************************************************************************** *
 * unget_w() - ungets (undoes) a previous word-get.
 * Passed: Pointer to the cmditems struct
 * *************************************************************************** */

void
#ifdef __STDC__
    unget_w(CMD_ITMS *ci)
#else
    unget_w(ci)
    CMD_ITMS *ci;
#endif
{
    fseek (ModFP, -2, SEEK_CUR);
    PCPos -= 2;
    ci->wcount -= 1;
}

/* ******************************************************************** *
 * Some substitutions to allow portability. Most deal with the byte     *
 *  order of the CPU                                                    *
 * ******************************************************************** */

/* Read multi-byte numbers from the file.  
 * We are going to make a serious assumption here.  If it's _OSK then
 * assume it's LITTLE_ENDIAN, other wise it's BIG_ENDIAN.  Also we'll
 * assume that if it's _OSK, _STDC_ is #undef; otherwise it's defined
 */

#ifdef _OSK
short fread_w(fp)
    FILE *fp;
{
    short w;
    
    if (fread(&w, 2, 1, fp) < 1)
    {
        filereadexit();
    }

    return w;
}

int fread_l(fp)
    FILE *fp;
{
    int l;
    
    if (fread(dst, 4, 1, fp) < 1)
    {
        filereadexit();
    }

    return l;
}
#else
short fread_w(FILE *fp)
{
    int tmpi[2] = {0,0};
    int *tt = tmpi;
    int count;

    for (count = 0; count < 2; count++)
    {
        if (fread(tt, 1, 1, fp) < 1)
        {
            filereadexit();
        }

        ++tt;
    }

    return tmpi[0]<<8 | (tmpi[1] & 0xff);
}

int fread_l(FILE *fp)
{
    int tt;
    int tmpnum = 0;
    int count;

    for (count = 0; count < 4; count++)
    {
        if (fread(&tt, 1, 1, fp) < 1)
        {
            filereadexit();
        }

        tmpnum = (tmpnum<<8) | (tt & 0xff); 
    }

    return tmpnum;

    return 1;
}
#endif

/* ********************************************************************************** *
 * get_eff_addr() - Build the appropriate opcode string for the command and store it  *
 *     in the command structire opcode string                                         *
 * ********************************************************************************** */

/* NOTE: SHOULD THE VALID MODES BE CONSIDERED HERE? */

int
#ifdef __STDC__
get_eff_addr(CMD_ITMS *ci, char *ea, int mode, int reg)
#else
get_eff_addr(ci, reversed)
    CMD_ITMS ci;
int mode;
int reg;
#endif
{
    short ext1;
    short ext2;
    short displac_w;
    int displac_l;
    char dispstr[50];
    char dispReg[4];
    char dispRegNam[2] = {'D','A'};

    switch (mode)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        sprintf(ea, ModeStrings[mode].str,reg);
        return 1;
        break;
    case 5:
        ext1 = getnext_w(ci);
        ++(ci->wcount);
        displac_w = (ext1 & 0xffff);

        /* The system biases the data Pointer (a6) by 0x8000 bytes, so compensate */

        if (reg == 6) {
            displac_w += 0x8000;
        }

        /* NOTE:: NEED TO TAKE INTO ACCOUNT WHEN DISPLACEMENT IS A LABEL !!! */
        sprintf(dispstr, "%d", displac_w);
        sprintf (ea, ModeStrings[mode].str,dispstr,reg);
        return 1;
        break;
    case 6:
        ext1 = getnext_w(ci);
        ++(ci->wcount);
        displac_w = (ext1 & 0xffff);

        if (reg == 6) {     /* Compensate for Data Register bias */
            displac_w += 0x8000;
        }
        sprintf (dispReg, "%c%d", dispRegNam[ext1 >> 15], (ext1 >> 12) & 7);
        /* NOTE:: NEED TO TAKE INTO ACCOUNT WHEN DISPLACEMENT IS A LABEL !!! */
        sprintf (dispstr, "%d", displac_w);
        sprintf (ea, ModeStrings[mode].str, dispReg, dispstr, reg);
        return 1;
        break;

        /* We now go to mode %111, where the mode is determined by the register field */
    case 7:
        switch (reg) {
        case 0: /* (xx).W */
            ext1 = getnext_w(ci);
            ++(ci->wcount);
            displac_w = ext1 & 0xffff;
            /* NOTE:: NEED TO TAKE INTO ACCOUNT WHEN DISPLACEMENT IS A LABEL !!! */
            sprintf (dispstr, "%d", displac_w);
            sprintf (ea, ModeStrings[reg].str, dispstr);
            return 1;
        case 1:                               /* (xxx).L */
            ext1 = getnext_w(ci);
            ++(ci->wcount);
            ext2 = getnext_w(ci);
            ++(ci->wcount);
            sprintf (dispstr, "%dL", (ext1 <<16) | ext2);
            sprintf (ea, ModeStrings[reg].str, dispstr);
            return 1;
        case 4:
            ext1 = getnext_w(ci);
            ++(ci->wcount);

            switch (ci->code[0] >> 12)
            {
                char b;
            case 1:    /* byte */
                b = ext1 & 0xff;
                sprintf (dispstr, "%d", (int)b);
                break;
            case 3:    /* word */
                displac_w = ext1 & 0xffff;
                sprintf (dispstr, "%d", displac_w);
                break;
            case 2:    /* long */
                ext2 = getnext_w(ci);
                ++(ci->wcount);
                displac_l = (ext1 << 8) | ext2;
                sprintf (dispstr, "%dL", displac_l);
                break;
            }

             sprintf (ea, Mode07Strings[reg].str, dispstr);
            return 1;
        case 2:              /* (d16,PC) */
            ext1 = getnext_w(ci);
            ++(ci->wcount);
            sprintf (dispstr, "%d", ext1);
            sprintf (ea, Mode07Strings[reg].str, dispstr);
            return 1;
        case 3:
            ext1 = getnext_w(ci);
            ++(ci->wcount);
            displac_w = (ext1 & 0xffff);
            sprintf (dispReg, "%c%d", dispRegNam[ext1 >> 15], (ext1 >> 12) & 7);
            /* NOTE:: NEED TO TAKE INTO ACCOUNT WHEN DISPLACEMENT IS A LABEL !!! */
            sprintf (dispstr, "%d", displac_w);
            sprintf (ea, Mode07Strings[mode].str, dispReg, dispstr, reg);
            return 1;
        }
    }

    return 0;    /* Return 0 means no effective address was found */
}
