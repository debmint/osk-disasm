
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * dismain.c - The main disassembler routine.  This handles a single   $
 *      pass for the disassembler routine. For each command, it passes $
 *      off the job of disassembling that command.                     $
 *                                                                     $
 * $Id::                                                               $
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define _MAIN_  /* We will define all variables in one header file, then
                   define them extern from all other files */

#include <ctype.h>
#include "disglobs.h"
#include "textdef.h"
#include "modtypes.h"
#include <string.h>
#include "proto.h"

#ifdef _WIN32
#   define strcasecmp _stricmp
#endif

#ifdef _OSK
#   define strcasecmp strcmp
#endif

/* Some variables that are only used in one or two modules */
int LblFilz;              /* Count of Label files specified     */
char *LblFNam[MAX_LBFIL]; /* Pointers to the path names for the files */

static int HdrLen;
static int CodeEnd;

extern struct databndaries *dbounds;
extern char realcmd[], pseudcmd[];

static int get_asmcmd(
#ifdef __STDC__
    void
#endif
);

/*
 * list_print() - Handle printouts of lines of code.  Prints to the listing
 *        and prints to asm code file if specified
 */

static void
#ifdef __STDC__
list_print (CMD_ITMS *ci, short ent, char *lblnam)
#else
list_print (ci, ent, lblnam)
    CMD_ITMS *ci;
    int ent;
    char *lblnam;
#endif
{
    register char *ListFmt = "%05x %04x %10.10s %s %s\n";
    register char *CmntFmt = "%05x %04x %10.10s %s %s %s\n";
    char *lb;

    lb = (lblnam ? lblnam : lblstr('L', CmdEnt));

    /* When ready, need to provide for option whether to print or not */
    if (ci->comment)
    {
        printf(CmntFmt, ent & 0xffff, ci->cmd_wrd, lb,
                        ci->mnem, ci->opcode, ci->comment);
    }
    else
    {
        printf(ListFmt, ent & 0xffff, ci->cmd_wrd, lb,
                        ci->mnem, ci->opcode);
    }
    /* Provide for printing to asm src listing */

}

static char *DrvrJmps[] = {
    "Init", "Read", "Write", "GetStat", "SetStat", "Term", "Except", NULL
};

static char *FmanJmps[] = {"Open", "Create", "Makdir", "Chgdir", "Delete", "Seek",
        "Read", "Write", "Readln", "Writeln", "Getstat", "Setstat", "Close", NULL};

/* ***********************
 * get_drvr_jmps()
 *    Read the Driver initialization table and set up
 *    label names.
 */

static void
#ifdef __STDC__
get_drvr_jmps(int mty)
#else
get_drvr_jmps(mty)
    int mty;
#endif
{
    register char **pt;
    register int jmpdst;
    int jmpbegin = ftell (ModFP);
    char boundstr[50];

    if (mty == MT_DEVDRVR)
    {
        pt = DrvrJmps;
    }
    else
    {
        pt = FmanJmps;
    }

    while (*pt)
    {
        jmpdst = fread_w(ModFP);

        if (jmpdst)
        {
            addlbl ('L', jmpdst, *pt);
            sprintf (boundstr, "L W L %x-%x", jmpbegin, ftell(ModFP) - 1);
        }
        else
        {
            sprintf (boundstr, "L W & %x-%x", jmpbegin, ftell(ModFP) - 1);
        }

        setupbounds (boundstr);
        jmpbegin = ftell(ModFP);
        ++pt;
    }

}

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
        /* Now get any further Mod-type specific headers */

        switch (M_Type)
        {
        case MT_PROGRAM:
        case MT_SYSTEM:
        case MT_TRAPLIB:
        case MT_FILEMAN:
        case MT_DEVDRVR:
            M_Exec = fread_l(ModFP);
            
            /* Add label for Exception Handler, if applicable */
            /* Only for specific Module Types??? */
            if (M_Except = fread_l(ModFP))
            {
                addlbl('L', M_Except, "");
            }

            /* Add btext */
            /* applicable for only specific Moule Types??? */
            addlbl ('L', 0, "btext");

            HdrEnd = ftell(ModFP); /* We'll keep changing it if necessary */

            if ((M_Type != MT_SYSTEM) && (M_Type != MT_FILEMAN))
            {
                M_Mem = fread_l(ModFP);

                if (M_Type != MT_DEVDRVR)
                {
                    M_Stack = fread_l(ModFP);
                    M_IData = fread_l(ModFP);
                    M_IRefs = fread_l( ModFP);

                    if (M_Type == MT_TRAPLIB)
                    {
                        M_Init = fread_l(ModFP);
                        M_Term = fread_l(ModFP);
                    }
                }

                HdrEnd = ftell(ModFP);  /* The final change.. */
            }

            if ((M_Type == MT_DEVDRVR) || (M_Type == MT_FILEMAN))
            {
                fseek (ModFP, M_Exec, SEEK_SET);
                get_drvr_jmps (M_Type);
            }

           if (M_IData)
            {
                fseek (ModFP, M_IData, SEEK_SET);
                IDataBegin = fread_l (ModFP);
                IDataCount = fread_l (ModFP);
                /* Insure we have an entry for the first Initialized Data */
                addlbl ('D', IDataBegin, "");
                CodeEnd = M_IData;
            }
           else
           {
               /* This may be incorrect */
               CodeEnd = M_Size - 3;
           }
        }

        fseek (ModFP, 0, SEEK_END);

        if (ftell(ModFP) < M_Size)
        {
            errexit ("\n*** ERROR!  File size < Module Size... Aborting...! ***\n");
        }

        break;
    case 0xdead:
        errexit("Disassembly of ROF files is not yet implemented.");
        break;
    default:
        errexit("Unknown module type");
    }

    return 0;
}

