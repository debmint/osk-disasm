/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * dismain.c - The main disassembler routine.  This handles a single   $
 *      pass for the disassembler routine. For each command, it passes $
 *      off the job of disassembling that command.                     $
 *                                                                     $
 * $Id::                                                               $
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "disglobs.h"

static int HdrLen;

extern void errexit(
#ifdef __STDC__
char *
#endif
);

#ifdef __STDC__
int fread_l(int *, FILE *);
int fread_w(int *, FILE *);
#endif

FILE *MFile;
int dopass
#ifdef __STDC__
(int argc,char **argv,int mypass)
#else
(argc,argv,mypass)
    int argc;
    int argv;
    int mypass;
#endif
{
    int sval = 0;
    int lval = 0;
    int wval = 0;
    if (!(MFile = fopen(ModFile, "r")))
    {
        errexit("Cannot open Module file for read");
    }
    /* Now test to see if what we've got works */
    if (!fread_w(&wval,MFile))
    {
        errexit("Could not read a Word value");
    }

    if(!fread_l(&lval,MFile))
    {
        errexit("Could not read a Long value");
    }
    if (!fread_b(&sval,MFile))
    {
        errexit("Could not read a Byte value");
    }
    printf ("\nWord = %04X\nLong = %08X\nByte=%02X\n",wval,lval,sval);

}

int fread_b
#ifdef __STDC__
(int *dst, FILE *fp)
#else
(dst, fp)
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
