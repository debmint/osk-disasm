/* ******************************************************************* *
 * lblfuncs.c - Functions related to labels and amodes                 $
 *                                                                     $
 * $Id::                                                               $
 * ******************************************************************* */

#include "disglobs.h"
#include <string.h>
#include <ctype.h>
#include "userdef.h"
#include "proto.h"

#ifdef _OSK
/* This is to resolve a bug in stdlib.h */
#ifdef abs
#   undef abs
#endif
#define abs(a) ((a) < 0 ? -(a) : (a))
#endif

LBLCLAS LblList[] = {
    {'_', NULL},
    {'!', NULL}, {'=', NULL}, {'A', NULL}, {'B', NULL},
    {'C', NULL}, {'D', NULL}, {'E', NULL}, {'F', NULL},
    {'G', NULL}, {'H', NULL}, {'I', NULL}, {'J', NULL},
    {'K', NULL}, {'L', NULL}, {'M', NULL}, {'N', NULL},
    {'O', NULL}, {'P', NULL}, {'Q', NULL}, {'R', NULL},
    {'S', NULL}, {'T', NULL}, {'U', NULL}, {'V', NULL},
    {'W', NULL}, {'X', NULL}, {'Y', NULL}, {'Z', NULL},
    {0, NULL}
};

extern struct databndaries *LAdds[];
extern struct databndaries *dbounds;

/*
 * labelclass() - Returns the base entry for the desired class
 *
 */

LBLCLAS *
#ifdef __STDC__
labelclass (char lblclass)
#else
labelclass (lblclass)
    char lblclass;
#endif
{
    LBLCLAS *c = LblList;

    while (c->lclass != lblclass)
    {
        ++c;

        if (c->lclass == 0)
        {
            c = NULL;
            break;
        }
    }

    return c;
}

/* ------------------------------------------------------------ *
 * lblpos() - Searches the appropriate label list for a value   *
 * Passed:  (1) - The character representing the class          *
 *          (2) - The numeric value of that label               *
 *                                                              *
 * Returns: Pointer to the exact label if it has already been   *
 *          defined, else the label immediately preceding the   *
 *          location where it should be.                        *
 * ------------------------------------------------------------ */

static LBLDEF *
#ifdef __STDC__
lblpos (char lblclass, int lblval)
#else
lblpos (lblclass, lblval)
    char lblclass;
    int lblval;
#endif
{
    LBLDEF *me = labelclass (lblclass)->cEnt;

    if (me)
    {
        while ((me->Next) && (lblval >= (me->Next)->myaddr))
        {
            me = me->Next;
        }
    }

    return me;

}

/* ******************************************************************** *
 * movchr() - Append a char in the desired printable format onto dst    *
 * ******************************************************************** */

static void
#ifdef __STDC__
movchr (char *dst, unsigned char ch)
#else
movchr (dst, ch)
    char *dst;
    unsigned char ch;
#endif
{
    char mytmp[10];
    register LBLDEF *pp;

    if (isprint (ch & 0x7f) && ((ch & 0x7f) != ' '))
    {
        sprintf (mytmp, "'%c'", ch & 0x7f);
        strcat (dst, mytmp);
    }
    else
    {
        if (pp = findlbl ('^', ch))
        /*if ((pp = FindLbl (ListRoot ('^'), ch & 0x7f)))*/
        {
            strcat (dst, pp->sname);
        }
        else
        {
            sprintf (mytmp, "$%02x", ch & 0x7f);
            strcat (dst, mytmp);
        }
    }

    if (ch & 0x80)
    {
        strcat (dst, "+$80");
    }
}

/*
 * PrintLbl () - Prints out the label to "dest", in the format needed.
 * Passed : (1) dest - The string buffer into which to print the label.
 *          (2) clas - The Class Letter for the label.
 *          (3)  adr - The label's address.
 *          (4)   dl - ptr to the nlist tree for the label
 */

void
#ifdef __STDC__
PrintLbl (char *dest, char clas, int adr, LBLDEF *dl, int amod)
#else
PrintLbl (dest, clas, adr, dl, amod)
    char *dest;
    char clas;
    int adr;
    LBLDEF *dl;
    int amod;
