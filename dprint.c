/* ************************************************************************ *
 *                                                                          *
 *  oskdisasm - OS9-68K DISASSEMBLER                                        $
 *             following the example of Dynamite+                           *
 *                                                                          *
 *  $Id::                                                                   $
 *                                                                          *
 *  Edition History:                                                        *
 *  *  Date       Comments                                              by  *
 *  -- ---------- -------------------------------------------------     --- *
 *  01 2003/01/31 First began project                                   dlb *
 * ************************************************************************ *
 * File:  dprint.c                                                          *
 * Purpose: handle printing and output function                             *
 * ************************************************************************ */

#include "disglobs.h"
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "modtypes.h"
#include "userdef.h"
#include "proto.h"

#ifdef _WIN32
#   define snprintf _snprintf
#endif

#define CNULL '\0'
static int PgLin;

#ifdef __STDC__
static void PrintFormatted (char *pfmt, CMD_ITMS *ci);
static void NonBoundsLbl (char cClass);
static void StartPage ();
static void TellLabels (LBLDEF *me, int flg, char cClass, int minval);
#else
static void PrintFormatted ();
static void NonBoundsLbl ();
static void PrintComment();
static void StartPage ();
static void TellLabels ();
#endif

extern char *CmdBuf;
extern struct printbuf *pbuf;
extern struct rof_hdr *rofptr;

char pseudcmd[80] = "%5d  %05x %04x %-10s %-6s %-10s %s\n";
char realcmd[80] =  "%5d  %05x %04x %-9s %-10s %-6s %-10s %s\n";
char allcodcmd[80] = "%5d        %04x %04x\n";
char allcodcmd1[80] ="%5d        %04x\n";
char *blankcmd = "%5d";

int PgNum = 0;
int PrevEnt = 0;                /* Previous CmdEnt - to print non-boundary labels */
int InProg;                     /* Flag that we're in main program, so that it won't
                                   munge the label name */
static char ClsHd[100];         /* header string for label equates */
static char FmtBuf[200];        /* Buffer to store formatted string */
int HadWrote;                   /* flag that header has been written */
char *SrcHd;                    /* ptr for header for source file */
char *IBuf;                     /* Pointer to buffer containing the Init Data values */


struct modnam ModTyps[] = {
    {"Prgrm", 1},
    {"Sbrtn", 2},
    {"Multi", 3},
    {"Data", 4},
    {"CSDData", 5},
    {"TrapLib", 11},
    {"Systm", 12},
    {"FlMgr", 13},
    {"Drivr", 14},
    {"Devic", 15},
    {"",0}
};

struct modnam ModLangs[] = {
    {"Objct",    1},
    {"ICode",    2},
    {"PCode",    3},
    {"CCode",    4},
    {"CblCode",  5},
    {"FrtnCode", 6},
    {"", 0}
};

struct modnam ModAtts[] = {
    {"ReEnt", 0x80},
    {"Ghost", 0x40},
    {"SupStat", 0x20},
    {"",0}
};

/* *************************************************************** *
 * adjpscmd () - Adjusts pseudcmd/realcmd label length to          *
 *              NamLen + 2                                         *
 * *************************************************************** */

/*void
adjpscmd (void)
{
    sprintf (pseudcmd, "%s%d%s\n", "%5d  %04X %-14s %-",
                                   NamLen + 2,
                                   "s %-6s %-10s %s");
    sprintf (realcmd, "%s%d%s\n", "%5d  %04X %-04s %-9s %-",
                                  NamLen + 2,
                                  "s %-6s %-10s %s");
}*/

void
tabinit ()
{
    strcpy (realcmd, "%5d\t%04X\t%s\t%s\t%s\t%s\t%s\n");
    strcpy (pseudcmd, "%5d\t%04X\t%s\t\t%s\t%s\t%s\n");
}

/*
 * PrintPsect() - Set up the Psect for printout
 *
 */

void
#ifdef __STDC__
PrintAllCodLine (int w1, int w2)
#else
PrintAllCodLine (w1, w2)
    int w1, w2;
#endif
{
    printf (allcodcmd, LinNum++, w1 & 0xffff, w2 & 0xffff);

    if (PgLin > PgDepth - 3)
    {
        StartPage();
    }
}

void
#ifdef __STDC__
PrintAllCodL1 (int w1)
#else
PrintAllCodL1 (w1)
    int w1;
#endif
{
    printf (allcodcmd1, LinNum++, w1 & 0xffff);

    if (PgLin > PgDepth - 3)
    {
        StartPage();
    }
}

void
PrintPsect()
{
    register struct modnam *pt;
    char *ProgType = NULL;
    char *ProgLang = NULL;
    char *ProgAtts = NULL;
    /*char *StackAddL;*/
    char **prgsets[4];
    unsigned char hdrvals[3];
    int c;
    CMD_ITMS Ci;
    char *psecfld[2];
    int pgWdthSave;

    prgsets[0] = &ProgType; prgsets[1] = &ProgLang;
    prgsets[2] = &ProgAtts; prgsets[3] = NULL;
    psecfld[0] = ",((%s << 8)"; psecfld[1] = " | %s)";
    ProgType = modnam_find (ModTyps, (unsigned char)M_Type)->name;
    hdrvals[0] = M_Type;
    ProgLang = modnam_find (ModLangs, (unsigned char)M_Lang)->name;
    hdrvals[1] = M_Lang;
    ProgAtts = modnam_find (ModAtts, (unsigned char)M_Attr)->name;
    hdrvals[2] = M_Attr;
    strcpy (Ci.mnem, "set");
#ifdef _WIN32
    strcpy (EaString, strrchr(ModFile, '\\') + 1);
#else
    strcpy (EaString, strrchr(ModFile, '/') + 1);
#endif
    strcat (EaString, "_a");
    Ci.comment = "";

    for (c = 0; prgsets[c]; c++)
    {
        if (*prgsets[c])
        {
            Ci.cmd_wrd = hdrvals[c];
            sprintf (Ci.opcode, "%d", hdrvals[c]);
            strcpy(Ci.lblname, *prgsets[c]);
            PrintLine (pseudcmd, &Ci, CNULL, 0, 0); 
        }
        else {
            sprintf (Ci.mnem, "%d", hdrvals[c]);
        }

        sprintf (Ci.opcode, psecfld[c % 2], *prgsets[c]);
        strcat (EaString, Ci.opcode);
    }

    sprintf (&EaString[strlen(EaString)], "|%d)", M_Edit);
    strcat (EaString, ",0");    /* For the time being, don't add any stack */
    sprintf (&EaString[strlen(EaString)], ",%s", findlbl ('L', M_Exec)->sname);
    strcpy (Ci.opcode, EaString);
    strcpy (Ci.mnem, "psect");
    /* Be sure to have enough space to write psect */
    pgWdthSave = PgWidth;
    PgWidth = 200;
    PrintLine (pseudcmd, &Ci, CNULL, 0, 0); 
    PgWidth = pgWdthSave;
}

