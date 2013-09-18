/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * dismain.c - The main disassembler routine.  This handles a single   $
 *      pass for the disassembler routine. For each command, it passes $
 *      off the job of disassembling that command.                     $
 *                                                                     $
 * $Id::                                                               $
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "disglobs.h"
#include "textdef.h"
#include "modtypes.h"
#include <string.h>
#include "proto.h"

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
                printf("%08x %04.4x %s %s\n", CmdEnt, Instruction.cmd_wrd, Instruction.mnem, Instruction.opcode);
                if (Instruction.wcount)
                {
                    int count;
                    printf("%9s", "");

                    for (count = 0; count < Instruction.wcount; count++)
                    {
                       printf("%04x ", Instruction.code[count]);
                    }

                    printf("\n");
                }
            }
        }
        else
        {
            if (Pass == 2)
            {
                printf ("%08x %04.4x %s %08x\n", CmdEnt, (short)Instruction.cmd_wrd&0xffff, "ds.w", Instruction.cmd_wrd);
                /*PCPos += 2;
                //CmdEnt = PCPos;*/
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


int notimplemented(CMD_ITMS *ci, int tblno)
{
    return 0;
}

static int
get_asmcmd()
{
    extern OPSTRUCTURE syntax1[];
    extern COPROCSTRUCTURE syntax2[];
    extern error;

    int opword;
    int h,
      j;
    int size;


    size = 0;
    Instruction.wcount = 0;
    opword = getnext_w (&Instruction);
    /* Make adjustments for this being the command word */
    Instruction.cmd_wrd = Instruction.code[0];
    Instruction.wcount = 0; /* Set it back again */

    for (j = 1; j <= MAXINST; j++)
    {
        register OPSTRUCTURE *curop;
        error = FALSE;
        curop = tablematch (opword, j);

        if (!error)
        {
            if (curop->opfunc(&Instruction, j, curop))
            {
                return 1;
            }
        }
    }

#if(DEVICE==68040 || COPROCESSOR==TRUE)
    for (h = 3; h <= MAXCOPROCINST; h++)
    {
        error = FALSE;
        j = fpmatch (start);
        if (!error)
        {
            size = disassembleattempt (start, j);
            if (size != 0)
                break;
        }
    }
#endif
/*    if (size == 0)
    {
        printf ("\n%c%8x", HEXDEL, start);
        printf (" %4X\t\t  DC.W", get16 (start));
        printf (" \t\t?  ");
        return (2);
    }
    else
        return (size);*/

    return 0;
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
fread_b(fp)
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

/* **************************************************************************** *
 * getnext_w() - Fetches the next word (an Extended Word) from the module        *
 * Passed: The cmditems pointer                                                 *
 * Returns: the word retrieved                                                  *
 *                                                                              *
 * The PCPos is updated, the count of words in the instruction is updated and   *
 *    the word is stored in the proper Info->code position                      *
 * **************************************************************************** */

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
    ci->code[ci->wcount] = w;
    ci->wcount += 1;
    return w;
}

/* *************************************************************************** *
 * ungetnext_w() - ungets (undoes) a previous word-get.
 * Passed: Pointer to the cmditems struct
 * *************************************************************************** */

void
#ifdef __STDC__
    ungetnext_w(CMD_ITMS *ci)
#else
    ungetnext_w(ci)
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
    
    if (fread(&l, 4, 1, fp) < 1)
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