#endif
{
    char tmp[10];
    short decn = adr & 0xffff;
    register int mask;

    /* Readjust class definition if necessary */

    if (clas == '@')
    {
         if (abs(adr) < 9)
        /*if ( (adr <= 9) ||
             ((PBytSiz == 1) && adr > 244) ||
             ((PBytSiz == 2) && adr > 65526) )*/
        {
            clas = '&';
        }
        else
        {
            clas = '$';
        }
    }

    switch (clas)
    {
        char *hexfmt;

        case '$':       /* Hexadecimal notation */
            switch (amod)
            {
                default:
                    switch (PBytSiz)
                    {
                    case 1:
                        adr &= 0xff;
                        break;
                    case 2:
                        adr &= 0xffff;
                        break;
                    default:
                        break;
                    }

                    if (abs(adr) <= 0xff)
                    {
                        hexfmt = "%02x";
                    }
                    else if (abs(adr) <= 0xffff)
                    {
                        hexfmt = "%04x";
                    }
                    else
                    {
                        hexfmt = "%x";
                    }

                    break;
                case AM_LONG:
                    hexfmt = "%08x";
                    break;
                case AM_SHORT:
                    hexfmt = "%04x";
                    break;
            }

            sprintf (tmp, hexfmt, adr);
            sprintf (dest, "$%s", tmp);
            break;
        case '&':       /* Decimal */
            sprintf (dest, "%d", adr);
            break;
        case '^':       /* ASCII */
            *dest = '\0';

            if (adr > 0xff)
            {
                movchr (dest, (adr >> 8) & 0xff);
                strcat (dest, "*256+");
            }

            movchr (dest, adr & 0xff);

            break;
        case '%':       /* Binary */
            strcpy (dest, "%");

            if (adr > 0xffff)
            {
                mask = 0x80000000;
            }
            else if (adr > 0xff)
            {
                mask = 0x8000;
            }
            else
            {
                mask = 0x80;
            }

            while (mask)
            {
                strcat (dest, (mask & adr ? "1" : "0"));
                mask >>= 1;
            }

            break;
        default:
            strcpy (dest, dl->sname);
    }
}

/* **************************************************************** *
 * ClasHere()	See if a Data boundary for this address is defined  *
 * Passed : (1) Pointer to Boundary Class list                      *
 *          (2) Address to check for                                *
 * Returns: Ptr to Boundary definition if found,  NULL if no match. *
 * **************************************************************** */

struct databndaries *
#ifdef __STDC__
ClasHere (struct databndaries *bp, int adrs)
#else
ClasHere (bp, adrs)
    struct databndaries *bp;
    int adrs;
