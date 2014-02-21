
/* ******************************************************************** *
 * rof.c - handles rof files                                            $
 *                                                                      $
 * $Id::                                                                $
 * ******************************************************************** */


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "disglobs.h"
#include "userdef.h"
#define ROF_C
#include "rof.h"
#include "proto.h"

#ifdef __STDC__
static struct rof_extrn *DataDoBlock (struct rof_extrn *mylist, int datasize, struct asc_data *, char cclass);
static void ROFDataLst (struct rof_extrn *mylist, int maxcount, struct asc_data *ascdat, char cclass);
static void get_refs(char *vname, int count, int ref_typ);
#else
struct rof_extrn *DataDoBlock ();
void ROFDataLst ();
void get_refs();
#endif

char  rname[100];

/*int code_begin;*/

extern int CodeEnd,
           PrevEnt;
extern char realcmd[],
            pseudcmd[];

/* DEBUGGING function */
void
#ifdef __STDC__
reflst (void)
#else
reflst ()
    struct asc_data *cl;
#endif
{
    struct rof_extrn * meme = refs_code;

    while (meme)
    {
        fprintf (stderr, "%05x : %-20s (%s)\n", meme->Ofst, 
            meme->Extrn ? meme->EName.nam : meme->EName.lbl->sname, meme->Extrn ? "Extern" : "Local");
        meme = meme->ENext;
    }
    /*if (cl->LNext)
    {
        reflst (cl->LNext);
    }

    printf ("   >>>   %04x    %d\n",cl->start,cl->length);

    if (cl->RNext)
    {
        reflst (cl->RNext);
    }*/
}

/* **************************************************************** *
 * RealPos() - Returns the correct file position.  Returns CmdEnt   *
 *             Non-ROF files, CmdEnt Offset by Code Entry Point     *
 * **************************************************************** */

int
#ifdef __STDC__
RealEnt(void)
#else
RealEnt()
#endif
{
    return IsROF ? (CmdEnt + HdrEnd) : CmdEnt;
}

/* ************************************************************************ *
 * AddInitLbls() - Add the initialization data to the Labels Ref            *
 *          On entry, the file pointer is positioned to the begin of the    *
 *          initialized data list for this particular vsect.                *
 * ************************************************************************ */

void
#ifdef __STDC__
AddInitLbls (struct rof_extrn *tbl, int dataSiz, char klas)
#else
AddInitLbls (tbl, dataSiz, klas);
    struct rof_extrn *tbl, int dataSiz; char klas;
#endif
{
    /*int count = fread_w(ModFP);*/
    char *dataBuf,
         *ptr;
    int c = 1;

    if (tbl)
    {
        register int refVal;

        do {
            if (!tbl->Extrn)
            {
                switch (REFSIZ(tbl->Type))
                {
                case 1: /*SIZ_BYTE:*/
                    refVal = fgetc(ModFP);
                    --dataSiz;
                    break;
                case 2: /*SIZ_WORD:*/
                    refVal = 0;
                    refVal = fread_w(ModFP);
                    dataSiz -= 2;
                    break;
                default:
                    refVal = fread_l(ModFP);
                    dataSiz -= 4;
                }

                tbl->EName.lbl = addlbl(tbl->dstClass, refVal, "");
            }

            tbl = tbl->ENext;
        } while (tbl);
    }
}

/* ************************************************** *
 * rofhdr() - read and interpret rof header           *
 * ************************************************** */

void
#ifdef __STDC__
getRofHdr (FILE *progpath)
#else
rofhdr()
    FILE *progpath;
