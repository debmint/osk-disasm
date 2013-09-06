/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * dis68.c - The Text Mode Front-End for the OSK disassembler          $
 *                                                                     $
 * $Id::                                                               $
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define _MAIN_  /* We will define all variables in one header file, then
                   define them extern from all other files */

#include "disglobs.h"

static void
#ifdef __STDC__
getoptions(int,char**);
#else
getoptions();
#endif

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
    getoptions(argc,argv);

    /* We must have a file to disassemble */
    if (ModFile == NULL)
        errexit("You must specify a file to disassemble");

    /*ModFile = argv[1];*/
    Pass = 1;
    dopass(argc,argv,1);
    Pass = 2;
    dopass(argc,argv,1);

    return 0;
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
    errexit ("Error reading file... Aborting");
}