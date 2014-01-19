/* ************************************************************************ *
 *                                                                          $
 * os9disasm - a project to disassemble Os9-coco modules into source code   $
 *             following the example of Dynamite+                           $
 *                                                                          $
 * ************************************************************************ $
 *                                                                          $
 *  Edition History:                                                        $
 *  #  Date       Comments                                              by  $
 *  -- ---------- -------------------------------------------------     --- $
 *  01 2003/01/31 First began project                                   dlb $
 * ************************************************************************ $
 * File:  cmdfile.c                                                         $
 * Purpose: process command file                                            $
 *                                                                          *
 *  $Id::                                                                   $
 * ************************************************************************ */

#define _GNU_SOURCE     /* Needed to get isblank() defined */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "userdef.h"
#include "disglobs.h"
#include "proto.h"
/*#include "amodes.h"*/

char *CmdFileName;
FILE *CmdFP;
int DoingCmds;          /* Flag - Nonzero if we're doing the command file (not commandline) */

static int NxtBnd;
static int NoEnd;      /* Flag that no end on last bound                 */
static int GettingAmode;    /* Flag 1=getting Addressing mode 0=Data Boundary */
static struct databndaries *prevbnd ;

#ifdef __STDC__
static char *cpyhexnum (char *dst, char *src);
static char *cpy_digit_str (char *dst, char *src);
static int do_mode (char *lpos);
static char *setoffset (char *p, struct ofsetree *oft);
static int optincmd (char *lpos);
static void cmdamode (char *pt);
static void boundsline (char *mypos);
static struct commenttree *newcomment (int addrs,
                            struct commenttree *parent);
#else
static char *cpyhexnum ();
static char *cpy_digit_str ();
static int do_mode ();
static char *setoffset ();
static int optincmd ();
static void cmdamode ();
static void boundsline ();
static struct commenttree *newcomment ();
#endif

/* some of the following may need to be moved tothe global file */
struct databndaries *LAdds[33];     /* Temporary */
struct databndaries *dbounds;
char BoundsNames[] = "ABCDLSW";

static void
#ifdef __STDC__
badexit (char *msg)
#else
badexit (msg)
    char *msg;
#endif
{
    if (DoingCmds)
    {
        char errtxt[100];
        sprintf (errtxt, "Line %d: %s", LinNum, msg);
        errexit (errtxt);
    }
    else
    {
        errexit(msg);
    }
}

/* ************************************************************************ *
 * AsmComment() Add comments to be placed into the assembler source/listing *
 *          on separate lines                                               *
 * Format for this entry:                                                   *
 *      " <Label class> addr <delim>Dtext<delim>                            *
 *      ---                                                                 *
 *      textD\n                                                             *
 * D represents any delimiter, following lines will be added until a line   *
 *      ended by the delimiter is encountered.  The delimiter can be        *
 *      included in the text as long as it's not the last char on the line. *
 *      The delimiter can be the last character if it's doubled             *
 * ************************************************************************ */

