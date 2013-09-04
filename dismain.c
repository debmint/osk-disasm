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
 *Get the module header.  We will do this only in Pass 1
 * ****************************************************** */

static int
get_modhead()
{
    int *stdmods[] = {
        &M_ID, &M_SysRev, &M_Size,  &M_Owner,  &M_SysRev,  &M_Size,
        &M_Name, &M_Accs, &M_Type, &M_Lang,  &M_Attr,  &M_Revs,
        &M_Edit,  &M_Usage, &M_Symbol, 0
    },
        **modpt;
    int *mlist1[] = {&M_Exec,&M_Except,0};
    int *mlist2[] = {&M_Stack, &M_IData, &M_IRefs, 0};
    int *mlist3[] =  {&M_Init, &M_Term,0};

    /* Get standard (common to all modules) header fields */
    modpt = stdmods;

    while (*modpt)
    {
        if (fread_b(*modpt,MFile) < 1)
        {
            filereadexit();
            ++modpt;

        }
    }

    /* We have 14 bytes that are not used */
    if (fseek(MFile, 14, SEEK_CUR) != 0)
    {
        errexit("Cannot Seek to file location");
    }

    if (fread_w(&M_Parity, MFile) < 1)
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
        modpt = mlist1;

        while ((*modpt)++) {
            if (fread_l(*modpt, MFile) < 1)
            {
                filereadexit();
            }
        }

        if ((M_Type != MT_SYSTEM) && (M_Type != MT_FILEMAN)) {
            modpt = mlist2;

            while(*(modpt)++)
            {
                if (fread_l(*modpt, MFile) < 1)
                {
                    filereadexit();
                }
            }

            if (M_Type == MT_TRAPLIB)
            {
                modpt = mlist3;

                while (*(modpt)++)
                {
                    if (fread_l(*modpt, MFile) < 1)
                    {
                        filereadexit();
                    }
                }
            }
        }

    }
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
    printf("Have entered dopass()\n");

    if (!(MFile = fopen(ModFile, "r")))
    {
        errexit("Cannot open Module file for read");
    }

    get_modhead();
}

int
#ifdef __STDC__
fread_b(int *dst, FILE *fp)
#else
fread_b(dst, fp)
    int *dst;
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