/* ************************************** *
 * OutputLine () - does the actual output *
 * to the listing and/or source file      *
 * ************************************** */

static void
#ifdef __STDC__
OutputLine (char *pfmt, CMD_ITMS *ci)
#else
OutputLine (pfmt, ci)
char *pfmt; CMD_ITMS *ci;
#endif
{
    LBLDEF *nl;
    char lbl[100];

    if (  InProg &&
        (nl = findlbl ('L', CmdEnt)))
    {
        ci->lblname = nl->sname;

        if (IsROF && nl->global)
        {
            strcat (lbl, ":");
        }
    }

    PrintFormatted (pfmt, ci);

    if (WrtSrc)
    {
        fprintf (AsmPath, "%s %s %s", lbl, ci->mnem, ci->opcode);

        if (strlen (ci->comment))
        {
            fprintf (AsmPath, " %s", ci->comment);
        }

        fprintf (AsmPath, "\n");
    }
}

    /* Straighten/clean up - prepare for next line  */

static void
PrintCleanup ()
{
    PrevEnt = CmdEnt;

    /*CmdLen = 0;*/
    ++PgLin;
    ++LinNum;
}

static void
BlankLine ()                    /* Prints a blank line */
{
    if ( ! PgLin || PgLin > (PgDepth - 5))
    {
        StartPage ();
    }

    printf ("%5d\n", LinNum++);
    ++PgLin;

    if (WrtSrc)
    {
        fprintf (AsmPath, "\n");
    }
}

/* ************************************************************ *
 * PrintNonCmd() - A utility function to print any non-command  *
 *          line (except stored comments).                      *
 *          Prints the line with line number, and updates       *
 *          PgLin & LinNum                                      *
 * Passed: str - the string to print                            *
 *         preblank - true if blankline before str              *
 *         postblank - true if blankline after str              *
 * ************************************************************ */

static void
#ifdef __STDC__
PrintNonCmd (char *str, int preblank, int postblank)
#else
PrintNonCmd (str, preblank, postblank)
    char *str; int preblank, postblank)
#endif
{
    if (IsROF)
    {
        if (preblank)
        {
            BlankLine();
        }

        printf ("%5d %s\n", LinNum, str);

        if (WrtSrc)
        {
            fprintf (AsmPath, "%s", str);
        }

        if (postblank)
        {
            BlankLine();
        }
    }

    PrintCleanup ();
}

/* ******************************************************** *
 * get_comment() - Checks for append comment for current    *
 *              command line.                               *
 * Passed: (1) cClass,                                       *
 *         (2) entry address for command                    *
 * Returns: ptr to comment string if present                *
 *          ptr to empty string if none                     *
 * ******************************************************** */

char *
#ifdef __STDC__
get_apcomment(char clas, int addr)
#else
get_apcomment (clas, addr)
    char clas;
    int addr;