static int
AsmComment (char *lpos, FILE *cmdfile)
{
    int adr = 0;
    register char *txt;
    char delim;
    int lastline;
    struct commenttree *treebase;
    register struct commenttree *me;
    struct cmntline *prevline = 0;
    char lblclass;

    if ( ! (lpos = cmntsetup (lpos, &lblclass, &adr)))
    {
        return -1;
    }

    switch (*lpos)
    {
#ifndef OSK
        case '\r':
#endif
        case '\n':
            return 0;    /* Require AT LEAST the delimiter on the first line */
        default:
            delim = *lpos;
            *lpos = '\0';   /* So we can have an empty string for first line */
            ++lpos;
    }

    /* Now locate or set up an appropriate tree structure */

    treebase = Comments[strpos (lblorder, lblclass)];

    if ( ! treebase)
    {
        me = treebase = Comments[strpos (lblorder, lblclass)] =
            newcomment (adr, 0);
    }
    else
    {
        struct commenttree *oldme;

       /* int cmtfound = 0;*/
        me = treebase;

        while (1) {
            if (adr < me->adrs)
            {
                if (me->cmtLeft)
                {
                    me = me->cmtLeft;
                    continue;
                }
                else
                {
                    oldme = me;
                    me = newcomment (adr, me);
                    oldme->cmtLeft = me;

                    /*cmtfound = 1;*/
                    break;
                }
            }
            else
            {
                if (adr > me->adrs)
                {
                    if (me->cmtRight)
                    {
                        me = me->cmtRight;
                        continue;
                    }
                    else        /* The same address was previously done */
                    {
                        oldme = me;
                        me = newcomment (adr, me);
                        oldme->cmtRight = me;
                        break;
                    }
                }
                else
                {
                    /*cmtfound = 1;*/

                    /* Here, we need to find the last comment */
                    prevline = me->commts;

                    while (prevline->nextline) {
                        prevline = prevline->nextline;
                    }

                    /*cmtfound = 1;*/
                    break;
                }
            }
        }       /* while (1) ( for locating or adding comments */
    }           /* end of search in commenttree - me = appropriate tree */

/************************************************************************** */

    /* Now get the comment strings */
    while (1)
    {
        struct cmntline *cline;
        char mbuf[500];

        lastline = 0;

        if (strchr(lpos,'\n'))
        {
            *strchr(lpos,'\n') = '\0';
        }
        if (strchr(lpos,'\r'))
        {
            *strchr(lpos,'\r') = '\0';
        }

        /* If this is the last line, set flag for later */

        if (lpos[strlen(lpos) - 1] == delim)
        {
            lastline = 1;
            lpos[strlen(lpos) - 1] = '\0'; /* Get rid of the delimiter */
        }

        /* Now we can finally store the comment */
        if ( ! (txt = (char *)calloc (1, strlen(lpos)+1)))
        {
            fprintf(stderr,"Error - cannot allocate memory for comment\n");
            exit(1);
        }
        strncpy (txt,lpos,strlen(lpos));

        if ( ! (cline = (struct cmntline *)calloc (1, sizeof (struct cmntline))))
        {
            fprintf (stderr,
                    "Error - cannot allocate memory for comment line\n");
            exit(1);
        }
        if (prevline)
        {
            prevline->nextline = cline;
        }
        else
        {
            me->commts = cline;
        }

        prevline = cline;
        cline->ctxt = txt;

        if (lastline)
        {
            return 0;
        }
        else
        {
            if (!(lpos = fgets(mbuf,sizeof(mbuf),cmdfile)))
            {
                return -1; /* Try to proceed on error */
            }

            ++LinNum;
            lpos = skipblank(lpos);
        }
    }
}

void
do_cmd_file ()
{
   /* FILE *CmdFP;*/
    char miscbuf[240];

    /* We will do our check for a specification for the CmdFile here */
    if ( !CmdFP)
    {
        return;
    }

    NxtBnd = 0;                 /* init Next boundary pointer */
    DoingCmds = 1;

    /*if ( ! (CmdFP = fopen (CmdFileName, "rb")))
    {
        fprintf (stderr, "Erorr # %d: Cannot open cmd file %s for read\n",
                          errno, CmdFileName);
        exit (1);
    }*/

    while (fgets (miscbuf, sizeof (miscbuf), CmdFP))
    {
        char *th,
             *mbf;
        ++LinNum;

        mbf = skipblank (miscbuf);

        /* Convert newlines and carriage returns to null */

        if ((th = (char *) strchr (mbf, '\n')))
        {
            *th = '\0';
        }

        if ((th = strchr (mbf, '\r')))
        {
                *th = '\0';
        }

        if ( ! strlen (mbf))      /* blank line? */
        {
            continue;
        }

        switch (*mbf)
        {
        case '*':         /* Comment */
            continue;           /*yes, ignore   */
        case '+':         /* Options */
            if (optincmd (++mbf) == -1)
            {   /* an error in an option would probably be fatal, anyway */
                fprintf (stderr, "Error processing cmd line #%d\n", LinNum);
                exit (1);
            }
            continue;
        case '"':        /* Assembler file comment(s)    */
            if (AsmComment (++mbf, CmdFP) == -1)
            {
                fprintf(stderr, "Error processing cmd line #%d\n", LinNum);
                exit (1);
            }

            continue;
        case '\'':   /* Append comment to end of line */
            if (ApndCmnt ( ++mbf) == -1)
            {
                fprintf (stderr, "Error processing cmd line #%d\n",LinNum);
                exit (1);
            }

            continue;
        case '>':
            cmdamode (skipblank (++mbf));
            continue;

        /* rof ascii data definition */
        /*case '=':
            rof_ascii (skipblank (++mbf));
            continue;*/
        }

        if (strchr (BoundsNames, toupper (*mbf)) || isdigit (*mbf))
        {
            boundsline (mbf);
        }
        else   /* All possible options are exhausted */
        {
            fprintf (stderr, "Illegal cmd or switch in line %d\n", LinNum);
            exit (1);
        }
    }

    DoingCmds = 0;
}

