/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  *
 * dis68.c - The Text Mode Front-End for the OSK disassembler           *
 *                                                                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  *
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
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */

#include <ctype.h>
#include <string.h>
#ifdef _WIN32
#   include <io.h>
#endif
#include "disglobs.h"
#include "userdef.h"
#include "proto.h"

#ifdef _WIN32
#   define strdup _strdup
#   define access _access
#   define R_OK 4
#endif

extern int LblFilz;              /* Count of Label files specified     */
extern char *LblFNam[]; /* Pointers to the path names for the files */
extern char *CmdFileName;        /* The path for the Command File Name */
extern FILE *CmdFP;
extern int DoingCmds;

/* **********************
 * usage() - Print Help message
 *
 * ***** */

static void
usage()
{
    fprintf (stderr, "\nOSKDis: Disassemble an OS9-68K Module\n");
    fprintf (stderr, "  Options:\n");
    fprintf (stderr, "\t-c\tSpecify the command file\n");
    fprintf (stderr, "\t-m=<x>\tSpecify CPU\n");
    fprintf (stderr, "\t\tx =\t 0  - 68000 (default)\n");
    fprintf (stderr, "\t\t\t 8  - 68008\n");
    fprintf (stderr, "\t\t\t20 - 68020\n");
    fprintf (stderr, "\t\t-s      Specify a label file (%d allowed)\n", MAX_LBFIL);
    fprintf (stderr, "\t\t-u      print unformatted listing (without line #, headers or blank lines\n");
    fprintf (stderr, "\t\t-?, -h  Show this help\n");
}

/* **************************************************************** *
 * getoptions() - Parse the arguments list and process them         *
 *        We could have used getopt() in OSK or linux, but it       *
 *        doesn't seem to be available in Windows, so we'll do it   *
 *        the old-fashioned way.                                    *
 * **************************************************************** *
 */
static void
#ifdef __STDC__
getoptions(int argc, char **argv)
#else
getoptions (argc,argv)
int argc;
char **argv;
#endif
{
    register int count;

    for (count = 1; count < argc; count++)
    {
        if (argv[count][0] == '-')
        {    /* It's an option */
            do_opt (&argv[count][1]);
        }
        else
        {
            if (ModFile != NULL)
            {
                /* I think we did handle multiple files in the 6809 version
                   If so, we'll fix that later*/
                errexit("Only One File can be processed");
            }

            ModFile = argv[count];
        }
    }
}

int
#ifdef __STDC__
main(int argc,char **argv)
#else
main(argc,argv)
    int argc;
    char **argv;
#endif
{
    int ret;
    char buf[100];

    /* Process command-line options first */

    /*while ((ret = getopt(argc, argv, "abc:d")) != -1)
    {
    }*/
    getoptions(argc, argv);

    /* We must have a file to disassemble */
    if (ModFile == NULL)
        errexit("You must specify a file to disassemble");

    /*ModFile = argv[1];*/
    Pass = 1;
    dopass(argc,argv,1);
    Pass = 2;
    dopass(argc, argv, Pass);

    return 0;
}

/* *************************
 * build_path() - Verify that the path is a valid path.
 *    If the path is not valid, try some alternatives.
 * ***** */

FILE *
#ifdef __STDC__
build_path (char *p, char *faccs)
#else
build_path (p, faccs)
    char *p;
    char *faccs;
#endif
{
    char tmpnam[100];
    char *c;
    FILE *fp;

    if (fp = fopen (p, faccs))
    {
        return fp;
    }

    if (p[0] == '~')
    {
        if (c = getenv ("HOME"))   /* Try the HOME env variable */
        {
            /* We will make some assumptions here..
             * We will assume the path is in the form "~/..."*/
            sprintf (tmpnam, "%s%s", c, &p[1]);
            
            if ((fp = fopen (tmpnam, faccs)))
            {
                return fp;
            }
            else
            {
                return NULL;
            }
        }
    }

    if (DefDir)
    {
        sprintf (tmpnam, "%s/%s", DefDir, p);

        if ((fp = fopen (tmpnam, faccs)))
        {
            return fp;
        }
    }

    if (CmdFileName && strlen(CmdFileName))
    {
        char dirpath[256];
        char fname[256];
        int relpath;
#ifdef _WIN32
        char drv[3];
        char ext[256];

        _splitpath(p, drv, dirpath, fname, ext);
        relpath = (strlen(drv) + strlen(dirpath));

        if (relpath == 0)
        {
            _splitpath(CmdFileName, drv, dirpath, fname, ext);
            _makepath(tmpnam, drv, dirpath, p, NULL);
        }
        else
        {
            return NULL;
        }
#else
#endif
        if ((fp = fopen(tmpnam, faccs)))
        {
            return fp;
        }
    }

    return NULL;         /* Everything failed */
}
 