#endif
{
    int glbl_cnt,
        ext_count,
        count;          /* Generic counter */
    int local_count;

    IsROF = TRUE;    /* Flag that module is an ROF module */
    fseek(progpath, 0, SEEK_SET);   /* Start all over */

    /* get header data */

        //ROFHd.sync = (M_ID << 16) | (fread_w(ModFP) & 0xffff);
        ROFHd.sync = fread_l(ModFP);
        
        if (ROFHd.sync != 0xdeadface)
        {
            errexit ("Illegal ROF Module sync bytes");
        }

        ROFHd.ty_lan = fread_w(ModFP);
        ROFHd.att_rev = fread_w (ModFP);       /* Attribute/Revision word */
        ROFHd.valid = fread_w (ModFP);         /* Nonzero if valid */
        ROFHd.series = fread_w (ModFP);        /* Assembler version used to compile */
        fread(ROFHd.rdate, sizeof(ROFHd.rdate), 1, ModFP);
        ROFHd.edition = fread_w(ModFP);;
        ROFHd.statstorage = fread_l (ModFP);   /* Size of static variable storage */
        ROFHd.idatsz = fread_l (ModFP);        /* Size of initialized data */
        ROFHd.codsz = fread_l (ModFP);         /* Size of the object code  */
        ROFHd.stksz = fread_l (ModFP);         /* Size of stack required   */
        ROFHd.code_begin = fread_l (ModFP);    /* Offset to entry point of object code   */
        ROFHd.utrap = fread_l (ModFP);         /* Offset to unitialized trap entry point */
        ROFHd.remotestatsiz = fread_l (ModFP); /* Size of remote static storage   */
        ROFHd.remoteidatsiz = fread_l (ModFP); /* Size of remote initialized data */
        ROFHd.debugsiz = fread_l (ModFP);      /* Size of the debug   */

        ROFHd.rname = freadString();

    /* Set ModData to an unreasonable high number so ListData
     * won't do it's thing...
     */

    M_Mem = 0x10000;
    /*ModData = 0x7fff;*/

    /* ************************************************ *
     * Get the Global definitions                       *
     * ************************************************ */
    count = glbl_cnt = fread_w (progpath);

    while (count--)
    {
        char *name;
        LBLDEF *me;
        int adrs;
        int typ;
        char klas;

        name = freadString();
        typ = fread_w (progpath);
        adrs = fread_l (progpath);

        if (me = addlbl(rof_class(typ, REFGLBL), adrs, name))
        {
            me->global = 1;
        }
    }

    /* Code section... read, or save file position   */
    HdrEnd = ftell (progpath);
    CodeEnd = ROFHd.codsz;
    /*idp_begin = code_begin + rofptr->codsz;
    indp_begin = idp_begin + rofptr->idpsz;*/

    if (fseek (progpath, ROFHd.codsz, SEEK_CUR) == -1)
    {
        fprintf (stderr, "rofhdr(): Seek error on module\n");
        exit (errno);
    }

    /* ********************************** *
     *    Initialized data Section        *
     * ********************************** */

    IDataCount = fread_w(ModFP);
    IDataBegin = ftell(ModFP);

    /* ********************************** *
     *    External References Section     *
     * ********************************** */

    if (fseek (ModFP, IDataBegin + ROFHd.idatsz + ROFHd.remotestatsiz + ROFHd.debugsiz - 2, SEEK_SET) == -1)
    {
        fprintf (stderr, "rofhdr(): Seek error on module\n");
        exit (errno);
    }

    ext_count = fread_w (ModFP);

    while (ext_count--)
    {
        char *_name;
        int refcount;

        _name = freadString();
        refcount = fread_w (ModFP);

        /* Get the individual occurrences for this name */

        get_refs (_name, refcount, REFXTRN);
    }


    /* *************************** *
     *    Local variables...       *
     * *************************** */

    local_count = fread_w (ModFP);

    get_refs("", local_count, REFLOCAL);

    /* Now we need to add labels for these refs */

    /* common block variables... */
    /* Do this after everything else is done */
    /* NOTE: We may need to save current ftell() to restore it after this */

    if (fseek(ModFP, IDataBegin - 2, SEEK_SET) == -1)
    {
        errexit ("RofLoadInitData() : Failed to seek to begin of Init Data");
    }

    AddInitLbls (refs_idata, ROFHd.idatsz, '_');
    AddInitLbls (refs_iremote, ROFHd.idatsz, 'H');

    /* Now we're ready to disassemble the code */

    /* Position to begin of Code section */
    fseek (progpath, HdrEnd, SEEK_SET);
    PCPos = 0;

   /* rofdis();*/
}

void
#ifdef __STDC__
    RofLoadInitData (void)
#else
    ROFLoadInitData()
#endif
{
    int ext_count;
    int local_count;

    /* ********************************** *
     * Initialized data section           *
     * ********************************** */

    /* ********************************** *
     *   Initialized remote data Section  *
     * ********************************** */

    /* ********************************** *
     *     Debug Information Section      *
     * ********************************** */

}

/* ****************************************************** *
 * rof_class () - returns the Destination reference for   *
 *          the reference                                 *
 * Passed: The Type byte from the reference               *
 * Returns: The Class Letter for the entry                *
 * ****************************************************** */