/* ************************************************************************ *
 * ApndCmnt() - Add comments to the end of the assembler command line       *
 * Format for this entry:                                                   *
 *      ' <Label Class> <hex addr> text                                     *
 *      ---                                                                 *
 * No delimiter is used, all text to the end of the line is included        *
 * ************************************************************************ */

int
ApndCmnt (char *lpos)
{
    char lblclass;
    int myadr,
        make_cmnt = 1;
    struct apndcmnt *mycmnt,
                    **me_ptr;
    char *cline;

    if ( ! (lpos = cmntsetup (lpos, &lblclass, &myadr)))
    {
        return -1;
    }

    mycmnt = CmntApnd[strpos (lblorder, lblclass)];
    me_ptr = &(CmntApnd[strpos (lblorder, lblclass)]);

    if (mycmnt)
    {
        while (1)
        {
            if (myadr < mycmnt->adrs)
            {
                me_ptr = &(mycmnt->apLeft);

                if (mycmnt->apLeft)
                {
                    mycmnt = mycmnt->apLeft;
                }
                else
                {
                    break;
                }
            }
            else
            {
                if (myadr > mycmnt->adrs)
                {
                    me_ptr = &(mycmnt->apRight);

                    if (mycmnt->apRight)
                    {
                        mycmnt = mycmnt->apRight;
                    }
                    else
                    {
                        break;
                    }
                }
                else                /* Else we've encountered an   */
                {                   /* existing entry for this adr */
                    make_cmnt = 0;  /* Flag not to calloc()        */
                    break;
                }
            }
        }
    }

    if (make_cmnt)
    {
        mycmnt = (struct apndcmnt *)calloc (1,sizeof (struct apndcmnt));
    }

    if ( ! mycmnt)
    {
        fprintf (stderr, "Cannot allocate memory for append comment\n");
        exit (1);
    }

    mycmnt->adrs = myadr;

 /*   if (me_ptr)
    {*/
        *me_ptr = mycmnt;
/*    }
    else
    {
        CmntApnd[strpos (lblorder, lblclass)] = mycmnt;
    }*/

    /* Get rid of newlines */

    if ((cline = strchr (lpos, '\n')))
    {
        *cline = '\0';
    }

    if ((cline = strchr (lpos, '\r')))
    {
        *cline = '\0';
    }

    mycmnt->CmPtr = (char *)malloc (strlen (lpos) + 1);

    if (mycmnt->CmPtr)
    {
        strcpy (mycmnt->CmPtr, lpos);
    }
    else
    {
        fprintf (stderr,
                "Cannot allocat memory for append comment string\n");
        exit (1);
    }

    return 1;
}

/* 
 * cmntsetup() - Get Label class and address for comment *
 * Passed: (1) - the command line string
 *         (2) - ptr to Class variable
 *         (3) - ptr to Address variable                       *
 * Returns current position in command line string       *
 *         positioned on the first char of the comment   *
 *         (either the delim for self-standing comments  *
 *          or the comment string for appends)           *
 * ***************************************************** */