#endif
{
    struct apndcmnt *mytree = CmntApnd[strpos (lblorder, clas)];

    if ( ! clas)
    {
        return NULL;
    }

    if (mytree)
    {
        while (1)
        {
            if (addr < mytree->adrs)
            {
                if (mytree->apLeft)
                {
                    mytree = mytree->apLeft;
                }
                else
                {
                    break;
                }
            }
            else
            {
                if (addr > mytree->adrs)
                {
                    if (mytree->apRight)
                    {
                        mytree = mytree->apRight;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    return mytree->CmPtr;
                }
            }
        }
    }

    return NULL;
}


/* ******************************************************** *
 * PrintLine () - The generic, global printline function    *
 *                It checks for unlisted boundaries, prints *
 *                the line, and then does cleanup           *
 * ******************************************************** */

void
#ifdef __STDC__
PrintLine (char *pfmt, CMD_ITMS *ci, char cClass, int cmdlow, int cmdhi)
#else
PrintLine (pfmt, ci, cClass, cmdlow, cmdhi)
    char *pfmt;
    CMD_ITMS *ci;
    char cClass;
    int cmdlow,
        cmdhi;
#endif
{
    NonBoundsLbl (cClass);            /*Check for non-boundary labels */

    if (cClass)
    {
        PrintComment (cClass, cmdlow, cmdhi);
        ci->comment = get_apcomment(cClass, cmdlow);
    }

    OutputLine (pfmt, ci);

    PrintCleanup ();
}

static void
#ifdef __STDC__
UpString (char *s)              /* Translate a string to uppercase */
#else
UpString (s)
    char *s;
#endif
{
    register int x = strlen (s);

    while (x--)
    {
        if (isalpha (*s))
            *s = toupper (*s);
        ++s;
    }
}

/* *********************************************** *
 * UpPbuf() - Translates a whole print buffer's    *
 *            contents to upper case, if UpCase    *
 * Passed: Pointer to the print buffer to UpCase   *
 * *********************************************** */

/*static void
UpPbuf (struct printbuf *pb)
{
    if (UpCase)
    {
        UpString (pb->instr);
        UpString (pb->opcod);
        UpString(pb->lbnm);
        UpString (pb->mnem);
        UpString(pb->operand);
    }
}*/

static void
#ifdef __STDC__
PrintFormatted (char *pfmt, CMD_ITMS *ci)
#else
PrintFormatted (pfmt, ci)
    char *pfmt; CMD_ITMS *ci;
#endif
{
    int _linlen;

    if ( ! PgLin || PgLin > (PgDepth - 3))
        StartPage ();

    /*if (UpCase)
    {
        UpPbuf (pb);
    }*/

    /* make sure any non-initialized fields don't create Segfault */

    if ( ! ci->lblname)    ci->lblname = "";
    /*if ( ! ci->mnem)        strcpy(ci->mnem, "");
    if ( ! ci->opcode)     strcpy(ci->opcode, "");*/
    if ( ! ci->comment)     ci->comment = "";

    if (pfmt == pseudcmd)
    {
        _linlen = snprintf (FmtBuf, PgWidth - 2, pfmt,
                                    LinNum, CmdEnt, ci->cmd_wrd, ci->lblname,
                                    ci->mnem, ci->opcode, ci->comment);
    }
    else
    {
        _linlen = snprintf (FmtBuf, PgWidth - 2, pfmt,
                                LinNum, CmdEnt, ci->cmd_wrd, ci->lblname,
                                ci->mnem, ci->opcode, ci->comment);
    }

    if ((_linlen >= PgWidth - 2) || (_linlen < 0))
    {
        FmtBuf[PgWidth - 3] = '\n';
        FmtBuf[PgWidth - 2] = '\0';
    }

    printf ("%s", FmtBuf);
    fflush (stdout);
}

static void
StartPage ()
{
    char *bywhom = "* Disassembly by Os9disasm of";
    time_t now;
    struct tm *tm;

    if (PgLin)
    {
        while (PgLin++ < PgDepth)
        {
            printf ("\n");
        }
    }

    ++PgNum;
    PgLin = 0;
    /*.. */

    now = time (0);
    tm = localtime (&now);

    printf
        ("OS9 Cross Disassembler - Ver. %s    %02d/%02d/%02d %02d:%02d:%02d      Page %03d\n\n", VERSION,
         tm->tm_mon + 1, tm->tm_mday, tm->tm_year + 1900, tm->tm_hour,
         tm->tm_min, tm->tm_sec, PgNum);
    PgLin = 2;

    if (PgNum == 1)
    {                           /* print disassembler info on first page */
        LinNum = 1;
        printf ("%5d%20s %s %s\n", LinNum++, "", bywhom, ModFile);

        if (WrtSrc)
        {
            fprintf (AsmPath, "%s %s\n", bywhom, ModFile);
        }

        BlankLine ();
    }
}

/*
 * PrintComment() -Print any comments appropriate
 *
 */

void
#ifdef __STDC__
PrintComment(char lblcClass, int cmdlow, int cmdhi)
#else
PrintComment(lblcClass, cmdlow, cmdhi)
    char lblcClass; int cmdlow, cmdhi)
#endif
{
    register struct commenttree *me;
    register int x;

    for (x = cmdlow; x < cmdhi; x++)
    {
        me = Comments[strpos (lblorder, lblcClass)];

        while (me)
        {
            if (x < me->adrs)
            {
                me = me->cmtLeft;
            }
            else
            {
                if (x > me->adrs)
                {
                    me = me->cmtRight;
                }
                else        /* Assume for now it's equal */
                {
                    struct cmntline *line;

                    line = me->commts;

                    do {
                        printf("%5d       * %s\n", LinNum++, line->ctxt);

                        if (WrtSrc)
                        {
                            fprintf (AsmPath, "* %s\n", line->ctxt);
                        }

                    } while ((line = line->nextline));

                    break;  /* This address done, proceed with next x */
                }
            }
        }
    }
}

static void
#ifdef __STDC__
NonBoundsLbl (char cClass)
#else
    NonBoundsLbl(cClass)
    char cClass;
#endif
{
    if (cClass)
    {
        register int x;
        CMD_ITMS Ci;
        register LBLDEF *nl;

        strcpy (Ci.mnem, "equ");
        Ci.comment = "";

        for (x = PrevEnt + 1; x < CmdEnt; x++)
        {

            if (nl = findlbl (cClass, x))
            {
                Ci.lblname = nl->sname;

                if (x > CmdEnt)
                {
                    sprintf (Ci.opcode, "*+%d", x - CmdEnt);
                }
                else
                {
                    sprintf (Ci.opcode, "*-%d", CmdEnt - x);
                }

                /*PrintLine (pseudcmd, &Ci, cClass, CmdEnt, PCPos);*/
                printf (pseudcmd, LinNum++, nl->myaddr, Ci.cmd_wrd,
                        Ci.lblname, Ci.mnem, Ci.opcode, "");
                ++PgLin;

                if (WrtSrc)
                {
                    fprintf (AsmPath, "%s %s %s\n", Ci.lblname, Ci.mnem,
                             Ci.opcode);
                }
            }
        }
    }
}

/* ****************************** *
 * RsOrg() - print RSDos org line *
 * ****************************** */

/* ********************************************* *
 * ROFPsect() - writes out psect                 *
 * Passed: rof_hdr *rptr                         *
 * ********************************************* */

