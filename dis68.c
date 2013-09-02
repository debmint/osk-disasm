/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * dis68.c - The Text Mode Front-End for the OSK disassembler          $
 *                                                                     $
 * $Id::                                                               $
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define _MAIN_  /* We will define all variables in one header file, then
                   define them extern from all other files */

#include "disglobs.h"

#ifdef __STDC__
int dopass(int, char **, int);
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
    /* Process command-line options first */
    
    while ((ret = getopt(argc, argv, "abc:d")) != -1)
    {
    }

    ModFile = argv[1];
    Pass = 1;
    dopass(argc,argv,1);
    Pass = 2;
    return 0;
}

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