char *
cmntsetup (char *cpos, char *clas, int *adrs)
{
    register char *p;

    cpos = skipblank (cpos);

    *clas = *(cpos++);       /* first element is Label Class */

    if (isalpha (*clas) && islower (*clas))
    {
        *clas = toupper(*clas);
    }

    if ( ! strchr(lblorder,*clas))
    {
        fprintf (stderr, "Illegal label class for comment, Line %d\n", LinNum);
        return (char *)0;
    }

    cpos = skipblank (cpos);    /* cpos now points to address */

    if (sscanf (cpos,"%x", adrs) != 1)
    {
        fprintf (stderr,"Error in getting address of comment : Line #%d\n",
                 LinNum);
        exit (1);
    }

    /* Now move up past address */

    while ((*cpos != ' ') && (*cpos != '\t'))
    {
        ++cpos;
    }

    /* Now cpos is begin of (first) line of text */
    p = cpos;

    while ((*p != ';') && (*p != '\n') && (*p != '\r') && (*p))
    {
        ++p;
    }

    *p = '\0';
    return (cpos = skipblank (cpos));
}

/* *********************************************** *
 * newcomment() adds new commenttree address       *
 * Passed: address for comment,                    *
 *         parent or null if first in tree         *
 * *********************************************** */

static struct commenttree *
newcomment (int addrs, struct commenttree *parent)
{
    struct commenttree *newtree;

    if ( ! (newtree = (struct commenttree *)calloc(1,sizeof (struct commenttree))))
    {
        fprintf (stderr, "Cannot allocate memory for commenttree\n");
        exit (1);
    }

    newtree->adrs = addrs;
    newtree->cmtUp = parent;
    return newtree;
}

/* ****************************************** *
 * Process options found in command file line *
 * ****************************************** */

static int
#ifdef __STDC__
optincmd (char *lpos)
#else
optincmd (lpos)
    char *lpos;
#endif
{
    char st[500], *spt = st;

    while (*(lpos = skipblank (lpos)))
    {
        if (sscanf (lpos, "%500s", spt) != 1)
        {
            return (-1);
        }

        lpos += strlen (spt);

        if (*spt == '-')
        {
            ++spt;
        }

        do_opt (spt);
    }
    return (0);
}

/* ************************************************************** *
 * cmdsplit() :  split cmdline..  i.e. transfer characters up to  *
 * ';' or end of line - returns ptr to next char in cmdline buffer*
 * ************************************************************** */

char *
#ifdef __STDC__
cmdsplit (char *dest, char *src)
#else
cmdsplit (dest, src)
    char *dest, *src;
#endif
{
    char c;

    src = skipblank (src);

    if ( ! (c = *src) || (c == '\n'))
        return 0;

    if (strchr (src, ';'))
    {
        while ((c = *(src++)) != ';')
        {
            *(dest++) = c;
        }

        *dest = '\0';
    }
    else
    {
        strcpy (dest, src);
        src += strlen (src);
    }
    return src;
}


/* *************************************************** *
 * Process addressing modes found in command file line *
 * *************************************************** */

static void
#ifdef __STDC__
cmdamode (char *pt)
#else
cmdamode (pt)
    char *pt;
#endif
{
    char buf[80];

    GettingAmode = 1;

    while ((pt = cmdsplit (buf, pt)))
    {
        do_mode (buf);
    }

    GettingAmode = 0;
}

/* **************************************************************** *
 * getrange() This function interprets the range specified in the   *
 *      command line and stores the low and high values in "lo"     *
 *      and "hi" (the addresses are passed by the caller            *
 * **************************************************************** */     

void
#ifdef __STDC__
getrange (char *pt, int *lo, int *hi, int usize, int allowopen)
#else
getrange (pt, lo, hi, usize, allowopen)
    char *pt; int *lo, *hi, usize, allowopen;