/*
 * modnam_find() - search a modna struct for the desired value
 * Returns: On Success: Pointer to the desired value
 *          On Failure: NULL
 */

struct modnam *
#ifdef __STDC__
modnam_find (struct modnam *pt, int desired)
#else
modnam_find (pt, desired)
    struct modnam *pt;
    int desired;
#endif
{
    while ((pt->val) && (pt->val != desired))
    {
        ++pt;
    }

    return (pt->val ? pt : NULL);
}

/* ******************************************************************** *
 * RdLblFile() - Reads a label file and stores label values into label  *
 *      tree if value (or address) is present in tree                   *
 *      Path to file has already been opened and is stored in "inpath"  *
 * ******************************************************************** */

static void
#ifdef __STDC__
RdLblFile (FILE *inpath)
#else
RdLblFile (inpath)
    FILE *inpath;
#endif
{
    char labelname[30],
         clas,
         eq[10],
         strval[15],
        *lbegin;
    int address;
    LBLDEF *nl;

    while ( ! feof (inpath))
    {
        char rdbuf[81];

        fgets (rdbuf, 80, inpath);

        if ( ! (lbegin = skipblank (rdbuf)) || (*lbegin == '*'))
        {
            continue;
        }

        if (sscanf (rdbuf, "%s %s %s %c", labelname, eq, strval, &clas) == 4)
        {
#ifdef _OSK
              register int c;
#endif
            clas = toupper (clas);

#ifdef _OSK
            for (c = 0; c < 3; c++)
            {
                eq[c] = _tolower(eq[c]);
            }
#endif
            if ( ! strcasecmp (eq, "equ"))
            {
                /* Store address in proper place */

                if (strval[0] == '$')   /* if represented in hex */
                {
                    sscanf (&strval[1], "%x", &address);
                }
                else
                {
                    address = atoi (strval);
                }

                nl = findlbl (clas, address);

                if (nl)
                {
                    strncpy (nl->sname, labelname, sizeof(nl->sname) - 1);
                    nl->sname[sizeof(nl->sname)] = '\0';
                    nl->stdname = 1;
                }
            }
        }
    }
}

/* ******************************************************** *
 * GetLabels() - Set up all label definitions               *
 *      Reads in all default label files and then reads     *
 *      any files specified by the '-s' option.             *
 * ******************************************************** */

static void
GetLabels ()                    /* Read the labelfiles */
{
    register int x;
    register struct nlist *nl;
    char  filename[500];

    /* First, get built-in ascii definitions.  Later, if desired,
     * they can be redefined */

    /*for (x = 0; x < 33; x++)
    {
        if ((nl = FindLbl (ListRoot ('^'), x)))
        {
            strcpy (nl->sname, CtrlCod[x]);
        }
    }

    if ((nl = FindLbl (ListRoot ('^'), 0x7f)))
    {
        strcpy (nl->sname, "del");
    }*/

    /* Next read in the Standard systems names file */

    /*if ((OSType == OS_9) || (OSType == OS_Moto))
    {
        tmpnam = build_path ("sysnames");

        if ( ! (inpath = fopen (tmpnam, "rb")))
            fprintf (stderr, "Error in opening Sysnames file..%s\n",
                              filename);
        else
        {
            RdLblFile ();
            fclose (inpath);
        }
    }*/

    /* Now read in label files specified on the command line */

    for (x = 0; x < LblFilz; x++)
    {
        register FILE *inpath;

        if ((inpath = build_path (LblFNam[x], BINREAD)))
        {
            RdLblFile (inpath);
            fclose (inpath);
        }
        else
        {
            fprintf (stderr, "ERROR! cannot open Label File %s for read\n",
                     LblFNam[x]);
        }
    }
}