char
#ifdef __STDC__
rof_class (int typ, int refTy)
#else
rof_class (typ)
    int typ; int refTy
#endif
{
    int cclass;

    /* We'll tie up additional classes for data/bss as follows
     * D for data
     * _ for init data
     * G for remote
     * H for remote init
     *
     */

    switch (refTy)
    {
    case REFGLBL:
        switch (typ & 0x100)
        {
        case 0:         /* NOT common */
            switch (typ & 0x04)        /* Docs are backward? */
            {
            case 0:         /* Data */
                switch (typ & 0x01)  /* Docs are backward ? */
                {
                case 0:         /* Uninit */
                    switch (typ & 0x02)
                    {
                    case 0:
                        return 'D';
                    default:
                        return 'G';
                    }
                default:    /* Init */
                    switch (typ & 0x02)
                    {
                    case 0:
                        return '_';
                    default:
                        return 'H';
                    }
                }
            default: /* Code or Equ */
                switch (typ & 0x02)
                {
                case 0:     /* NOT remote */
                    return 'L';
                default:
                    return 'L'; /* FIXME: This is actually 'equ' */
                }
            }
        default:        /* common */
            switch (typ & 0x20)
            {
            case 0:     /* NOT Remote */
                /* These are WRONG! but for now, we'll use them */
                return '_';
            default:
                return 'D';
            }
        }

        break;

    case REFXTRN:
    case REFLOCAL:
        switch (typ & 0x200)
        {
        case 0: /* NOT remote */
            switch (typ & 0x20)
            {
            case 0:     /* data */
                return '_';
            default:    /* debug */
                return 'L';
            }
        default:    /* remote */
            switch (typ & 0x20)
            {
            case 0:
                return '_';
            default:      /* debug */
                break;
            }
            return 'H';
        }
    }

    return 'L'; /* Should never get to here, but for safety's sake */
}

/* ************************************************** *
 * rof_addlbl() - Adds a label to the nlist tree if   *
 *                applicable                          *
 *                Copies rof name to nlist name if    *
 *                different
 * Passed: adrs - address of label                    *
 *         ref  - reference structure                 *
 * ************************************************** */

//void
//#ifdef __STDC__
//rof_addlbl (int adrs, struct rof_extrn *ref)
//#else
//rof_addlbl (adrs, ref)
//    int adrs; struct rof_extrn *ref;
//#endif
//{
//    LBLDEF *nl;
//
//    /* The following may be a kludge.  The problem is that Relative
//     * external references get added to class C.
//     * Hopefully, no external references are needed in the label ref
//     * tables.  We'll try this to see...
//     */
//
//    if (ref->Extrn)
//    {
//        return;
//    }
//
//    if ((nl = addlbl (adrs, rof_class (ref->Type, 1), NULL)))
//    {
//        if (strlen (ref->EName))
//        {
//            if (strcmp (nl->sname, ref->EName))
//            {
//                strcpy (nl->sname, ref->EName);
//            }
//        }
//    }
//}

/* ************************************************************** *
 * get_refs() - get entries for given reference,                  *
 *            either external or local.                           *
 * Passed:  name (or blank for locals)                            *
 *          count - number of entries to process                  *
 *          1 if external, 0 if local                             *
 * ************************************************************** */

static void
#ifdef __STDC__
get_refs(char *vname, int count, int ref_typ)
#else
get_refs(vname, count, ref_typ)
    char *vname; int count; int refType, int ref_typ;