#endif
{
    char tmpdat[50], *dpt, c;

    dpt = tmpdat;               /* just to be sure */

    /* see if it's just a single byte/word */

    if ( ! (isxdigit (*(pt = skipblank (pt)))))
    {
        if ((*pt == '-') || ((*pt == '/')))
        {
            if (GettingAmode)
            {
                badexit ("Open-ended ranges not permitted with Amodes");
            }

            if (NoEnd)
            {
                badexit ("No start address/no end address in prev line");
            }

            *lo = NxtBnd;
        }
        else
        {                       /* no range specified */
            if (*pt && (*pt != '\n'))   /* non-digit gargabe */
            {
                badexit ("Illegal Address specified");
            }
            else
            {
                /*sprintf (pt, "%x", NxtBnd);*/
                *lo = NxtBnd;
                NxtBnd = *lo + usize;
                *hi = NxtBnd - 1;
                return;   /* We're done with this line */
            }
        }
    }
    else  /* We have a (first) number) */
    {
        pt = cpyhexnum (tmpdat, pt);
        sscanf (tmpdat, "%x", lo);
    }

    /* Scan for second number/range/etc */

    switch (c = *(pt = skipblank (pt)))
    {
    case '/':
        if (GettingAmode == 1)
        {
            badexit ("Cannot specify \"/\" in this mode!");
        }

        pt = skipblank (++pt);

        switch (*pt)
        {
            case ';':
            case '\0':
                if (NoEnd)          /* if a NoEnd from prev cmd, */
                {                   /* second bump won't be done */
                    ++NoEnd;
                }

                ++NoEnd;
                *hi = *lo;
                break;
            default:
                pt = cpy_digit_str (tmpdat, pt);
                NxtBnd = *lo + atoi (tmpdat);
                *hi = NxtBnd - 1;
                break;
        }

        break;
    case '-':
        switch (*(pt = skipblank (++pt)))
        {
        case ';':
        case '\0':
            if (NoEnd)
            {  /* if a NoEnd from prev cmd, second bump won't be done */
                ++NoEnd;
            }

            ++NoEnd;          /* Flag need end address */
            *hi = *lo;          /* tmp value */
            break;
        default:
            if (isxdigit (*pt))
            {
                tellme (pt);
                pt = cpyhexnum (tmpdat, pt);
                sscanf (tmpdat, "%x", hi);
                NxtBnd = *hi + 1;
            }
        }
        break;
    default:
        if ( ! (*pt))
        {
            NxtBnd = *lo + usize;
            *hi = NxtBnd - 1;
        }
    }

    pt = skipblank(pt);

    if (*pt)
    {
        badexit ("Extra data...");
    }
}

/*
 * do_mode() - Process a single Addressing Mode command.  called by
 *      cmdamode(), which splits the command line up into single commands.
 * Passed : lpos - Pointer to current character in command line.
 *
 */

static int
#ifdef __STDC__
do_mode (char *lpos)
#else
do_mode (lpos)
    char *lpos;
#endif
{
    struct databndaries *mptr;
    register int mclass;         /* addressing mode */
    register int notimm = 4;    /* Was 5, but we moved AM_DRCT */
    char c;
    int lo, hi;
    register struct databndaries *lp;
    struct ofsetree *otreept = 0;

    if (*(lpos = skipblank (lpos)) == '#')
    {
        notimm = 0;
        lpos = skipblank (++lpos);
    }

    switch (c = toupper (*(lpos++)))
    {
    case 'I': AMode = AM_IMM;
        break;
    case 'A':
        AMode = AM_A0;     /* Initial */
        c = *(lpos++);

         /* Must be a0, a1, ..., a7 */
        if ((c > '0') && c < ('8'))
        {
            AMode += (c - '0');
        }
        else
        {
            char ar[4];

            strncpy (ar, lpos - 1, 2);
            ar[2] = '\0';
            fprintf (stderr, "Illegal Address Register: %s\n", ar);
        }

        break;
    case 'R':
        AMode = AM_REL;
        break;
    case 'S':
        AMode = AM_SHORT;
        break;
    case 'L':
        AMode = AM_LONG;
        break;
    default:
        badexit ("Illegal addressing mode");
        exit (1);               /* not needed but just to be safe */
    }

    lpos = skipblank (lpos);
    mclass = *(lpos++);
    mclass = toupper (mclass);

    if ( ! strchr (lblorder, mclass)) /* Legal class ? */
    {
        badexit ("Illegal class definition");
    }

    /* Offset spec (if any) */

    /* check for default reset (no address) */

    if ( ! (lpos = skipblank (lpos)) || ! (*lpos) || (*lpos == ';'))
    {
        DfltLbls[AMode - 1] = mclass;
        return 1;
    }

    if (*(lpos) == '(')
    {
        otreept = (struct ofsetree *)calloc (1, sizeof (struct ofsetree));

        if ( ! otreept)
        {
            badexit ("Cannot allocate memory for offset!");
        }

        lpos = setoffset (++lpos, otreept);
    }

    lpos = skipblank (lpos);

     /*  Hopefully, passing a hard-coded 1 will work always.
     */

    getrange (lpos, &lo, &hi, 1, 0);

    /* Now insert new range into tree */

    if ( ! (mptr = (struct databndaries *)calloc (1, sizeof (struct databndaries))))
    {
        badexit ("Cannot allocate memory for data definition");
    }

    mptr->b_lo = lo;
    mptr->b_hi = hi;
    mptr->DLess = mptr->DMore = 0;
    mptr->b_typ = mclass;        /*a_mode; */
    mptr->dofst = otreept;

    if ( ! LAdds[AMode])
    {
        LAdds[AMode] = mptr;
        mptr->DPrev = 0;
    }
    else
    {
        lp = LAdds[AMode];

        while (1)
        {
            if (hi < lp->b_lo)
            {
                if (lp->DLess)
                {
                    lp = lp->DLess;
                    continue;
                }
                else
                {
                    lp->DLess = mptr;
                    mptr->DPrev = lp;
                    return 1;
                }
            }
            else
            {
                if (lo > lp->b_hi)
                {
                    if (lp->DMore)
                    {
                        lp = lp->DMore;
                        continue;
                    }
                    else
                    {
                        lp->DMore = mptr;
                        mptr->DPrev = lp;
                        return 1;
                    }
                }
                else
                {
                    badexit ("Addressing mode segments overlap");
                }
            }
        }
    }

    return 1;
}