/*
 * dopass() - Do a complete single pass through the module.
 *      The first pass establishes the addresses of necessary labels
 *      On the second pass, the desired label names have been inserted
 *      and printing of the listing and writing of the source file is
 *      done, if either or both is desired.
 */

int
#ifdef __STDC__
dopass(int argc,char **argv,int mypass)
#else
dopass(argc,argv,mypass)
    int argc;
    char **argv;
    int mypass;
#endif
{
    int sval = 0;
    int lval = 0;
    int wval = 0;

    if (Pass == 1)
    {
        if (!(ModFP = fopen(ModFile, BINREAD)))
        {
            errexit("Cannot open Module file for read");
        }

        PCPos = 0;
        get_modhead();
        PCPos = ftell(ModFP);
        /*process_label(&Instruction, 'L', M_Exec);*/
        addlbl ('L', M_Exec, NULL);
        
        /* Set Default Addressing Modes according to Module Type */
        switch (M_Type)
        {
        case MT_PROGRAM:
            strcpy(DfltLbls, "&&&&&&D&&&&L");
            break;
        case MT_DEVDRVR:
            /*  Init/Term:
                 (a1)=Device Dscr
                Read/Write,Get/SetStat:
                 (a1)=Path Dscr, (a5)=Caller's Register Stack
                All:
                 (a1)=Path Dscr
                 (a2)=Static Storage, (a4)=Process Dscr, (a6)=System Globals

               (a1) & (a5) default to Read/Write, etc.  For Init/Term, specify
               AModes for these with Boundary Defs*/
            strcpy (DfltLbls, "&ZD&PG&&&&&L");
            break;
        }

        GetIRefs();
        do_cmd_file();

    }
    else   /* Do Pass 2 Setup */
    {
        /*parsetree ('A');*/
        GetLabels();
        WrtEquates (1);
        WrtEquates (0);
        CmdEnt = 0;
        PrintPsect();

        /*if (IsROF)
        {
        }
        else*/
        {
            OS9DataPrint();
        }
    }

    /* NOTE: This is just a temporary kludge The begin _SHOULD_ be
             the first byte past the end of the header, but until
             we get bounds working, we'll do  this. */
    if (fseek(ModFP, HdrEnd, SEEK_SET) != 0)
    {
        errexit("FIle Seek Error");
    }

    PCPos = HdrEnd;

    while (PCPos < CodeEnd)
    {
        struct databndaries *bp;

        Instruction.comment = NULL;
        Instruction.lblname = "";
        CmdEnt = PCPos;

        /* NOTE: The 6809 version did an "if" and it apparently worked,
         *     but we had to do this to get it to work with consecutive bounds.
         */
        while (bp = ClasHere (dbounds, PCPos))
        {
            NsrtBnds (bp);
            CmdEnt = PCPos;
        }
       
        if (get_asmcmd())
        {
            if (Pass == 2)
            {
                char *ilcmnt;

                /*PrintComment('L', CmdEnt, PCPos);*/
                Instruction.comment = get_apcomment ('L', CmdEnt);
                /*list_print (&Instruction, CmdEnt, NULL);*/
                PrintLine (pseudcmd, &Instruction, 'L', CmdEnt, PCPos);

                if (PrintAllCode && Instruction.wcount)
                {
                    int count = Instruction.wcount;
                    int wpos = 0;
                    /*printf("%6s", "");*/

                    while (count)
                    {
                        if (count > 1)
                        {
                            PrintAllCodLine(Instruction.code[wpos], Instruction.code[wpos + 1]);
                            count -= 2;
                            wpos += 2;
                        }
                        else
                        {
                            PrintAllCodL1(Instruction.code[wpos]);
                            --count;
                            ++wpos;
                        }
                       /*printf("%04x ", Instruction.code[count] & 0xffff);*/
                    }
                }
            }
        }
        else
        {
            if (Pass == 2)
            {
                strcpy (Instruction.mnem, "dc.w");
                sprintf (Instruction.opcode, "$%x", Instruction.cmd_wrd & 0xffff);
                PrintLine (pseudcmd, &Instruction, 'L', CmdEnt, PCPos);
                CmdEnt = PCPos;
                /*list_print (&Instruction, CmdEnt, NULL);*/
                
            }
        }
    }

    if (Pass == 2)
    {
        WrtEnds();
    }

    return 0;
}