/* **************************************************************** *
 * do_opt() - Searches for match of char pointed to by c and does   *
 *      whatever setup processing is needed.                        *
 *      This is used for both command-line opts and  opts found in  *
 *      the command file.                                           *
 * **************************************************************** */

void
#ifdef __STDC__
do_opt (char *c)
#else
do_opt (c)
    char *c;
#endif
{
    char *pt = c;
    char *AsmFile;
    int v;

    switch (tolower (*(pt++)))
    {
    case 'a':
        PrintAllCode = 1;   /* Print all code words */
        break;
    case 'c':                  /* Specify Command file */
        if (CmdFileName)
        {
            fprintf (stderr, "Command file already defined\n");
            fprintf (stderr, "Ignoring %s\n", pass_eq (pt));
        }
        else
        {
            CmdFileName = pass_eq (pt);

            if (!(CmdFP = build_path (CmdFileName, BINREAD)))
            {
                fprintf (stderr, "*** Failed to open Command file %s***\n",
                        CmdFileName);
                fprintf (stderr,
                        "Continuing without using the Command file\n");
            }
        }

        break;
    case 'm':      /* Target CPU */
        pt = pass_eq(pt);
        
        switch (v = strtol(pt, &pt, 10))
        {
        case 68000:
        case 68008:
        case 68010:
        case 68020:
            cpu = v - 68000;
            break;
        case 0:
        case 8:
        case 10:
        case 20:
            cpu = v;
            break;
        default:
            fprintf (stderr, "Error: %d is not a valid CPU... Ignoring\n", v);
            break;
        }

        break;
    case 'o':                  /* output asm src file */
        AsmFile = pass_eq(pt);

        if ( ! (AsmPath = fopen (AsmFile, BINWRITE)))
        {
            errexit ("Cannot open output file\n");
        }
        
        WrtSrc = 1;
        break;
    /*case 'x':
        pt = pass_eq(pt);

        switch (toupper(*pt))
        {
            case 'C':
                OSType = OS_Coco;
                DfltLbls = CocoDflt;
                fprintf(stderr, "You are disassembling for coco\n");
                break;
            default:
                fprintf (stderr, "Error, undefined OS type: %s\n", pt);
                exit (1);
        }

        break;*/
    case 'r':           /* File is an ROF file */
        IsROF = 1;
        break;
    case 's':                  /* Label file name       */
        if (LblFilz < MAX_LBFIL)
        {
            pt = pass_eq (pt);

            if (*pt)
            {
                /* In the OS9 version we used build_path here,
                 * but let's try waiting till we read the file
                 */
                if (DoingCmds)
                {
                    LblFNam[LblFilz++] = strdup(pt);
                }
                else
                {
                    LblFNam[LblFilz++] = pt;
                }
            }
        }
        else
        {
            fprintf (stderr, "Max label files allotted -- Ignoring \'%s\'\n",
                     pass_eq (pt));
        }
        break;
    case 'g':                 /* tabs (g-print-capable) */
        /*tabinit ();*/
        break;
    case 'p':                  /* Page width/depth */
        switch (tolower (*(pt++)))
        {
        case 'w':
            PgWidth = atoi (pass_eq (pt));
            break;
        case 'd':
            PgDepth = atoi (pass_eq (pt));
            break;
        default:
            usage ();
            exit (1);
            break;
        }

        break;
    case 'u':                  /* Translate to upper-case */
        IsUnformatted = 1;
        /*UpCase = 1;*/
        break;
    case 'd':
        if ( ! DoingCmds)
        {
            DefDir = pass_eq (pt);
        }
        else
        {
            pt = pass_eq (pt);

            if ( ! (DefDir = strdup (pt)))
            {
                fprintf (stderr, "Cannot allocate memory for Defs dirname\n");
                exit (1);
            }
        }

        break;
    /*case 'z':
        dozeros = 1;
        break;*/
    default:                   /* Unknown Option */
        usage ();
        exit (1);
    }
}

/* ***************************************************************************** *
 * errexit() - Exit when an error occurs.  Prints a prompt describing the error  *
 * Passed: A brief string describing the nature of the error.  A return is not   *
 *      required in the prompt string; the subroutine provides one.              *
 * ***************************************************************************** */

void errexit
#ifdef _OSK
(pmpt)
    char *pmpt;
#else
(char *pmpt)
#endif
{
    fprintf (stderr, "%s\n",pmpt);
    exit(errno ? errno : 1);
}

/* *******************************
 * Exit on File Read error...
 * ******************************* */

void filereadexit()
{
    if (feof(ModFP))
    {
        errexit ("End of file reached prematurely\n");
    }
    else
    {
        errexit ("Error reading file...\nAborting");
    }
}