/*#define OPSCAT(str) sprintf (ci->opcode, "%s,%s", pbuf->operand, str)
#define OPDCAT(nu) sprintf (ci->opcode, "%s,%d", pbuf->operand, nu)
#define OPHCAT(nu) sprintf (pbuf->operand, "%s,%04x", pbuf->operand, nu)*/

void
#ifdef __STDC__
ROFPsect (struct rof_hdr *rptr)
#else
ROFPsect (rptr)
    struct rof_hdr *rptr;
#endif
{
    LBLDEF *nl;
    CMD_ITMS Ci;

    /*strcpy (Ci.instr, "");*/
    strcpy (Ci.opcode, "");
    strcpy (Ci.lblname, "");
    strcpy (Ci.mnem, "psect");
    sprintf (Ci.opcode, "%s,$%x,$%x,%d,%d", rptr->rname,
                                                rptr->ty_lan >> 8,
                                                rptr->ty_lan & 0xff,
                                                rptr->edition,
                                                rptr->stksz
            );
/*#define OPSCAT(str) sprintf (ci->opcode, "%s,%s", pbuf->operand, str)
#define OPDCAT(nu) sprintf (ci->opcode, "%s,%d", pbuf->operand, nu)
#define OPHCAT(nu) sprintf (pbuf->operand, "%s,%04x", pbuf->operand, nu)*/

    if ((nl = findlbl('L', rptr->modent)))
    {
        sprintf (Ci.opcode, "%s,%s", Ci.opcode, nl->sname);
        /*OPSCAT(nl->sname);*/
    }
    else
    {
        sprintf (Ci.opcode, "%s,%04x", Ci.opcode, (int)(rptr->modent));
        /*OPHCAT ((int)(rptr->modent));*/
    }

    CmdEnt = 0;
    PrevEnt = 1;    /* To prevent NonBoundsLbl() output */
    PrintLine (pseudcmd, &Ci, CNULL, 0, 0); 
}


/* ********************************************* *
 * OS9Modline()  - print out OS9 mod line        *
 * Note: We're going to assume that the          *
 *    file is positioned at the right            *
 *    place - we may try merged modules          *
 * ********************************************* */

/*void
OS9Modline ()
{
    struct nlist *LL = SymLst[strpos (lblorder, 'L')];
    struct printbuf PBf, *prtbf = &PBf;
    char hbf[10];
    long progstart = ftell (progpath);

    hbf[0] = '\0';
    InProg = 0;
    memset (prtbf, 0, sizeof (struct printbuf));
    CmdEnt = o9_fgetword (progpath);

    sprintf (prtbf->instr, "%04x", o9_fgetword (progpath));

    PrevEnt = CmdEnt + 1;       /* To prevent NonBoundsLbl() printouts */

    /*strcpy (prtbf->mnem, "mod");

    /* Now copy the operand line */
    /*sprintf (prtbf->operand, "%s,%s,$%02x,$%02x,%s",
             FindLbl (LL, ModSiz)->sname, FindLbl (LL, ModNam)->sname,
             ModTyp, ModRev, FindLbl (LL, ModExe)->sname);

    if (HdrLen == 13)
    {
        strcat (prtbf->operand, ",");
        strcat(prtbf->operand,FindLbl(SymLst[strpos(lblorder,'D')],
                    M_Mem)->sname);
    }

    PrintLine (pseudcmd, prtbf, CNULL, 0, 0);
    InProg = 1;
    fseek (progpath, progstart+HdrLen, SEEK_SET);
}*/

/* **************************************** *
 * coco_wrt_end() - write the end statement *
 * to the listing                           *
 * **************************************** */

void
coco_wrt_end ()
{
    /* Fill in later */
}

/* WrtEmod() - writes EMOD statement to OS9 file */
/*void
WrtEmod ()
{
    struct printbuf PBf, *prtbf = &PBf;
    register struct nlist *nl;
    int first;
    unsigned char last;

    CmdEnt = Pc;
    memset (prtbf, 0, sizeof (struct printbuf));
    first = o9_fgetword (progpath);
    last = fgetc (progpath);
    sprintf(prtbf->instr, "%04x%02x", first, last);
    strcpy (prtbf->mnem, "emod");
    BlankLine ();
/*	PrintLine("%5d  %06x %-12s%s %-10s %-6s %s\n",prtbf);*/
    /*PrintLine (pseudcmd, prtbf, CNULL, 0, 0);
    BlankLine ();
    memset (prtbf, 0, sizeof (struct printbuf));
    Pc += 3;
    CmdEnt = Pc;

    if ((nl = FindLbl (SymLst[strpos (lblorder, 'L')], Pc)))
    {
        strcpy (prtbf->lbnm, nl->sname);
        strcpy (prtbf->mnem, "equ");
        strcpy (prtbf->operand, "*");
        PrintLine (realcmd, prtbf, CNULL, 0, 0);
    }

    BlankLine ();
    memset (prtbf, 0, sizeof (struct printbuf));
    strcpy (prtbf->mnem, "end");
    prtbf->comment = "";

    if (UpCase)
    {
        UpPbuf (prtbf);
    }

    PrintFormatted (realcmd, prtbf);
    /*printf("%5d  %20s%s\n",LinNum,"","end"); */

    /*if (WrtSrc)
    {
        fprintf (AsmPath, " %s\n", "end");
    }
}*/

/* *************************************************** *
 * WrtEnds() - writes the "ends" command line          *
 * *************************************************** */

void
WrtEnds()
{
    CMD_ITMS Ci;

    memset (&Ci, 0, sizeof (Ci));
    strcpy (Ci.mnem, "ends");

    BlankLine();

    PrintFormatted (realcmd, &Ci);

    if (WrtSrc)
    {
        fprintf (AsmPath, "%s %s %s\n", "", "ends", "");
    }

    BlankLine();
}

/* ******************
 * ireflist structure: Represents an entry in the Initialized Refs
 *       list.
 */