static CMD_ITMS *
#ifdef __STDC__
initcmditems (CMD_ITMS *ci)
#else
initcmditems (ci)
    CMD_ITMS *ci;
#endif
{
    ci->mnem[0] = 0;
    ci->wcount = 0;
    ci->opcode[0] ='\0';
    ci->comment = NULL;
    return ci;
}

/*
 * noimplemented() - A dummy function which simply returns NULL for instructions
 *       that do not yet have handler functions
 */

int
#ifdef __STDC__
notimplemented(CMD_ITMS *ci, int tblno, OPSTRUCTURE *op)
#else
notimplemented (ci, tblno, op)
    CMD_ITMS *ci;
    int tblno;
    OPSTRUCTURE *op;
#endif
{
    return 0;
}

/*extern OPSTRUCTURE *instr00,*instr01,*instr04,*instr05,*instr06,
       *instr07,*instr08,*instr09,*instr11,*instr12,*instr13,*instr14;*/
OPSTRUCTURE *opmains[] =
{
    instr00,
    instr01,
    instr02,    /* Repeat 3 times for 3 move sizes */
    instr03,
    instr04,
    instr05,
    instr06,
    instr07,
    instr08,
    instr09,
    NULL,       /* No instr 010 */
    instr11,
    instr12,
    instr13,
    instr14,
    NULL
};

