/* ******************************************************************* *
 * lblfuncs.c - Functions related to labels and amodes                 $
 *                                                                     $
 * $Id::                                                               $
 * ******************************************************************* */

#include "disglobs.h"
#include <string.h>
#include <ctype.h>


LBLCLAS LblList[] = {
    {'!', NULL}, {'^', NULL}, {'A', NULL}, {'B', NULL},
    {'C', NULL}, {'D', NULL}, {'E', NULL}, {'F', NULL},
    {'G', NULL}, {'H', NULL}, {'I', NULL}, {'J', NULL},
    {'K', NULL}, {'L', NULL}, {'M', NULL}, {'N', NULL},
    {'O', NULL}, {'P', NULL}, {'Q', NULL}, {'R', NULL},
    {'S', NULL}, {'T', NULL}, {'U', NULL}, {'V', NULL},
    {'W', NULL}, {'X', NULL}, {'Y', NULL}, {'Z', NULL},
    {0, NULL}
};

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
    LBLDEF *found = lblpos (lblclass, lblval);

    return (found->myaddr == lblval) ? found : NULL;
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
    
    if (newlbl = calloc(1, sizeof(LBLDEF)))
    {
        if (name && strlen(name))
        {
            strcpy (newlbl->sname, name);
        }
        else
        {
            sprintf (newlbl->sname, "%c%05x", toupper(lblclass), val);
        }

        newlbl->myaddr = val;
    }
    else
    {
        fprintf (stderr,
                "*** failed to allocat memory for label definition\n");
        return 0;
    }

    return newlbl;
}

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
    LBLDEF *oldlbl = lblpos (lblclass, val);
    LBLDEF *newlbl;

    if (oldlbl)
    {
        register int maxsize = sizeof (oldlbl->sname);

        if ((newname) && (strlen(newname) >= maxsize))
        {
            newname[maxsize - 1] = '\0';
        }

        if (oldlbl->myaddr == val)   /* Simply a rename */
        {
            if (newname)
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
    else
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
    int x;

    if (Pass == 1)
        return;

    for (x = 0; x < sizeof(LblList)/sizeof(LblList[0]); x++)
    {
        l = LblList[x].cEnt;

        if (l)
        {
            printf ("\nLabel definitions for Class '%c'\n\n", c);

            while (l->Next)
            {
                printf ("%s equ $%x\n", l->sname, l->myaddr);
                l = l->Next;
            }
        }
    }
}
