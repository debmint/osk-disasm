/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * dismain.c - The main disassembler routine.  This handles a single   $
 *      pass for the disassembler routine. For each command, it passes $
 *      off the job of disassembling that command.                     $
 *                                                                     $
 * $Id::                                                               $
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "disglobs.h"
#include "modtypes.h"


static int HdrLen;

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

    if ((fread_w(&M_ID,ModFP) < 1) || (fread_w(&M_SysRev,ModFP) < 1) ||
        (fread_l(&M_Size, ModFP) < 1) || (fread_l(&M_Owner, ModFP) < 1) ||
        (fread_l(&M_Name, ModFP) < 1) || (fread_w(&M_Accs, ModFP) < 1) ||
        (fread_b(&M_Type, ModFP) < 1) || (fread_b(&M_Lang, ModFP) < 1) ||
        (fread_b(&M_Attr, ModFP) < 1) || (fread_b(&M_Revs, ModFP) < 1) ||
        (fread_w(&M_Edit, ModFP) < 1) || (fread_l(&M_Usage, ModFP) < 1) ||
        (fread_l(&M_Symbol, ModFP) < 1))
    {
        filereadexit();
    }

    /* We have 14 bytes that are not used */
    if (fseek(ModFP, 14, SEEK_CUR) != 0)
    {
        errexit("Cannot Seek to file location");
    }

    if (fread_w(&M_Parity, ModFP) < 1)
    {
        filereadexit();
    }

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
        if ((fread_l(&M_Exec, ModFP) < 1) ||(fread_l(&M_Except, ModFP) < 1))
        {
            filereadexit();
        }

        HdrEnd = ftell(ModFP); /* We'll keep changing it if necessary */

        if ((M_Type != MT_SYSTEM) && (M_Type != MT_FILEMAN))
        {

            if ((fread_l(&M_Stack, ModFP) < 1) ||
                (fread_l(&M_IData, ModFP) < 1) ||
                (fread_l(&M_IRefs, ModFP) < 1))
            {
                filereadexit();
            }

            HdrEnd = ftell(ModFP);  /* Update it again */

            if (M_Type == MT_TRAPLIB)
            {

                if ((fread_l(&M_Init, ModFP) < 1) ||
                        (fread_l(&M_Term, ModFP) < 1))
                {
                    filereadexit();
                }

                HdrEnd = ftell(ModFP);  /* The final change.. */
            }
        }
    }

    showresults();      /* Test results */ 
}

/* Test results */
int showresults()
{
    char buf[200], *pt=buf;
    char tmpch='a';

    fseek(ModFP,M_Name,SEEK_SET);
    while (tmpch) {
        if (fread_b(&tmpch,ModFP) < 1)
            filereadexit();
        *(pt++) = tmpch;
    }
    *pt = '\0';
    printf("Module name: %s\n", buf);
    printf("Module Size: %X %d\n", M_Size, M_Size);
    printf( "Stack Size: %X %d\n", M_Stack, M_Stack);
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
        get_modhead();
    }
    else
    {
    }
}

int
#ifdef __STDC__
fread_b(char *dst, FILE *fp)
#else
fread_b(dst, fp)
    char *dst;
    FILE *fp;
#endif
{
    return fread(dst, 1, 1, fp);
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
int fread_w(dst,fp)
    int *dst;
    FILE *fp;
{
    return fread(dst, 2, 1, fp);
}

int fread_l(dst,fp)
    int *dst;
    FILE *fp;
{
    return fread(dst, 4, 1, fp);
}
#else
int fread_w(int *dst, FILE *fp)
{
    int tmpi[2] = {0,0};
    int *tt = tmpi;
    int count;

    for (count = 0; count < 2; count++)
    {
        if (fread(tt, 1, 1, fp) < 1)
        {
            return 0;
        }

        ++tt;
    }

    *dst = tmpi[0]<<8 | (tmpi[1] & 0xff);

    return 1;
}

int fread_l(int *dst, FILE *fp)
{
    int tt;
    int tmpnum = 0;
    int count;

    for (count = 0; count < 4; count++)
    {
        if (fread(&tt, 1, 1, fp) < 1)
        {
            return 0;
        }

        tmpnum = (tmpnum<<8) | (tt & 0xff); 
    }

    *dst = tmpnum;

    return 1;
}
#endif