#define ILLBDRYNAM "Illegal boundary name in line %d\n"

/* ************************************************************ *
 * boundsline () -Process boundaries found in command file line *
 *      This processes the entire line, including all multiple  *
 *      commands separated by a semicolon  on the same line     *
 *      (Called from mainline cmdfile processing routine.       *
 * ************************************************************ */     

static void
#ifdef __STDC__
boundsline (char *mypos)
#else
boundsline (mypos)
    char *mypos;
#endif
{
    char tmpbuf[80];
    register char *hold;
    register int count = 1;

    GettingAmode = 0;

    if (isdigit (*mypos))           /*   Repeat count for line   */
    {
        mypos = cpy_digit_str (tmpbuf, mypos);
        count = atoi (tmpbuf);
        mypos = skipblank (mypos);
    }

    hold = mypos;       /* Save begin of count repeated commands */

    /* Repeatedly process the repeated commands for count times */

    while (count--)
    {
        char *nextpos;
        mypos = hold;

        /* Process each command (within the repeat) individually */

        while ((nextpos = cmdsplit (tmpbuf, mypos)))
        {
            setupbounds (tmpbuf);
            mypos = nextpos;
        }
    }
}

/* ************************************************* *
 * set up offset (if there) for offset specification *
 * ************************************************* */

static char *
#ifdef __STDC__
setoffset (char *p, struct ofsetree *oft)
#else
setoffset (p, oft)
    char *p; struct ofsetree *oft;