struct ireflist {
    struct ireflist *Prev;
    struct ireflist *Next;
    int dAddr;
    LBLDEF *lbl;
    char IClass;
} *IRefs;

void showIRefs()
{
    struct ireflist *ir = IRefs;
    printf ("\n"); 
    while (ir)
    {
        printf ("--- Address: $%04x    Dest: %s ---\n", ir->dAddr, ir->lbl->sname);
        ir = ir->Next;
    }
    printf ("\n");
}

/* *******
 * ParseIRefs() - Parse through the Initialized Refs list for either
 *    class 'D' or 'L'.
 *    Insert appropriate labels into Label Trees and the
 *    IRef table.
 */

void
#ifdef __STDC__
ParseIRefs(char rClass)
#else
ParseIRefs(rClass)
    char rClass;
#endif
{
    register int rCount;  /* The count for this block */
    register int MSB;
    register int dLoc;   /* The actual D location (with MSB appended) */

    LoadIData();  /* Make sure Init data list is loaded */

    /* Get an initial reading */
    MSB = fread_w(ModFP) << 16;
    rCount = fread_w(ModFP);

    while (MSB || rCount)   /* This will get All blocks for the Location */
    {
        while (rCount--)
        {
            struct ireflist *il, *ilpt;
            register int lblLoc;
            char *lblPos;
            int pCount;

            if ( !(il = (struct ireflist *)calloc (1, sizeof(struct ireflist))))
            {
                errexit ("Fatal: Cannot allocate memory for IrefList member");
            }

            il->dAddr = MSB | (fread_w(ModFP) & 0xffff);
            lblLoc = 0;
            lblPos = &IBuf[il->dAddr - IDataBegin];

            for (pCount = 0; pCount < 4; pCount++)
            {
                lblLoc = (lblLoc << 8) | (*(lblPos++));
            }

            il->lbl = addlbl (rClass, lblLoc, NULL);
            il->IClass = rClass;

            if (IRefs)   /* First entry? */
            {
                ilpt = IRefs;

                if (il->dAddr < ilpt->dAddr)
                {
                    il->Next = ilpt;
                    ilpt->Prev = il;
                    IRefs = il;
                }
                else
                {
                        /* Find iref PRECEDING this entry */
                    while (ilpt->Next && (il->dAddr >= ilpt->Next->dAddr))
                    {
                        ilpt = ilpt->Next;
                    }

                    if (ilpt->Next)
                    {
                        ilpt->Next->Prev = il;
                    }

                    il->Next = ilpt->Next;
                    il->Prev = ilpt;
                    ilpt->Next = il;             
                }
            }
            else
            {
                IRefs = il;
            }
        }

        MSB = fread_w(ModFP);
        rCount = fread_w(ModFP);
    }
}

/*
 * GetIRefs() - The mainline routine for getting the Initialized Refs.
 *    Seeks to the proper location and then calls ParseIRefs for each
 *    of 'D' and 'L'
 */

void
GetIRefs()
{

    if (M_IRefs == 0)
        return;

    if (fseek (ModFP, M_IRefs, SEEK_SET))
    {
        errexit ("Fatal: Failed to seek to Initialized Refs location");
    }

    ParseIRefs('L');
    ParseIRefs('D');
}

/* *************************************************** *
 * ROFDataPrint() - Mainline routine to list data defs *
 *          for ROF's                                  *
 * *************************************************** */

/*void
ROFDataPrint ()
{
    struct nlist *dta, *srch;
    char *dptell[2] = {"* Uninitialized data (cClass %c)",
                       "* Initialized Data (cClass %c)"};
    int sizes[4] = { rofptr->udpsz, rofptr->idpsz,
                     rofptr->udatsz, rofptr->idatsz
                   },
        *thissz = sizes;

    int vs,
        isinit;
    int reftyp[] = {2, 3, 0, 1};     /* Label ref Type */
    /*char dattmp[5];
    char *dattyp = dattmp;
    char mytmp[50];

    InProg = 0;
    memset (pbuf, 0, sizeof (struct printbuf));*/

    /* We compute dattyp for flexibility.  If we change the label type
     * specification all we have to do is change it in rof_cClass() and it
     * should work here automatically rather than hard-coding the cClasses
     */

    /*dattyp[4] = '\0';

    for (vs = 0; vs < 5; vs++)
    {
        dattyp[vs] = rof_cClass (reftyp[vs]);
    }

    for ( vs = 0; vs <= 1; vs++)    /* Cycle through DP, non-dp */
    /*{
        if ((thissz[0]) || thissz[1])
        {
            strcpy (pbuf->mnem, "vsect");

            if (!vs)
            {
                strcpy (pbuf->operand, "dp");
            }
            else
            {
                strcpy (pbuf->operand, "");
            }

            BlankLine();
            PrintLine (realcmd, pbuf, dattyp[vs], 0, 0);
            BlankLine();*/

            /* Process each of un-init, init */

            /*for (isinit = 0; isinit <= 1; isinit++)
            {
                dta = labelclass (dattyp[isinit]);

                sprintf (mytmp, dptell[isinit], dattyp[isinit]);

                if (isinit)
                {
                    if (thissz[isinit])
                    {
                        PrintNonCmd (mytmp, 0, 1);
                    }

                    ListInitROF (dta, thissz[isinit], vs, dattyp[isinit]);
                }
                else
                {
                    if (dta)
                    {*/
                        /* for PrintNonCmd(), send isinit so that a pre-blank
                         * line is not printed, since it is provided by
                         * PrinLine above */

                       /* if (thissz[isinit])
                        {
                            PrintNonCmd (mytmp, isinit, 1);
                        }

                        srch = dta;

                        while (srch->LNext)
                        {
                            srch = srch->LNext;
                        }

                        if (srch->myaddr)
                        {*/                       /* i.e., if not D000 */
                            /*strcpy (pbuf->mnem, "rmb");
                            sprintf (pbuf->operand, "%d", srch->myaddr);
                            CmdEnt = PrevEnt = 0;
                            PrintLine (realcmd, pbuf, dattyp[isinit], 0,
                                                      srch->myaddr);
                        }/*

                        /* For max value, send a large value so ListData
                         * will print all for cClass
                         */

                       /* M_Mem = thissz[isinit];
                        ListData (dta, thissz[isinit], dattyp[isinit]);
                   /* }*/           /* end "if (dta)" */
                   /* else*/        /* else no labels.. check to see */
                    /*{*/           /* if any "hidden" variables */
                        /*if (thissz[isinit])
                        {
                            PrintNonCmd (mytmp, isinit, 1);
                            strcpy (pbuf->mnem, "rmb");
                            sprintf (pbuf->operand, "%d", thissz[isinit]);
                            PrintLine (realcmd, pbuf, dattyp[isinit], 0,
                                                        0);
                        }
                    }
                }
            }*/

            /* Do "ends" for this vsect */

           /* WrtEnds();
        }

        dattyp += 2;*/   /* Done with this cClass, point to next */
        /*thissz += 2;*/   /* And to the next data size */
    /*}

    BlankLine ();
    InProg = 1;
}*/