#endif
{
    struct rof_extrn *prevRef = NULL;
    char myClass;
    unsigned int _ty;
    int _ofst;
    struct rof_extrn *new_ref,
                        *curRef,
                    **base = 0;

    while (count--)
    {
        _ty = fread_w (ModFP);
        _ofst = fread_l (ModFP);

        /* Add to externs table */
        switch (myClass = rof_class(_ty, ref_typ))
        {
        case 'L':
            base = &refs_code;
            break;
        case 'D':
            base = &refs_data;
            break;
        case '_':
            base = &refs_idata;
            break;
        case 'G':
            base = &refs_remote;
            break;
        case 'H':
            base = &refs_iremote;
            break;
        default:
            myClass = 'L';  /* Possibly cause erroneous result, but to avoid crash */
            base = &refs_code;
        }

        if ((ref_typ == REFLOCAL) && (myClass == 'L'))
        {
        }
        else
        {
            new_ref = (struct rof_extrn *)mem_alloc(sizeof(struct rof_extrn));
            memset (new_ref, 0, sizeof(struct rof_extrn));

            new_ref->Type = _ty;
            new_ref->Ofst = _ofst;
            new_ref->Extrn = (ref_typ == REFXTRN);

            if (ref_typ == REFLOCAL)
            {
                new_ref->dstClass = rof_class(_ty, REFGLBL);
            }

            if (prevRef)
            {
                prevRef->MyNext = new_ref;
            }

            /*base = &refs_data;*/  /* For the time being, let's try to just use one list for all refs */

            /* If this tree has not yet been initialized, simply set the
             * base pointer to this entry (as the first)
             */

            if ( ! *base)
            {
                *base = new_ref;
            }

            /* If we get here, this particular tree has alreay been started,
             * so find where to put the new entry.  Note, for starters, let's
             * assume that each entry will be unique, that is, this location
             * won't be here
             */

            else        /* We have entries, insert it into the proper place */
            {
                curRef = *base;
                /*extrns = *base;*/     /* Use the global externs pointer */

                if (_ofst < curRef->Ofst)
                {
                    new_ref->ENext = curRef;
                    curRef->EUp = new_ref;
                    *base = new_ref;
                }
                else
                {
                    while ((curRef->ENext) && (_ofst > curRef->ENext->Ofst))
                    {
                        curRef = curRef->ENext;
                    }

                    /*if (curRef->Ofst > _ofst)
                    {
                        curRef = curRef->EUp;
                    }*/

                    if (curRef->ENext)
                    {
                        curRef->ENext->EUp = new_ref;
                    }

                    new_ref->ENext = curRef->ENext;
                    new_ref->EUp = curRef;
                    curRef->ENext = new_ref;
                }

            }       /* *base != NULL */

            prevRef = new_ref;

            if (new_ref->Extrn)
            {
                new_ref->EName.nam = vname;
            }
        }       /* end "if (ref_typ == REFXTRN)" */
    }           /* end "while (count--) */
}

/* ************************************************** *
 * find_extrn() - find an external reference          *
 * Passed : (1) xtrn - starting extrn ref             *
 *          (2) adrs - Address to match               *
 * ************************************************** */

struct rof_extrn *
#ifdef __STDC__
find_extrn ( struct rof_extrn *xtrn, int adrs)
#else
find_extrn (xtrn, adrs)
    struct rof_extrn *xtrn; int adrs;
#endif
{
    int found = 0;

    if (!xtrn)
    {
        return 0;
    }

    while ((adrs < xtrn->Ofst) && (xtrn->ENext))
    {
        xtrn = xtrn->ENext;
    }

    return (xtrn->Ofst == adrs ? xtrn : NULL);
}

/* ******************************************************** *
 * Returns the size defined in the type                     *
 * ******************************************************** */

int
#ifdef __STDC__
typeFetchSize (int rtype)
#else
typeFetchSize (rtype)
    int rtype;
#endif
{
    return (rtype >> 3) & 3;
}

/* ************************************************************ *
 * rof_lblref() - Process a label reference found in the code.  *
 *       On entry, Pc points to the begin of the reference      *
 * Passed: pointer to int variable to store value of operand    *
 *         value                                                *
 * Returns: pointer to the rof_extern entry, with a label name  *
 *       added, if applicable                                   *
 * ************************************************************ */

struct rof_extrn *
#ifdef __STDC__
rof_lblref (CMD_ITMS *ci, int *value)
#else
rof_lblref (ci, value)
    CMD_ITMS *ci;
    int *value;
#endif
{
    struct rof_extrn *thisref;
    register char *refFmt;

    if ( ! (thisref = find_extrn (refs_code, PCPos)))
    {
        return 0;
    }

    /**value = getc (progpath);
    ++Pc;*/

    switch (typeFetchSize(thisref->Type))
    {
    case SIZ_BYTE:
        *value = getc(ModFP);
        refFmt = "%c%02x";
        ++PCPos;
        break;
    case SIZ_WORD:
        *value = getnext_w(ci);
        refFmt = "%c%04x";
        break;
    case SIZ_LONG:
        *value = (getnext_w(ci) & 0xffff) << 16;
        *value |= getnext_w(ci) & 0xffff;
        refFmt = (*value > 0xffff) ? "%c%04x" : "%c%08x";
        break;
    }

    if ((Pass == 2)  && (strlen(thisref->EName.nam) == 0))
    {
        sprintf (thisref->EName.nam, refFmt, thisref->Type, *value);
    }

    return thisref;
}

