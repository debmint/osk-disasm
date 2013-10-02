
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

static int HdrLen;

extern struct databndaries *dbounds;

static int get_asmcmd(
#ifdef __STDC__
    void
#endif
);

#ifdef __STDC__
char *get_apcomment(char clas, int addr);
void PrintComment(char lblclass, int cmdlow, int cmdhi);
#else
char *get_apcomment ();
void PrintComment();
#endif

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

static void
psect_setup()
{
    register struct modnam *pt;
    char *ProgType = NULL;
    char *ProgLang = NULL;
    char *ProgAtts = NULL;
    /*char *StackAddL;*/
    char **prgsets[4];
    unsigned char hdrvals[3];
    int c;
    CMD_ITMS CmdIt;
    char *psecfld[2];

    prgsets[0] = &ProgType; prgsets[1] = &ProgLang;
    prgsets[2] = &ProgAtts; prgsets[3] = NULL;
    psecfld[0] = ",((%s << 8)"; psecfld[1] = " | %s)";
    ProgType = modnam_find (ModTyps, (unsigned char)M_Type)->name;
    hdrvals[0] = M_Type;
    ProgLang = modnam_find (ModLangs, (unsigned char)M_Lang)->name;
    hdrvals[1] = M_Lang;
    ProgAtts = modnam_find (ModAtts, (unsigned char)M_Attr)->name;
    hdrvals[2] = M_Attr;
    strcpy (CmdIt.mnem, "set");
#ifdef _WIN32
    strcpy (EaString, strrchr(ModFile, '\\') + 1);
#else
    strcpy (EaString, strrchr(ModFile, '/') + 1);
#endif
    strcat (EaString, "_a");

    for (c = 0; prgsets[c]; c++)
    {
        if (*prgsets[c])
        {
            CmdIt.cmd_wrd = hdrvals[c];
            sprintf (CmdIt.opcode, "%d", hdrvals[c]);
            /*strcpy(CmdIt.mnem, *prgsets[c]);*/
            list_print (&CmdIt, hdrvals[c], *prgsets[c]);
        }
        else {
            sprintf (CmdIt.mnem, "%d", hdrvals[c]);
        }

        sprintf (CmdIt.opcode, psecfld[c % 2], *prgsets[c]);
        strcat (EaString, CmdIt.opcode);
    }

    sprintf (&EaString[strlen(EaString)], "|%d)", M_Edit);
    strcat (EaString, ",0");    /* For the time being, don't add any stack */
    sprintf (&EaString[strlen(EaString)], ",%s", findlbl ('L', M_Exec)->sname);
    strcpy (CmdIt.opcode, EaString);
    strcpy (CmdIt.mnem, "psect");
    list_print (&CmdIt, M_Exec, NULL);
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

    if (Pass == 1)
    {
        if (!(ModFP = fopen(ModFile, BINREAD)))
        {
            errexit("Cannot open Module file for read");
        }

        PCPos = 0;
        get_modhead();
        PCPos = ftell(ModFP);
        process_label(&Instruction, 'L', M_Exec);
        
        /* Set Default Addressing Modes according to Module Type */
        switch (ModType)
        {
        case MT_PROGRAM:
            strcpy(DfltLbls, "&&&&&&D&&LL");
            break;
        case MT_DEVDRVR:
            break;
        }

        do_cmd_file();

    }
    else   /* Do Pass 2 Setup */
    {
        psect_setup();
    }

    /* NOTE: This is just a temporary kludge The begin _SHOULD_ be
             the first byte past the end of the header, but until
             we get bounds working, we'll do  this. */
    if (fseek(ModFP, HdrEnd, SEEK_SET) != 0)
    {
        errexit("FIle Seek Error");
    }

    PCPos = HdrEnd;

    while (PCPos < M_Size - 3)   /* Temporarily ignore the CRC */
    {
        struct databndaries *bp;

        Instruction.comment = NULL;
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

                PrintComment('L', CmdEnt, PCPos);
                Instruction.comment = get_apcomment ('L', CmdEnt);
                list_print (&Instruction, CmdEnt, NULL);

                if (Instruction.wcount)
                {
                    int count;
                    printf("%6s", "");

                    for (count = 0; count < Instruction.wcount; count++)
                    {
                       printf("%04x ", Instruction.code[count] & 0xffff);
                    }

                    printf("\n");
                }
            }
        }
        else
        {
            if (Pass == 2)
            {
                strcpy (Instruction.mnem, "ds.w");
                sprintf (Instruction.opcode, "$%x", Instruction.cmd_wrd & 0xffff);
                /* To do the following, we need to set it up in Pass 1 */
                /*process_label (&Instruction, 'L', CmdEnt);*/
                list_print (&Instruction, CmdEnt, NULL);
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
    instr01,    /* Repeat 3 times for 3 move sizes */
    instr01,
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
    Instruction.cmd_wrd = Instruction.code[0];
    Instruction.wcount = 0; /* Set it back again */
    
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

        if (!error)
        {
            if (curop->opfunc(&Instruction, curop->id, curop))
            {
                return 1;
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

char
#ifdef __STDC__
getnext_b(CMD_ITMS *ci)
#else
getnext_b(ci)
    CMD_ITMS *ci;
#endif
{
    char b;

    if (fread(&b, 1, 1, ModFP) < 1)
    {
        filereadexit();
    }

    ++PCPos;
    /* We won't store this into the buffers
     * as it is not a command */
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


/* ******************************************************************** *
 * MovBytes() - Reads data for Data Boundary range from input file and  *
 *          places it onto the print buffer (and does any applicable    *
 *          printing if in pass 2).                                     *
 * ******************************************************************** */

static void
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
                Ci.cmd_wrd =  ((Ci.cmd_wrd << (PBytSiz * 8)) | valu);
            }

            if (strlen(Ci.opcode))
            {
                strcat (Ci.opcode, ",");
            }

            strcat (Ci.opcode, tmps);

            /* If length of operand string is max, print a line */

            if ( (strlen (Ci.opcode) > 22) || findlbl ('L', PCPos))
            {
                list_print(&Ci, CmdEnt, NULL);
                Ci.opcode[0] = '\0';
                Ci.cmd_wrd = 0;
                CmdEnt = PCPos/* _ PBytSiz*/;
                cCount = 0;
            }
        }

        /*PCPos += PBytSiz;*/
    }

    /* Loop finished.. print any unprinted data */

    if ((Pass==2) && strlen (Ci.opcode))
    {
        list_print (&Ci, (short)CmdEnt, NULL);
        /*PrintLine (pseudcmd, pbuf, 'L', CmdEnt, PCPos);*/
    }
}

/* ********************************************************* *
 * AddDelims() - Add delimiters for fcc/fcs operand -        *
 *              checks string for nonexistand delimiter      *
 *              and copies string with delims to destination *
 * ********************************************************* */

static void
#ifdef __STDC__
AddDelims (char *dest, char *src)
#else
AddDelims (dest, src)
    char *dest;
    char *src;
#endif
{
    char delim = '"';
    static char bestdelims[] = "\"'/#\\|$!";
    char *dref = bestdelims;

    /* First, try to use some "preferred" delimiters */

    while (strchr (src, *dref))
    {
        ++dref;

        if (*dref == '\0')
        {
            break;
        }
    }

    delim = *dref;

    if (delim == '\0')
    {
        /* OK.. we didn't find a delim in the above..  Now let's
         * start off basically where we left off and parse through
         * the whole ASCII set to find one
         */

        delim = '\x25';

        while ( ! strchr(src, delim))
        {
            ++delim;

            /* This should never happen, but just in case */

            if (delim == '\x7f')
            {
                fprintf (stderr,
                         "Error, string contains ALL ASCII Characters???\n");
                exit(1);
            }
        }
    }

    sprintf (dest, "%c%s%c", delim, src, delim);
}

/* ************************************************** *
 * MovAsc() - Move nb byes int fcc (or fcs) statement *
 * ************************************************** */

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

    /*memset (pbuf, 0, sizeof (pbuf));*/
    strcpy (Ci.mnem, "dc.b");         /* Default mnemonic to "fcc" */
    CmdEnt = PCPos;

    *oper_tmp = '\0';
    Ci.cmd_wrd = 0;
    Ci.comment = NULL;
    /*Ci.opcode[0] = '\0';*/

    while (nb--)
    {
        register int x;
        char c[6];

        x = fgetc (ModFP);
        ++cCount;

        if (isprint (x))
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

                /*if ((x & 0x80))
                //{
                //    strcpy (pbuf->mnem, "fcs");
                //    AddDelims (pbuf->operand, oper_tmp);
                //    PrintLine (pseudcmd, pbuf, aclass, CmdEnt, PCPos);
                //    *oper_tmp = '\0';
                //    CmdEnt = PCPos + 1;
                //    strcpy (pbuf->mnem, "fcc");
                //}*/

                if ((strlen (oper_tmp) > 24) ||
                    (strlen (oper_tmp) && findlbl (aclass, x)))
                    /*(strlen (oper_tmp) && findlbl (ListRoot (aclass), PCPos + 1)))*/
                {
                    AddDelims (Ci.opcode, oper_tmp);
                    list_print (&Ci, CmdEnt, NULL);
                    /*PrintLine (pseudcmd, pbuf, aclass, CmdEnt, PCPos);*/
                    oper_tmp[0] = '\0';
                    CmdEnt = PCPos + 1;
                    Ci.cmd_wrd = 0;
                    /*strcpy (pbuf->mnem, "fcc");*/
                }
            }   /* end if (Pass2) */
        }
        else            /* then it's a control character */
        {
            if ((Pass == 2) && (strlen (oper_tmp)))
            {
                AddDelims (Ci.opcode, oper_tmp);
                list_print (&Ci, CmdEnt, NULL);
                Ci.opcode[0] = '\0';
                /*PrintLine (pseudcmd, pbuf, aclass, CmdEnt, PCPos);*/
                oper_tmp[0] = '\0';
                CmdEnt = PCPos;
            }

            if (Pass == 1)
            {
                if ((x & 0x7f) < 33)
                {
                    addlbl (aclass, x & 0xff, "");
                }
            }
            else
            {
                /* a dummy ptr to pass to Printlbl() to satify prototypes */
                struct nlist *nlp;

                /* do the following to keep gcc quiet about uninitialized
                 * variable.. If we're wrong, we'll get a segfault letting
                 * us know of our mistake.. */

                nlp = NULL;

                /*strcpy (pbuf->mnem, "fcb");*/
                sprintf (Ci.opcode, "%d", x);
                list_print (&Ci, CmdEnt, NULL);
                Ci.opcode[0] = '\0';
                /*PrintLbl (Ci.opcode, '^', x, nlp);
                //sprintf (pbuf->instr, "%02x", x & 0xff);
                //list_print (&Ci, CmdEnt, NULL);
                //PrintLine (pseudcmd, pbuf, aclass, CmdEnt, PCPos);
                //strcpy (pbuf->mnem, "fcc");*/
            }

            /*CmdEnt = PCPos + 2;*/
        }

        ++PCPos;
    }       /* end while (nb--) - all chars moved */

    /* Finally clean up any remaining data. */
    if ((Pass == 2) && (strlen (Ci.opcode)) )       /* Clear out any pending string */
    {
        AddDelims (Ci.opcode, oper_tmp);
        list_print (&Ci, CmdEnt, NULL);
        /*PrintLine (pseudcmd, pbuf, aclass, CmdEnt, PCPos);*/
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

/* ******************************************************** *
 * get_comment() - Checks for append comment for current    *
 *              command line.                               *
 * Passed: (1) class,                                       *
 *         (2) entry address for command                    *
 * Returns: ptr to comment string if present                *
 *          ptr to empty string if none                     *
 * ******************************************************** */

static char *
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


/* print any comments appropriate */

static void
#ifdef __STDC__
PrintComment(char lblclass, int cmdlow, int cmdhi)
#else
PrintComment(char lblclass, int cmdlow, int cmdhi)
    char lblclass;
    int cmdlow,
        cmdhi;
#endif
{
    register struct commenttree *me;
    register int x;

    for (x = cmdlow; x < cmdhi; x++)
    {
        me = Comments[strpos (lblorder, lblclass)];

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