/* *************
 * LoadIData() - Allocate a buffer for the Init Data list
 *       and load the data
 */

char *
LoadIData()
{
    if (!IBuf)
    {
        /* Allocate space for buffer */ 
        if ( !(IBuf = (char *)malloc(IDataCount + 2)))
        {
            errexit("Error! Cannot allocate memory for Init Data Buffer");
        }

        if (fseek (ModFP, M_IData + 8, SEEK_SET))
        {
            char en[100];

            sprintf (en, "Error.. Cannot fseek() to Init Data list (%x)", M_IData + 8);
            errexit (en);
        }

        /* Read data into buffer*/
        if (fread (IBuf, 1, IDataCount, ModFP) < IDataCount)
        {
            errexit( "Cannot read Initialized Data bytes");
        }
    }

    return IBuf;
}


/* *************
 * ListInitData ()
 *
 */

 static void
#ifdef __STDC__
ListInitData (LBLDEF *ldf, int nBytes, char lclass)
#else
ListInitData (ldf, nBytes, lclass)
    LBLDEF *ldf; int nBytes; char lclass;
#endif
{
    CMD_ITMS Ci;
    register char *curpos;
    struct ireflist *il;
    char *hexFmt;

    Ci.cmd_wrd = 0;
    Ci.comment = "";
    Ci.mnem[0] = '\0';
    Ci.lblname = "";
    Ci.wcount = 0;
    strcpy (Ci.opcode, "* Initialized Data Definitions");
    NowClass = 'D';
    PCPos = IDataBegin;        /* MovBytes/MovASC use PCPos */
    CmdEnt = PCPos;

    il = IRefs;

    curpos = IBuf;
    BlankLine();
    PrintLine (pseudcmd, &Ci, lclass, ldf->myaddr, ldf->myaddr);
    Ci.opcode[0] = '\0';
    BlankLine();
    AMode = 0;             /* Mode for Data             */

    while (nBytes > 0)
    {
        register int lblCount,
            ppos;
        int isAsc;

        /* Get byte count for this label */
        if (ldf->Next)
            lblCount = ldf->Next->myaddr - ldf->myaddr;
        else
            lblCount = nBytes;

        if (lblCount > nBytes)
        {
            lblCount = nBytes;  /* Don't go past the end of data */
        }

        CmdEnt = PCPos;     /* Save Entry Point */
        ppos = lblCount;
        Ci.lblname = ldf->sname;
        isAsc = 0;

        /* Check to see if the whole block might be ASCII */
        if ((il->dAddr >= PCPos + lblCount) && (isprint(*curpos) || isspace(*curpos)))
        {
            register char *ch = curpos;
            isAsc = 1;

            while (ppos--)
            {
                int c = *(ch++);

                if ( !(isprint (c) || (isspace(c)) || (c == 0)))
                {
                    isAsc = 0;
                    break;
                }
            }
        }

        if (isAsc)
        {
            register char *ch = curpos;
            char *strEnd = &curpos[lblCount];
            strcpy (Ci.mnem, "dc.b");

            while (strlen(curpos) && (curpos < strEnd))
            {
                char tmpbuf[30];

                strncpy (tmpbuf, curpos, 24);

                if (strlen(tmpbuf) >= 24)
                    tmpbuf[24] = '\0';

                AddDelims (Ci.opcode, tmpbuf);
                curpos += strlen(tmpbuf);
                lblCount -= strlen(tmpbuf);
                nBytes -= strlen(tmpbuf);
                PCPos += strlen(tmpbuf);
                PrintLine (pseudcmd, &Ci, 'D', CmdEnt, CmdEnt);
                CmdEnt = PCPos;
                PrevEnt = PCPos;
                Ci.lblname = "";
                Ci.opcode[0] = '\0';
            }
            /*MovASC (lblCount, lclass);*/
        }

        /* We might ought to provide for longs, but it might
        * be more confusing */
        if (lblCount & 1)
        {
            PBytSiz = 1;
            strcpy (Ci.mnem, "dc.b");
            hexFmt = "$%02x";
        }
        else
        {
            PBytSiz = 2;
            strcpy (Ci.mnem, "dc.w");
            hexFmt = "$%04x";
        }

        /*Ci.lblname = findlbl ('D', CmdEnt)->sname;
        Ci.opcode[0] = '\0';*/
        ppos = lblCount;

        while (ppos > 0)
        {
            char tmp[20];
            int val;

            if (PCPos == il->dAddr)
            {
                register int count;
                val = 0;
                tmp[0] = '\0';

                for (count = 0; count < 4; count++)
                {
                    val = (val << 8) | ((*curpos++) & 0xff);
                }

                strcpy (Ci.opcode, il->lbl->sname);
                strcpy (Ci.mnem, "dc.l");
                OutputLine (pseudcmd, &Ci);
                il = il->Next;
                Ci.lblname = "";
                Ci.opcode[0] = '\0';
                PCPos += 4;
                CmdEnt = PCPos;
                ppos -= 4;
                PrevEnt = PCPos;
                continue;
            }

            switch (PBytSiz)
            {
            case 1:
                sprintf (tmp, "$%02x", *(curpos++));
                break;
            case 2:
                val = *(curpos++);
                val = (val << 8) | (*(curpos++) & 0xff);
                sprintf (tmp, "$%04x", val);
                break;
            }

            if (strlen(Ci.opcode))
            {
                strcat (Ci.opcode, ",");
            }

            strcat (Ci.opcode, tmp);
            ppos -= PBytSiz;
            PCPos += PBytSiz;

            if (strlen(Ci.opcode) > 24)
            {
                PrevEnt = PCPos;
                OutputLine (pseudcmd, &Ci);
                Ci.lblname = "";
                Ci.opcode[0] = '\0';
                Ci.wcount = 0;
                CmdEnt = PCPos;
            }
        }

        if (strlen(Ci.opcode))   /* Any final cleanup */
        {
            PrevEnt = PCPos;
            OutputLine (pseudcmd, &Ci);
            Ci.wcount = 0;
            Ci.opcode[0] = '\0';
        }

        CmdEnt = PCPos;
        isAsc = 1;

        nBytes -= lblCount;
        ldf = ldf->Next;
    }
}