/* ******************************************************** *
 * rof_find_asc() - Find an ascii data block def            *
 * Passed : (1) tree - ptr to asc_dat tree                  *
 *          (2) entry - Command entry point (usually CmdEnt *
 * Returns: tree entry if present, 0 if no match            *
 * ******************************************************** */

static struct asc_data *
#ifdef __STDC__
rof_find_asc (struct asc_data *tree, int entry)
#else
rof_find_asc (tree, entry)
    struct asc_data *tree; int entry;
#endif
{
    if (!tree)
    {
        return 0;
    }

    while (1)
    {
        if (entry < tree->start)
        {
            if (tree->LNext)
            {
                tree = tree->LNext;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            if (entry > tree->start)
            {
                if (tree->RNext)
                {
                    tree = tree->RNext;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return tree;
            }
        }
    }
}

/* ******************************************************** *
 * rof_datasize() - returns the end of rof data area        *
 * Passed: Label Class letter to search                     *
 * Returns: size of this data area                          *
 *          If not a data area, returns 0                   *
 * ******************************************************** */

int
#ifdef __STDC__
rof_datasize (char cclass)
#else
rof_datasize (cclass)
char cclass;
#endif
{
    int dsize;

    switch (cclass)
    {
        case 'D':
            dsize = ROFHd.statstorage;
            break;
        case 'H':
            dsize = ROFHd.remoteidatsiz;  
            break;
        case 'G':
            dsize = ROFHd.remotestatsiz;
            break;
        case '_':
            dsize = ROFHd.idatsz;
            break;
        default:
            dsize = 0;
    }

    return dsize;
}

/* ******************************************************************** *
 * DataDoBlock - Process a block composed of an initialized reference   *
 *               from a data area                                       *
 * Passed : (1) struct rof_extrn *mylist - pointer to tree element      *
 *          (2) int datasize - the size of the area to process          *
 *          (3) char class - the label class (D or C)                   *
 * ******************************************************************** */

static struct rof_extrn *
#ifdef __STDC__
DataDoBlock (struct rof_extrn *mylist, int datasize,
             struct asc_data *ascdat, char cclass)
#else
DataDoBlock (mylist, int datasize, ascdat, cclass)
    struct rof_extrn *mylist; int datasize;
             struct asc_data *ascdat, char cclass;
#endif
{
    struct rof_extrn *srch;
    LBLDEF *nl;
    CMD_ITMS Ci;

    memset (&Ci, 0, sizeof(Ci));

    while (datasize > 0)
    {
        int bump = 2,
            my_val;

        /* Insert Label if applicable */

        if (nl = findlbl(cclass, CmdEnt))
        {
            strcpy (Ci.lblname, nl->sname);

            if (nl->global)
            {
                strcat (Ci.lblname, ":");
            }
        }

        /* First check that mylist is not null. If this vsect has no
         * references, 'mylist' will be null
         */

        if ( mylist && (srch = find_extrn (mylist, CmdEnt)) )
        {
            strcpy (Ci.mnem, "dc.");

            switch ((srch->Type >> 3) & 3)
            {
            case SIZ_BYTE:
                strcat (Ci.mnem, "b");
                my_val = fgetc(ModFP);
                bump = 1;
                break;
            case SIZ_WORD:
                strcat (Ci.mnem, "w");
                my_val = fread_w(ModFP);
                bump = 2;
                break;
            case SIZ_LONG:
                strcat (Ci.mnem, "l");
                my_val = fread_l (ModFP);
                bump = 4;
            }

            switch (mylist->Type)
            {
            case REFGLBL:
                strcpy(Ci.opcode, mylist->EName.nam);
                break;
            case REFLOCAL:
                strcpy (Ci.opcode, mylist->EName.lbl->sname);
                break;
            default:        /* Catchall */
                strcpy (Ci.opcode, "???");
            }

        }
        else      /* No reference entry for this area */
        {
            struct asc_data *mydat;

            /* Check for ASCII definition, and print it out if so */

            mydat = rof_find_asc (ascdat, CmdEnt);

            if (mydat)
            {
                PCPos = CmdEnt;    /* MovASC sets CmdEnt = Pc */
                MovASC (mydat->length, cclass);
                CmdEnt += mydat->length;
                PrevEnt = CmdEnt;
                datasize -= mydat->length;
                continue;
            }

            my_val = fgetc (ModFP);
            bump = 1;
            strcpy (Ci.mnem, "dc.b");
            sprintf (Ci.opcode, "$%02x", my_val);
        }

        PrintLine (realcmd, &Ci, cclass, CmdEnt, CmdEnt + datasize);
        CmdEnt += bump;
        PrevEnt = CmdEnt;
        datasize -= bump;
        mylist = mylist->ENext;
        nl = nl->Next;
    }

    return mylist;
}

static void
#ifdef __STDC__
ROFDataLst (struct rof_extrn *mylist, int maxcount, struct asc_data *ascdat,
            char cclass)
#else
ROFDataLst (mylist, maxcount, ascdat, cclass)
    struct rof_extrn *mylist; int maxcount; struct asc_data *ascdat; char cclass;
#endif
{
    struct rof_extrn *my_ref,
                     *srch;
    int datasize;

    my_ref = mylist;

    if (mylist)
    {
        if (mylist->Ofst < maxcount)
        {
            do {
                /*datasize = (mylist->Type >> 3) & 3;
                if (mylist->ENext && (mylist->ENext->Ofst < maxcount))
                {
                    datasize = mylist->ENext->Ofst - mylist->Ofst;
                }
                else
                {
                    datasize = maxcount - mylist->Ofst;
                }*/

                CmdEnt = my_ref->Ofst;
                DataDoBlock (my_ref, datasize, ascdat, cclass);
            } while ((mylist->ENext) && (mylist->Ofst < maxcount));
        }
    }
}

/* **************************************************************** *
 * ListInitROF() - moves initialized data into the listing.         *
 *                 Really a setup routine                           *
 * Passed : (1) nl - Ptr to proper nlist, positioned at the first   *
 *                    element to be listed                          *
 *          (2) mycount - count of elements in this sect.           *
 *          (3) class   - Label Class letter                        *
 * **************************************************************** */

void
#ifdef __STDC__
ListInitROF (char * hdr, LBLCLAS *nl, int mycount, char mclass)
#else
ListInitROF (hdr, datbegin, nl, mycount, notdp, mclass)
    hdr, LBLCLAS *nl; int mycount; char mclass;
#endif
{
    struct rof_extrn *mylist,
                     *srchlst;
    struct asc_data *ascdat;
    CMD_ITMS Ci;
    char *iClass = "_H";      /* Note: Need to add more refs */
    int r;

    if (fseek (ModFP, IDataBegin - 2, SEEK_SET) == -1)
    {
        errexit ("ListInitROF(): Failed to seek to begin of Initialized data");
    }

    for (r = 0; r < 2; r++,iClass++)
    {
        int rcount;
        int isize;

        CmdEnt = 0;

        switch (*iClass)
        {
        case '_':
            mylist = refs_idata;
            isize = ROFHd.idatsz;
            break;
        case 'H':
            mylist = refs_iremote;
            isize = ROFHd.remoteidatsiz;
            break;
        }

        ascdat = data_ascii;
        /*fseek (ModFP, IDataBegin, SEEK_SET);*/

        if (mylist)
        {
            int filPos = ftell(ModFP);

            PCPos = 0;

            do
            {
                LBLDEF *mylbl;
                register int myVal;
                char *dstname;

                memset (&Ci, 0, sizeof(CMD_ITMS));
                Ci.cmd_wrd = mylist->Ofst;
            
                switch ((mylist->Type >> 3) & 3)
                {
                case 1: /*SIZ_BYTE:*/
                    strcpy (Ci.mnem, "dc.b");
                    myVal = fgetc(ModFP);
                    Ci.cmd_wrd = myVal & 0xff;
                    --isize;
                    break;
                case 2: /*SIZ_WORD:*/
                    strcpy (Ci.mnem, "dc.w");
                    myVal = fread_w(ModFP);
                    Ci.cmd_wrd = myVal & 0xffff;
                    isize -=2;
                    break;
                default:
                    strcpy (Ci.mnem, "dc.l");
                    myVal = fread_l(ModFP);
                    Ci.cmd_wrd = (myVal >> 16) & 0xffff;
                    Ci.code[0] = myVal & 0xffff;
                    Ci.wcount = 1;
                    isize -= 4;
                }

                if ((mylbl = findlbl(*iClass, mylist->Ofst)))
                {
                    Ci.lblname = mylbl->sname;
                }

                /*if (mylist->dstClass)
                {
                    mylbl = findlbl(*iClass, myVal);
                    strcpy (Ci.opcode, mylbl ? mylbl->sname :
                }*/

                /*strcpy (Ci.opcode, mylist->dstClass ? findlbl(mylist->dstClass, myVal)->sname : mylist->EName.nam);*/
                strcpy (Ci.opcode, mylist->dstClass ? mylist->EName.lbl->sname : mylist->EName.nam);
                PrintLine(pseudcmd, &Ci, *iClass, 0, 0);

                mylist = mylist->ENext;
            } while (mylist);
        }
        /*else
        {
            mylist = DataDoBlock(mylist, mycount, ascdat, *iClass);
        }*/
    }
}

/* ******************************************************************* *
 * rof_ascii() - set up ASCII specification for initialized data from  *
 *                 the command file.                                   *
 *                                                                     *
 * Passed: ptr - pointer to command line position, Positioned to the   *
 *                  first character of the specification               *
 *                  line, past the "=" command file specifier          *
 *         Format for cmdline is "= d|n <start> - <end> or             *
 *                                      <start>/<length>               *
 * ******************************************************************* */

void
#ifdef __STDC__
rof_ascii ( char *cmdline)
#else
rof_ascii (cmdline)
char *cmdline;
#endif
{
    struct asc_data *me,
                    **tree = NULL;
    char oneline[80];

    while ((cmdline = cmdsplit (oneline, cmdline)))
    {
        char vsct,      /* vsect type, d=dp, b=bss */
             *ptr;
        int start,
            end;

        ptr = oneline;
        vsct = *ptr;
        ptr = skipblank (++ptr);

        getrange (ptr, &start, &end, 1, 0);

        if (end > 0)
        {
            me = (struct asc_data *)mem_alloc (sizeof (struct asc_data));
            memset (me, 0, sizeof(struct asc_data));

            me->start = start;
            me->length = end - start + 1;

            tree = &data_ascii;

            if ( ! (*tree))       /* If this tree has not been yet started */
            {
                *tree = me;
            }
            else
            {
                struct asc_data *srch;

                srch = *tree;

                while (1)
                {
                    if (start < srch->start)
                    {
                        if (srch->LNext)
                        {
                            srch = srch->LNext;
                        }
                        else
                        {
                            srch->LNext = me;
                            return;
                        }
                    }
                    else
                    {
                        if (start > srch->start)
                        {
                            if (srch->RNext)
                            {
                                srch = srch->RNext;
                            }
                            else
                            {
                                srch->RNext = me;
                                return;
                            }
                        }
                        else
                        {
                            fprintf (stderr,
                                     "Address %04x for vsect %c already defined\n",
                                     start, vsct
                                    );
                            return;
                        }
                    }
                }
            }
        }
    }
}

void
#ifdef __STDC__
setROFPass(void)
#else
setROFPass()
#endif
{
    if (Pass == 1)
    {
        codeRefs_sav = refs_code;
    }
    else
    {
        refs_code = codeRefs_sav;
    }
}

char *
#ifdef __STDC__
IsRef(char *dst, int curloc, int ival)
#else
IsRef(dst, curloc, ival)
    char *dst; int curloc; int ival;
#endif
{
    register char *retVal = NULL;

    if (refs_code && (refs_code->Ofst == curloc))
    {
        register struct rof_extrn *ep_tmp;

        if (Pass == 1)
        {
            refs_code = refs_code->ENext;
            return dst;
        }
        else
        {
            if (refs_code->Extrn)
            {
                strcpy(dst, refs_code->EName.nam);
                retVal = dst;

                if (ival)
                {
                    char offsetbuf[20];

                    strcat (dst, (refs_code->Type & 0x80) ? "-" : "+");
                    sprintf (offsetbuf, "%d", ival);
                    strcat (dst, offsetbuf);
                }
            }   /* Else leave retVal=NULL - for local refs, let calling process handle it */

            ep_tmp = refs_code->ENext;

            if (Pass == 2)
            {
                if (!refs_code->MyNext)
                {
                    /*if (refs_code->Extrn)
                    {
                        free (refs_code->EName.nam);
                    }*/
                }

                if (ep_tmp)
                {
                    ep_tmp->EUp = NULL;
                }

                free (refs_code);
                refs_code = ep_tmp;
            }
        }
    }       /* End "if (refs_code && (refs_code->Ofst == val)) */
    
    return retVal;
}