static int
get_asmcmd()
{
    /*extern OPSTRUCTURE syntax1[];
    extern COPROCSTRUCTURE syntax2[];*/
    register OPSTRUCTURE *optbl;
    extern int error;

    int opword;
    int h,
      j;
    int size;


    size = 0;
    Instruction.wcount = 0;
    opword = getnext_w (&Instruction);
    /* Make adjustments for this being the command word */
    Instruction.cmd_wrd = Instruction.code[0] & 0xffff;
    Instruction.wcount = 0; /* Set it back again */

    /* This may be temporary.  Opword %1111xxxxxxxxxxxx is available
     * for 68030-68040 CPU's, but we are not yet making this available
     */
    if ((opword & 0xf000) == 0xf000)
        return 0;
    
    if (!(optbl = opmains[(opword >> 12) & 0x0f]))
    {
        return 0;
    }

    for (j = 0; j <= MAXINST; j++)
    {
        OPSTRUCTURE *curop = &optbl[j];

        error = FALSE;

        if (!(curop->name))
            break;

        curop = tablematch (opword, curop);

        /* The table must be organized such that the "cpulvl" field
         * is sorted in ascending order.  Therefore, if a "cpulvl"
         * higher than "cpu" is encountered we can abort the search.
         */
        if (curop->cpulvl > cpu)
        {
            return 0;
        }

        if (!error)
        {
            if (curop->opfunc(&Instruction, curop->id, curop))
            {
                return 1;
            }
            else
            {
                if (ftell(ModFP) != CmdEnt + 2)
                {
                    fseek(ModFP, CmdEnt + 2, SEEK_SET);
                    PCPos = CmdEnt + 2;
                    initcmditems(&Instruction);
                }
            }
        }
    }

#if (DEVICE==68040 || COPROCESSOR==TRUE)
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

/* ******************************************************************** *
 * MovBytes() - Reads data for Data Boundary range from input file and  *
 *          places it onto the print buffer (and does any applicable    *
 *          printing if in pass 2).                                     *
 * ******************************************************************** */

void
#ifdef __STDC__
MovBytes (struct databndaries *db)
#else
MovBytes (db)
    struct databndaries *db;
#endif
{
    CMD_ITMS Ci;
    char tmps[20];
    int valu;
#ifdef _OSK
    static
#endif
    char *xFmt[3] = {"$%02x", "$%04x", "$%08x"};
    int cCount = 0,
        maxLst;

    CmdEnt = PCPos;

    /* This may be temporary, and we may set PBytSiz
     * to the appropriate value */
    strcpy (Ci.mnem, "dc");
    Ci.opcode[0] = '\0';
    Ci.lblname = "";
    Ci.comment = NULL;
    Ci.cmd_wrd = 0;
    PBytSiz = db->b_siz;

    switch (PBytSiz)
    {
    case 1:
        strcat (Ci.mnem, ".b");
        maxLst = 8;
        break;
    case 2:
        strcat (Ci.mnem, ".w");
        maxLst = 4;
        break;
    case 4:
        strcat (Ci.mnem, ".l");
        maxLst = 2;
        break;
    }

    while (PCPos <= db->b_hi)
    {
        /* Init dest buffer to null string for LblCalc concatenation */

        tmps[0] = '\0';

        switch (PBytSiz)
        {
        case 1:
            valu = fread_b(ModFP);
            break;
        case 2:
            valu = fread_w (ModFP);
            break;
        case 4:
            valu = fread_l (ModFP);
            break;
        }

        PCPos += db->b_siz;
        ++cCount;

        /*process_label (&Ci, AMode, valu);*/


        LblCalc (tmps, valu, AMode);

        if (Pass == 2)
        {
            /*char tmp[20];

            sprintf (tmp, xFmt[PBytSiz >> 1], valu);*/

            if (cCount < maxLst)
            {
                Ci.cmd_wrd =  ((Ci.cmd_wrd << (PBytSiz * 8)) | (valu & 0xff));
            }

            if (strlen(Ci.opcode))
            {
                strcat (Ci.opcode, ",");
            }

            strcat (Ci.opcode, tmps);

            /* If length of operand string is max, print a line */

            if ( (strlen (Ci.opcode) > 22) || findlbl ('L', PCPos))
            {
                PrintLine(pseudcmd, &Ci, 'L', CmdEnt, PCPos);
                Ci.opcode[0] = '\0';
                Ci.cmd_wrd = 0;
                Ci.lblname = NULL;
                CmdEnt = PCPos/* _ PBytSiz*/;
                cCount = 0;
            }
        }

        /*PCPos += PBytSiz;*/
    }

    /* Loop finished.. print any unprinted data */

    if ((Pass==2) && strlen (Ci.opcode))
    {
        PrintLine (pseudcmd, &Ci, 'L', CmdEnt, PCPos);
    }
}


/* ******************************************
 * MovAsc() - Move nb byes fordcb" statement
 */

void
#ifdef __STDC__
MovASC (int nb, char aclass)
#else
MovASC (nb, aclass)
    int nb;
    char aclass;
#endif
{
    char oper_tmp[30];
    CMD_ITMS Ci;
    int cCount = 0;

    strcpy (Ci.mnem, "dc.b");         /* Default mnemonic to "fcc" */
    CmdEnt = PCPos;
    *oper_tmp = '\0';
    Ci.cmd_wrd = 0;
    Ci.comment = NULL;
    Ci.lblname = "";

    while (nb--)
    {
        register int x;
        char c[6];

        if ((strlen (oper_tmp) > 24) ||
            (strlen (oper_tmp) && findlbl (aclass, PCPos)))
            /*(strlen (oper_tmp) && findlbl (ListRoot (aclass), PCPos + 1)))*/
        {
            sprintf (Ci.opcode, "\"%s\"", oper_tmp);
            PrintLine (pseudcmd, &Ci, 'L', CmdEnt, PCPos);
            oper_tmp[0] = '\0';
            CmdEnt = PCPos;
            Ci.lblname = "";
            Ci.cmd_wrd = 0;
            Ci.wcount = 0;
            cCount = 0;
        }

        x = fgetc (ModFP);
        ++cCount;
        ++PCPos;

        if (isprint (x) && (x != '"'))
        {
            if (Pass == 2)
            {
                /*if (strlen (pbuf->instr) < 12)
                {
                    sprintf (c, "%02x ", x);
                    strcat (pbuf->instr, c);
                }*/

                sprintf (c, "%c", x & 0x7f);
                strcat (oper_tmp, c);

                if (cCount < 16)
                {
                    Ci.cmd_wrd = (Ci.cmd_wrd << 8) | (x & 0xff);
                }
            }   /* end if (Pass2) */
        }
        else            /* then it's a control character */
        {
            if (Pass == 1)
            {
                if ((x & 0x7f) < 33)
                {
                    char lbl[10];
                    sprintf (lbl, "ASC%02x", x);
                    addlbl ('^', x & 0xff, lbl);
                }
            }
            else       /* Pass 2 */
            {
                /* Print any unprinted ASCII characters */
                if (strlen (oper_tmp))
                {
                    sprintf (Ci.opcode, "\"%s\"", oper_tmp);
                    PrintLine (pseudcmd, &Ci, aclass, CmdEnt, CmdEnt);
                    Ci.opcode[0] = '\0';
                    Ci.cmd_wrd = 0;
                    Ci.lblname = "";
                    oper_tmp[0] = '\0';
                    cCount = 0;
                    CmdEnt = PCPos - 1; /* We already have the byte */
                }

                if (isprint(x))
                {
                    sprintf (Ci.opcode, "'%c'", x);
                }
                else
                {
                    sprintf (Ci.opcode, "$%x", x);
                }

                Ci.cmd_wrd = x;
                PrintLine (pseudcmd, &Ci, aclass, CmdEnt, PCPos);
                Ci.lblname = "";
                Ci.opcode[0] = '\0';
                Ci.cmd_wrd = 0;
                cCount = 0;
                CmdEnt = PCPos;
            }
        }
    }       /* end while (nb--) - all chars moved */

    /* Finally clean up any remaining data. */
    if ((Pass == 2) && (strlen (oper_tmp)) )       /* Clear out any pending string */
    {
        sprintf (Ci.opcode, "\"%s\"", oper_tmp);
        /*list_print (&Ci, CmdEnt, NULL);*/
        PrintLine (pseudcmd, &Ci, 'L', CmdEnt, PCPos);
        Ci.lblname = "";
        *oper_tmp = '\0';
    }

    CmdEnt = PCPos;
}

/* ********************************* *
 * NsertBnds():	Insert boundary area *
 * ********************************* */

void
#ifdef __STDC__
NsrtBnds (struct databndaries *bp)
#else
NsrtBnds (bp)
    struct databndaries *bp;
#endif
{
    /*memset (pbuf, 0, sizeof (struct printbuf));*/
    AMode = 0;                  /* To prevent LblCalc from defining class */
    NowClass = bp->b_class;
    PBytSiz = 1;                /* Default to one byte length */

    switch (bp->b_typ)
    {
        case 1:                    /* Ascii */
            /* Bugfix?  Pc was bp->b_lo...  that setup allowed going past
             * the end if the lower bound was not right. */

            MovASC ((bp->b_hi) - PCPos + 1, 'L');
            break;                  /* bump PC  */
        case 6:                     /* Word */
            PBytSiz = 2;            /* Takes care of both Word & Long */
            MovBytes(bp);
            break;
        case 4:                     /* Long */
            PBytSiz = 4;            /* Takes care of both Word & Long */
            MovBytes(bp);
            break;
        case 2:                     /* Byte */
        case 5:                     /* Short */
            MovBytes (bp);
            break;
        case 3:                    /* "C"ode .. not implememted yet */
            break;
        default:
            break;
    }

    NowClass = 0;
}

#ifdef DoIsCmd
/* ******************************************************************** *
 * IsCmd() - Checks to see if code pointed to by p is valid code.       *
 *      On entry, we are poised at the first byte of prospective code.  *
 * Returns: pointer to valid lkuptable entry or 0 on fail               *
 * ******************************************************************** */

static struct lkuptbl *
IsCmd (int *fbyte, int *csiz)
{
    struct lkuptbl *T;          /* pointer to appropriate tbl   */
    register int sz;            /* # entries in this table      */
    int c = fgetc (progpath);

    *csiz = 2;

    switch (*fbyte = c)
    {
        case '\x10':
            T = Pre10;
            c = fgetc (progpath);
            *fbyte =(*fbyte <<8) + c;
            sz = sizeof (Pre10) / sizeof (Pre10[0]);
            break;
        case '\x11':
            T = Pre11;
            c = fgetc (progpath);
            *fbyte =(*fbyte <<8) + c;
            sz = sizeof (Pre11) / sizeof (Pre11[0]);
            break;
        default:
            *csiz = 1;
            T = Byte1;
            sz = sizeof (Byte1) / sizeof (struct lkuptbl);
            break;
    }

    while ((T->cod != c))
    {
        if (--sz == 0)
        {
            return 0;
        }

        ++T;
    }

    AMode = T->amode;

    return ((T->cod == c) && (T->t_cpu <= CpuTyp)) ? T : 0;
}

#endif           /* ifdef IsCmd */