/* ************
 * OS9DataPrint()	Mainline routine to list data defs
 *
 */

void
OS9DataPrint ()
{
    LBLDEF *dta, *srch;
    char *what = "* OS9 data area definitions";
    CMD_ITMS Ci;
    long filePos = ftell (ModFP);
    
    if (!M_IData)
    {
        IDataBegin = M_Mem;
        IDataCount = 0;
    }

    InProg = 0;    /* Stop looking for Inline program labels to substitute */
    memset (&Ci, 0, sizeof (Ci));
    dta = labelclass ('D') ? labelclass('D')->cEnt : NULL;

    if (dta)
    {                           /* special tree for OS9 data defs */
        BlankLine ();
        printf ("%5d %22s%s\n", LinNum++, "", what);
        ++PgLin;

        if (WrtSrc)
        {
            fprintf (AsmPath, "%s\n", what);
        }

        BlankLine ();

        strcpy (Ci.mnem, "vsect");
        strcpy (Ci.opcode, "");
        Ci.cmd_wrd = 0;
        Ci.comment = "";
        CmdEnt = PrevEnt = 0;
        PrintLine (pseudcmd, &Ci, 'D', 0, 0);

        /*first, if first entry is not D000, rmb bytes up to first */
        srch = dta;

        /*while (srch->LNext)
        {
            srch = srch->LNext;
        }*/

        if (srch->myaddr)              /* i.e., if not D000 */
        {
            strcpy (Ci.mnem, "ds.b");
            sprintf (Ci.opcode, "%d", srch->myaddr);
            Ci.lblname = "";
            PrintLine (pseudcmd, &Ci, 'D', 0, srch->myaddr);
        }

        ListData (dta, IDataBegin, 'D');

        if (IDataBegin < M_Mem)
        {
            if (dta = findlbl('D', IDataBegin))
            {
                ListInitData (dta, IDataCount, 'D');
            }
        }
    }
    else
    {
        return;
    }

    strcpy(Ci.mnem, "ends");
    strcpy (Ci.opcode, "");
    Ci.cmd_wrd = 0;
    Ci.comment = "";
    PrintLine (pseudcmd, &Ci, 'D', CmdEnt, CmdEnt);
    BlankLine ();
    CmdEnt = PrevEnt = 0;
    InProg = 1;
    fseek (ModFP, filePos, SEEK_SET);
}

/* ******************************************************** *
 * ListData() - recursive routine to print rmb's for Data   *
 *              definitions                                 *
 * Passed: pointer to current nlist element                 *
 *         address of upper (or calling) ListData() routine *
 *         Label cClass                                      *
 * ******************************************************** */

void
#ifdef __STDC__
ListData (LBLDEF *me, int upadr, char cClass)
#else
ListData (me, upadr, cClass)
    LBLDEF *me; int upadr; char cClass;
#endif
{
    CMD_ITMS Ci;
    register LBLDEF *srch;
    register int datasize;

    memset (&Ci, 0, sizeof (Ci));

    /* Process lower entries first */

    /*if (me->LNext)
    {
        ListData (me->LNext, me->myaddr, cClass);
    }*/

    /* Don't print non-data elements here */

    if (me->myaddr >= upadr)
    {
        return;
    }

    /* Now we've come back, print this entry */

    /*strcpy (pbf->lbnm, me->sname);*/

    if (IsROF && me->global)
    {
        strcat (me->sname, ":");
    }

    /*if (me->RNext)
    {
        srch = me->RNext;*/       /* Find smallest entry in that list */

        /*while (srch->LNext)
        {
            srch = srch->LNext;
        }

        datasize = (srch->myaddr) - (me->myaddr);
    }
    else
    {
        datasize = (upadr) - (me->myaddr);
    }*/

    /* Don't print any Class 'D' variables which are not in Data area */
    /* Note, Don't think we'll get this far, we have a return up above,
     * but keep this one till we know it works

    if ((OSType == OS_9) && (me->myaddr > M_Mem))
    {
        return;
    }*/

    while (me)
    {
        if (me->myaddr >= upadr)
        {
            break;
        }

        if (me->Next)
        {
            datasize = me->Next->myaddr - me->myaddr;
        }
        else
        {
            datasize = upadr - me->myaddr;
        }

        strcpy (Ci.mnem, "ds.b");
        sprintf (Ci.opcode, "%d", datasize);
        Ci.lblname = me->sname;
        /*if (me->myaddr != upadr)
        {
            strcpy (Ci.mnem, "ds.b");
            sprintf (Ci.opcode, "%d", datasize);
            Ci.lblname = me->sname;
        }
        else
        {
            if (IsROF)
            {
                strcpy (Ci.mnem, "ds.b");
                sprintf (Ci.opcode, "%d", datasize);
            }
            else
            {
                strcpy (Ci.mnem, "ds.b");
                strcpy (Ci.opcode, "ds.b");
            }
        }*/

        CmdEnt = me->myaddr;
        PrevEnt = CmdEnt;
        PrintLine (pseudcmd, &Ci, cClass, me->myaddr, (me->myaddr + datasize));
        me = me->Next;

        /*if (me->RNext && (me->myaddr < M_Mem))
        {
            ListData (me->RNext, upadr, cClass);
        }*/
    }
}