#endif
{
    char c, bufr[80];

    /* Insure that the ofsetree struct is all NULL */

    oft->oclas_maj = oft->of_maj = oft->add_to = 0;

    p = skipblank (p);

    if ( ! strchr (p, ')'))
    {
        badexit ("\"(\" in command with no \")\"");
    }

    /* If it's "*", handle it */

    if ((c = toupper (*(p++))) == '*')
    {
        /* Hope this works - flag * addressing like this  */
        oft->incl_pc++;
        p = skipblank (p);      /* used this char, position for next */
        c = toupper (*(p++));
    }
    switch (c)
    {
        case '+':       /* Allowable for "*" ??? */
            ++oft->add_to;
            break;
        case '-':
            break;
        case ')':
            if (!oft->incl_pc)
                badexit ("Blank offset spec!!!");

            oft->oclas_maj = 'L';

            return p;
        default:
            badexit ("No '+', '-', or '*' in offset specification");
    }

    /* At this point, we have a "+" or "-" and are sitting on it */

    p = skipblank (p);
    c = toupper (*(p++));

    if ( ! strchr (lblorder, oft->oclas_maj = c) )
        badexit ("No offset specified !!");

    p = skipblank (p);

    if ( ! isxdigit (*p))
    {
        badexit ("Non-Hex number in offset value spec");
    }

    /* NOTE: need to be sure string is lowercase and following
     * value needs to go into the structure */

    p = cpyhexnum (bufr, p);
    sscanf (bufr, "%x", &(oft->of_maj));
    /*if(add_to)
       oft->of_maj = -(oft->of_maj); */

    /* Here oft->of_maj contains the offset specified in (+/-xxxx) */

    if (*(p = skipblank (p)) == ')')
    {
        addlbl(c, oft->of_maj, NULL);
        /*addlbl (oft->of_maj, c, NULL);*/
    }
    else {
        badexit ("Illegal character.. offset must end with \")\"");
    }

    return ++p;
}

static void
bndoverlap ()
{
    fprintf (stderr, "Data segment overlap in line %d\n", LinNum);
    exit (1);
}

/* ***************************************************** *
 * bdinsert() - inserts an entry into the boundary table *
 * ***************************************************** */

static void
#ifdef __STDC__
bdinsert (struct databndaries *bb)
#else
bdinsert (bb)
    struct databndaries *bb;
#endif
{
    register struct databndaries *npt;
    register int mylo = bb->b_lo, myhi = bb->b_hi;

    npt = dbounds;              /*  Start at base       */

    while (1)
    {
        if (myhi < npt->b_lo)
        {
            if (npt->DLess)
            {
                npt = npt->DLess;
                continue;
            }
            else
            {
                bb->DPrev = npt;
                npt->DLess = bb;
                return;
            }
        }
        else
        {
            if (mylo > npt->b_hi)
            {
                if (npt->DMore)
                {
                    npt = npt->DMore;
                    continue;
                }
                else
                {
                    bb->DPrev = npt;
                    npt->DMore = bb;
                    return;
                }
            }
            else
            {
                bndoverlap ();
            }
        }
    }
}

/* **************************************************************** *
 * setupbounds() - The base entry point for setting up a single     *
 *      boundary.                                                   *
 * Passed : lpos = current position in cmd line                     *
 * **************************************************************** */

void
#ifdef __STDC__
setupbounds (char *lpos)
#else
setupbounds (lpos)
    char *lpos;