#endif
{
    register struct databndaries *pt;
    register int h = (int) adrs;

    if ( ! (pt = bp))
    {
        return 0;
    }

    while (1)
    {
        if (h < pt->b_lo)
        {
            if (pt->DLess)
                pt = pt->DLess;
            else
                return 0;
        }
        else
        {
            if (h > pt->b_hi)
            {
                if (pt->DMore)
                {
                    pt = pt->DMore;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return pt;
            }
        }
    }
}

/*
 * findlbl() - Finds the label with the exact value 'lblval'
 * Passed:  (1) - The character class for the label
 *          (2) - The value for the address
 * Returns: The label def if it exists, NULL if not found
 *
 */

LBLDEF *
#ifdef __STDC__
findlbl (char lblclass, int lblval)
#else
findlbl (lblclass, lblval)
    char lblclass;
    int lblval;
#endif
{
    LBLDEF *found;

    if (strchr ("^@$&", lblclass))
        return NULL;

    found = lblpos (lblclass, lblval);

    if (found)
    {
        return (found->myaddr == lblval) ? found : NULL;
    }

    return NULL;
}

char *
#ifdef __STDC__
lblstr (char lblclass, int lblval)
#else
lblstr (lblclass, lblval)
    char lblclass;
    int lblval;
#endif
{
    LBLDEF *me = findlbl(lblclass, lblval);

    return (me ? me->sname : "");
}
/* ---------------------------------------------------------------- *
 * create_lbldef() - Creates a new label definition                 *
 * Passed:  (1) - value (the address or value of the label)         *
 *          (2) - the name of the label                             *
 * Returns: Pointer to the new label def, or NULL on failure to     *
 *          allocate memory for the label.                          *
 *          The name and address is already stored.                 *
 * ---------------------------------------------------------------- */

static LBLDEF *
#ifdef __STDC__
create_lbldef (char lblclass, int val, char *name)
#else
create_lbldef (lblclass, val, name)
    char lblclass;
    int val;
    char *name;

#endif
{
    register LBLDEF *newlbl;
    
    newlbl = (LBLDEF *)mem_alloc(sizeof(LBLDEF));
    memset(newlbl, 0, sizeof(LBLDEF));
    {
        if (name && strlen(name))
        {
            strcpy (newlbl->sname, name);
        }
        else
        {
            /* Assume that a program label does not exceed 20 bits */
            /*sprintf (newlbl->sname, "%c%05x", toupper(lblclass), val & 0x3ffff);*/
            sprintf (newlbl->sname, "%c%05x", toupper(lblclass), val);
        }

        newlbl->myaddr = val;
    }

    return newlbl;
}

/* ************************
 * addlbl() - Add a label to the list
 *        Does nothing for class '^', '@', '$', or '&'
 *        if the label exists, and a different name is provided, renames the label
 * Passed : (1) char label class
 *          (2) int the address for the label
 *          (3) char * - the name for the label 
 *              If NULL or empty string, the hex address of the label prepended with
 *              the class letter is used.
 */

LBLDEF *
#ifdef __STDC__
addlbl (char lblclass, int val, char *newname)
#else
addlbl (lblclass, val, newname)
    char lblclass;
    int val;
    char *newname;
#endif
{
    LBLDEF *oldlbl;
    LBLDEF *newlbl;

    if (strchr("^@$&", lblclass))
    {
        return NULL;
    }

    if ((oldlbl = lblpos (lblclass, val)))
    {
        register int maxsize = sizeof (oldlbl->sname);

        if ((newname) && (strlen(newname) >= maxsize))
        {
            newname[maxsize - 1] = '\0';
        }

        if (oldlbl->myaddr == val)   /* Simply a rename */
        {
            if ((newname) && strlen (newname))
            {
                if (strcmp(newname, oldlbl->sname))
                {
                    strcpy(oldlbl->sname, newname);
                }
            }

            return oldlbl;
        }
        else /* Creating a new label (with others existing) */
        {
            if (newlbl = create_lbldef(lblclass, val, newname))
            {
                /* New beginning entry ?  */
                if (val < labelclass(lblclass)->cEnt->myaddr)
                {
                    LBLCLAS *clas = labelclass(lblclass);

                    if (clas)
                    {
                        oldlbl = clas->cEnt;
                        clas->cEnt = newlbl;
                        oldlbl->Prev = newlbl;
                        newlbl->Next = oldlbl;
                    }
                    else
                    {
                        fprintf (stderr, "*** Label class '%c' not found\n",
                                lblclass);
                    }
                }
                else        /* Insert into chain */
                {
                    newlbl->Prev = oldlbl;

                    if (oldlbl->Next)
                    {
                        newlbl->Next = oldlbl->Next;
                        oldlbl->Next->Prev = newlbl;
                    }

                    oldlbl->Next = newlbl;
                }

                return newlbl;
            }
        }
    }
    else    /* first entry in this tree */
    {
        if (newlbl = create_lbldef(lblclass, val, newname))
        {
            LBLCLAS *clas = labelclass(lblclass);

            if (clas)
            {
                clas->cEnt = newlbl;
            }
            else
            {
                fprintf (stderr, "*** Label class '%c' not found\n",
                        lblclass);
            }

            return newlbl;
        }
    }

    return 0;
}

/*
 * process_label: Handle label depending upon Pass.  If Pass 1, add
 *      it as needed, if Pass 2, place values into the CMD_ITMS fields
 *
 */

void
#ifdef __STDC__
process_label (CMD_ITMS *ci, char lblclass, int addr)
#else
process_label (ci, lblclass, addr)
    CMD_ITMS *ci;
    char lblclass;
    int addr;
#endif
{
    if (Pass == 1)
    {
        addlbl (lblclass, addr, NULL);
    }
    else   /* Pass 2, find it */
    {
        register LBLDEF *me;

        if (me = findlbl(lblclass, addr))
        {
            strcpy (ci->opcode, me->sname);
        }
        else
        {
            fprintf (stderr, "*** phasing error ***\n");
            sprintf (ci->opcode, "L%05x", addr);
        }
    }
}

void
#ifdef __STDC__
parsetree(char c)
#else
parsetree(c)
    char c;
#endif
{
    LBLDEF *l;
    LBLCLAS *lc;
    int x;

    if (Pass == 1)
        return;

    lc = LblList;


    while (lc->lclass)
    {
        c = lc->lclass;

        if ((l = labelclass (c)->cEnt))
        {
            printf ("\nLabel definitions for Class '%c'\n\n", c);

            while (l->Next)
            {
                printf ("%s equ $%x\n", l->sname, l->myaddr);
                l = l->Next;
            }
        }

        ++lc;
    }
}

/*
 * LblCalc() - Calculate the Label for a location
 * Passed:  (1) dst - pointer to character string into which to APPEND result                                       *
 *          (2) adr -  the address of the label
 *          (3) amod - the AMode desired
 */

int
#ifdef __STDC__
LblCalc (char *dst, int adr, int amod, int curloc)
#else
LblCalc (dst, adr, amod, curloc)
    char *dst; int adr; int amod; int curloc;
#endif
{
    int raw = adr /*& 0xffff */ ;   /* Raw offset (postbyte) - was unsigned */
    char mainclass,                 /* Class for this location */
         oclass = 0;                /* Class for offset (if present) */

    struct databndaries *kls = 0;
    LBLDEF *mylabel = 0;

    if (amod == AM_REL)
    {
        raw += curloc;
    }

    if (IsROF)
    {
        if (IsRef(dst, curloc, adr))
        {
            return 1;
        }
    }

    /* if amod is non-zero, we're doing a label class */

    if (amod)
    {
        if ((kls = ClasHere (LAdds[amod], CmdEnt)))
        {
            mainclass = kls->b_typ;

            if (kls->dofst)     /* Offset ? */
            {
                oclass = (char) (kls->dofst->oclas_maj);

                if (kls->dofst->add_to)
                {
                    raw -= kls->dofst->of_maj;
                }
                else
                {
                    raw += kls->dofst->of_maj;
                }

                /* Let's attempt to insert the label for PC-Rel offets here */

                if (kls->dofst->incl_pc)
                {
                    raw += CmdEnt;
                    addlbl (kls->dofst->oclas_maj, raw, NULL);
                }
            }
        }
        else
        {
            /*mainclass = DEFAULTCLASS;*/
            mainclass = DfltLbls[amod - 1];
        }
    }
    else              /* amod=0, it's a boundary def  */
    {
        if (NowClass)
        {
            kls = ClasHere (dbounds, CmdEnt);
            mainclass = NowClass;

            if (kls->dofst)
            {
                oclass = kls->dofst->oclas_maj;

                if (kls->dofst->add_to)
                {
                    raw -= kls->dofst->of_maj;
                }
                else
                {
                    raw += kls->dofst->of_maj;
                }

                if (kls->dofst->incl_pc)
                {
                    raw += CmdEnt;
                }
            }
        }
        else
        {
            return 0;
        }
    }

    /* Attempt to restrict class 'L' */
    /*if (mainclass == 'L')
    {
        raw &= 0x3ffff;
    }*/

    if (Pass == 1)
    {
        addlbl (mainclass, raw, NULL);
    }
    else
    {                           /*Pass2 */
        char tmpname[20];

        if ((mylabel = findlbl (mainclass, raw)))
        {
            PrintLbl (tmpname, mainclass, raw, mylabel, amod);
            strcat (dst, tmpname);
        }
        else
        {                       /* Special case for these */
            if (strchr ("^$@&%", mainclass))
            {
                PrintLbl (tmpname, mainclass, raw, mylabel, amod);
                strcat (dst, tmpname);
            }
            else
            {
                char t;

                t = (mainclass ? mainclass : 'D');
                fprintf (stderr, "Lookup error on Pass 2 (main)\n");
                fprintf (stderr, "Cannot find %c - %05x\n", t, raw);
             /*   fprintf (stderr, "Cmd line thus far: %s\n", tmpname);*/
                exit (1);
            }
        }

        /* Now process offset, if any */

        if (kls && kls->dofst)
        {
            char c = kls->dofst->oclas_maj;

            if (kls->dofst->add_to)
            {
                strcat (dst, "+");
            }
            else
            {
                strcat (dst, "-");
            }

            if (kls->dofst->incl_pc)
            {
                strcat (dst, "*");

                if (kls->dofst->of_maj)
                {
                    strcat (dst, "-");
                }
                else
                {
                    return 1;
                }
            }
            if (mylabel = findlbl (c,  kls->dofst->of_maj))
            /*if ((mylabel = FindLbl (LblList[strpos (lblorder, c)],
                                    kls->dofst->of_maj)))*/
            {
                PrintLbl (tmpname, c, kls->dofst->of_maj, mylabel, amod);
                strcat (dst, tmpname);
            }
            else
            {                   /* Special case for these */
                if (strchr ("^$@&", c))
                {
                    PrintLbl (tmpname, c, kls->dofst->of_maj, mylabel, amod);
                    strcat (dst, tmpname);
                }
                else
                {
                    char t;

                    t = (c ? c : 'D');
                    fprintf (stderr, "Lookup error on Pass 2 (offset)\n");
                    fprintf (stderr, "Cannot find %c%x\n", t,
                             kls->dofst->of_maj);
                    /*fprintf (stderr, "Cmd line thus far: %s\n", tmpname);*/
                    exit (1);
                }
            }

        }
    }

    return 1;
}