/* ************************************************** *
 * WrtEquates() - Print out label defs                *
 * Passed: stdflg - 1 for std labels, 0 for externals *
 * ************************************************** */

void
#ifdef __STDC__
WrtEquates (int stdflg)
#else
WrtEquates (stdflg)
    int stdflg;
#endif
{
    char *claspt = "!^ABCDEFGHIJKMNOPQRSTUVWXYZ;",
        *curnt = claspt,
        *syshd = "* OS-9 system function equates\n",
        *aschd = "* ASCII control character equates\n",
        *genhd[2] = { "* Class %c external label equates\n",
                      "* Class %c standard named label equates\n"
                    };
    register int flg;           /* local working flg - clone of stdflg */
    LBLDEF *me;

    InProg = 0;
    curnt = claspt;

    if ( ! stdflg)                /* print ! and ^ only on std cClass pass */
    {
        curnt += 2;
    }

    while ((NowClass = *(curnt++)) != ';')
    {
        int minval;

        flg = stdflg;
        strcpy (ClsHd, "%5d %21s");
        me = labelclass (NowClass) ? labelclass (NowClass)->cEnt : NULL;

        if (me)
        {
            /* For OS9, we only want external labels this pass */

            if (NowClass == 'D')
            {
                if (stdflg)     /* Don't print data defs */
                {
                    continue;
                }

                /* Probably an error if this happens
                 * What we're doing is positioning me to
                 * last real data element*/

               /* if (!(me = FindLbl (me, M_Mem)))*/
                if (! (me = findlbl (NowClass, M_Mem)))
                {
                    continue;
                }
            }

            /* Don't write vsect data for ROF's */

/*            if ((IsROF) && stdflg && strchr ("BDGH", NowClass))
            {
                continue;
            }*/

            switch (NowClass)
            {
                case '!':
                    strcat (ClsHd, syshd);
                    SrcHd = syshd;
                    flg = -1;
                    break;
                case '^':
                    strcat (ClsHd, aschd);
                    SrcHd = aschd;
                    flg = -1;
                    break;
                default:
                    strcat (ClsHd, genhd[flg]);
                    SrcHd = genhd[flg];
            }

            HadWrote = 0;       /* flag header not written */

            /* Determine minimum value for printing *
             * minval will be the first value to    *
             * print                                */

            minval = 0;     /* Default to "print all" */

            if (IsROF)
            {
                /*minval = rof_datasize (NowClass);*/

                /* If this cClass has any data, we want to exclude
                    * printing the last entry.
                    * Otherwise, if no real entries, we want to print
                    * element "0"
                    */

                /*if (minval)
                {
                    ++minval;
                }*/
            }
            else
            {
                if (NowClass == 'D')
                {
                    minval = M_Mem + 1;
                }
                else {
                    if (NowClass == 'L')
                    {
                        minval = M_Size + 1;
                    }
                }
            }

            TellLabels (me, flg, NowClass, minval);
        }
    }

    InProg = 1;
}

/* TellLabels(me) - Print out the labels for cClass in "me" tree */

static void
#ifdef __STDC__
TellLabels (LBLDEF *me, int flg, char cClass, int minval)
#else
TellLabels (me, flg, cClass, minval)
    LBLDEF *me;
    int flg;
    char cClass;
    int minval;
#endif
{
    CMD_ITMS Ci;

    memset (&Ci, 0, sizeof (Ci));
    strcpy (Ci.mnem, "equ");
   /* if (me->LNext)
    {
        TellLabels (me->LNext, flg, cClass, minval);
    }*/
    
    while (me)
    {
        if ((flg < 0) || (flg == me->stdname))
        {
            /* Don't print real OS9 Data variables here */

            if (me->myaddr >= minval)
            {
                if ( ! HadWrote)
                {
                    BlankLine ();
                    printf (ClsHd, LinNum++, "", cClass);
                    ++PgLin;

                    if (AsmPath)
                    {
                        fprintf (AsmPath, SrcHd, cClass);
                    }

                    HadWrote = 1;
                    BlankLine ();
                }

                Ci.lblname = me->sname;

                if (strchr ("!^", cClass))
                {
                    sprintf (Ci.opcode, "$%02x", me->myaddr);
                }
                else
                {
                    sprintf (Ci.opcode, "$%05x", me->myaddr);
                }

                CmdEnt = PrevEnt = me->myaddr;
                PrintLine (pseudcmd, &Ci, cClass, me->myaddr, me->myaddr + 1);
            }
        }

        me = me->Next;
       /* if (me->RNext)
        {
            TellLabels (me->RNext, flg, cClass, minval);
        }*/
    }
}