#endif
{
    struct databndaries *bdry;
    register int bdtyp,
             lclass = 0;
    int rglo,
        rghi;
    char c,
         loc[20];
    struct ofsetree *otreept = 0;

    GettingAmode = 0;
    PBytSiz = 1;                /* Default to single byte */

    /* First character should be boundary type */

    switch (c = toupper (*(lpos = skipblank (lpos))))
    {
        case 'C':       /* 'C'ode cmd.  simply sets the Boundary Pointer */
                        /* for the next command */
            lpos = skipblank (++lpos);
            lpos = cpyhexnum (loc, lpos);
            sscanf (loc, "%x", &NxtBnd);
            ++NxtBnd;   /* Position to start of NEXT boundary */
            return;     /* Nothing else to do for this option */

            /* Label types */

        case 'L':
            /*PBytSiz = 2;*/
            lpos = skipblank (++lpos);
            
            switch (toupper (*(lpos++)))
            {
            case 'W':
                PBytSiz = 2;
                break;
            case 'L':
                PBytSiz = 4;
            }

            lpos = skipblank (lpos);
            lclass = toupper(*lpos);

            if ( ! strchr (lblorder, lclass))
            {
                badexit ("Illegal Label Class");
            }

            break;

        case 'D':   /* Non-label "D"igit values) */
            lclass = '$';
            lpos = skipblank(++lpos);

            switch (toupper(*lpos))
            {
            case 'B':
                PBytSiz = 1;
                break;
            case 'W':
                PBytSiz = 2;
                break;
            case 'L':
                PBytSiz = 4;
                break;
            }

            break;

        case 'A':    /* ASCII string  data */
            lclass = '^';
            break;
        case '>':
            cmdamode (skipblank (++lpos));
            break;
        default:
            fprintf(stderr, "%s\n", lpos);
            badexit ("Illegal boundary name");
    }

    bdtyp = (int) strpos (BoundsNames, c);

    /* Offset spec (if any) */

    lpos = skipblank (++lpos);

    if (*(lpos) == '(')
    {
        otreept = (struct ofsetree *)calloc (1, sizeof (struct ofsetree));
        if ( ! otreept)
        {
            badexit ("Cannot allocate memory for offset!");
        }

        lpos = setoffset (++lpos, otreept);
    }

    getrange (lpos, &rglo, &rghi, PBytSiz, 1);

    /* Now create the addition to the list */

    if ( ! (bdry = (struct databndaries *)calloc (1, sizeof (struct databndaries))))
    {
        fprintf (stderr, "Cannot allocate memory for boundary\n");
        exit (1);
    }

    bdry->b_lo = rglo;
    bdry->b_hi = rghi;
    bdry->b_siz = PBytSiz;
    bdry->b_class = lclass;
    bdry->b_typ = bdtyp;
    bdry->DLess = bdry->DMore = 0;
    bdry->dofst = otreept;

    /* We had to put it down here where bdry had already been malloc'ed
     * The way it works:  if a range is open-ended, getrange()
     * flags it 1, This flag passed through here.. We don't want
     * to substitute till the next pass, so we bump it up one
     * again here, so that, actually, "2" causes the substitution
     *
     * Note that we can have two open-ended commands in succession,
     * therefore, the actual logic is substute if NoEnd >= 2
     */

    if (NoEnd)
    {
        switch (NoEnd)
        {
        case 1:                /* NoEnd from this pass */
            ++NoEnd;            /* flag for next pass */
            break;
        default:
            /*fprintf (stderr, "NoEnd in prev cmd.. ");
            fprintf (stderr, "inserting \\x%x for prev hi\n", bdry->b_lo);
            fprintf(stderr,"...\n");*/
            prevbnd->b_hi = bdry->b_lo - 1;
            NoEnd -= 2;          /* undo one flagging */
        }
    }

    prevbnd = bdry;             /* save this for open-ended bound */

    if ( ! dbounds)
    {                           /* First entry  */
        bdry->DPrev = 0;
        dbounds = bdry;
    }
    else
    {
        bdinsert (bdry);
    }
}

void
#ifdef __STDC__
tellme (char *pt)
#else
tellme (pt)
    char *pt;
#endif
{
    return;
}

/* ******************************************************************** *
 * cpyhexnum() - Copies all valid hexadecimal string from "src" to      *
 *      "dst" until a non-hex char is encountered and NULL-terminates   *
 *      string in "dst".                                                *
 * Passed : (1) - Destination for string .  It is the responsibility of *
 *                the caller to insure that "dst" is of adequate size.  *
 *          (2) - Source from which to copy                             *
 * Returns: Pointer to first non-hexadecimal character in src string.   *
 *          This may be a space, "/", etc.                              *
 * ******************************************************************** */

static char *
#ifdef __STDC__
cpyhexnum (char *dst, char *src)
#else
cpyhexnum (dst, src)
    char *dst,
         *src;
#endif
{
    while (isxdigit (*(src)))
        *(dst++) = tolower (*(src++));
    *dst = '\0';
    return src;
}

/* ******************************************************************** *
 * cpy_digit_str() - Copies string of digits from "src" to "dst" until  *
 *      a non-digit is encountered.  See cpyhexnum() for details        *
 * ******************************************************************** */

static char *
#ifdef __STDC__
cpy_digit_str (char *dst, char *src)
#else
cpy_digit_str (dst,src)
    char *dst,
         *src;
#endif
{
    while (isdigit (*(src)))
        *(dst++) = *(src++);
    *dst = '\0';
    return src;
}

/* FIXME : This is not used, but it's wrong anyway... fix or delete */
/*int
endofcmd (char *pp)
{
    return (1 ? ((*pp == '\n') || (*pp == ';') || ( ! (*pp))) : 0);
}*/

