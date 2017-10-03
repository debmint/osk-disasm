
/* ******************************************************************** *
 * rof.c - handles rof files                                            *
 *                                                                      *
 * ******************************************************************** *
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
static void ROFDataLst (struct rof_extrn *mylist, int maxcount, struct asc_data *ascdat, char cclass);
static void get_refs(char *vname, int count, int ref_typ, char *codebuffer);
#else
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
1141 (tbl, dataSiz, klas);
    struct rof_extrn *tbl, int dataSiz; char klas;
#endif
{
    char *dataBuf,
         *ptr;
    int c = 1;
    register int refVal;

    dataBuf = (char *)mem_alloc(dataSiz + 1);

    if (fread (dataBuf, dataSiz, 1, ModFP) == -1)
    {
        fprintf (stderr, "AddInitLbls(): Failed to read in data for Init Data Buffer");
        exit (errno);
    }

    while (tbl)
    {
        if (!tbl->Extrn)
        {
            ptr = &(dataBuf[tbl->Ofst]);

            switch (REFSIZ(tbl->Type))
            {
            case 1: /*SIZ_BYTE:*/
                refVal = *ptr & 0xff;
                --dataSiz;
                break;
            case 2: /*SIZ_WORD:*/
                /*refVal = 0;
                refVal = fread_w(ModFP);*/
                refVal = bufReadW(&ptr);
                dataSiz -= 2;
                break;
            default:
                refVal = bufReadL(&ptr);
                /*refVal = fread_l(ModFP);*/
                dataSiz -= 4;
            }

            tbl->EName.lbl = addlbl(tbl->dstClass, refVal, "");
        }

        tbl = tbl->ENext;
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
    char *codeBuf;

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

    /* Read code into buffer for get_refs() while we're here */



    if (fread ((codeBuf = (char *)mem_alloc(ROFHd.codsz + 1)), ROFHd.codsz, 1, ModFP) == -1)
    {
        fprintf (stderr, "Failed to read code buffer\n");
    }

    /*idp_begin = code_begin + rofptr->codsz;
    indp_begin = idp_begin + rofptr->idpsz;*/

    /*if (fseek (progpath, ROFHd.codsz, SEEK_CUR) == -1)
    {
        fprintf (stderr, "rofhdr(): Seek error on module\n");
        exit (errno);
    }*/

    /* ********************************** *
     *    Initialized data Section        *
     * ********************************** */

    IDataCount = ROFHd.idatsz;
    IDataBegin = ftell(ModFP);

    /* ********************************** *
     *    External References Section     *
     * ********************************** */

    if (fseek (ModFP, IDataBegin + ROFHd.idatsz + ROFHd.remotestatsiz + ROFHd.debugsiz, SEEK_SET) == -1)
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

        get_refs (_name, refcount, REFXTRN, NULL);
    }


    /* *************************** *
     *    Local variables...       *
     * *************************** */

    local_count = fread_w (ModFP);
    get_refs("", local_count, REFLOCAL, codeBuf);
    free (codeBuf);

    /* Now we need to add labels for these refs */

    /* common block variables... */
    /* Do this after everything else is done */
    /* NOTE: We may need to save current ftell() to restore it after this */

    if (fseek(ModFP, IDataBegin, SEEK_SET) == -1)
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
        switch (typ & 0x20)
        {
        case 0: /* NOT remote */
            switch (typ & 0x200)
            {
            case 0:     /* data */
                return '_';
            default:    /* remote */
                return 'L';
            }
        default:
            switch (typ & 0x200)
            {
            case 0:     /* code */
                return 'L';
            default:      /* debug */
                break;
            }
            return 'L';
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
get_refs(char *vname, int count, int ref_typ, char *code_buf)
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

        /* Skip Debug refs */
        if (ref_typ == REFLOCAL)
        {
            if ((_ty & 0x220) == 0x220)
            {
                continue;
            }
        }

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

        /*if ((ref_typ == REFLOCAL) && (myClass == 'L'))
        {
        }
        else*/
        {
            new_ref = (struct rof_extrn *)mem_alloc(sizeof(struct rof_extrn));
            memset (new_ref, 0, sizeof(struct rof_extrn));

            new_ref->Type = _ty;
            new_ref->Ofst = _ofst;
            new_ref->Extrn = (ref_typ == REFXTRN);

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
            else
            {
                register int dstVal;
                char *pt = &(code_buf[new_ref->Ofst]);

                new_ref->dstClass = rof_class(_ty, REFGLBL);
                
                if ((ref_typ == REFLOCAL) && (myClass == 'L'))
                {
                    switch ((new_ref->Type >> 3) & 3)
                    {
                    case 1:
                        dstVal = *pt & 0xff;
                        break;
                    case 2:
                        dstVal = bufReadW(&pt);
                        break;
                    default:
                        dstVal = bufReadL(&pt);
                    }

                    new_ref->EName.lbl = addlbl(new_ref->dstClass, dstVal, "");
                }
            }
        }               /* end "if (ref_typ == REFXTRN)" */
    }               /* end "while (count--) */
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

static char *
#ifdef __STDC__
DataDoBlock (struct rof_extrn **refsList, LBLDEF **lblList, char *iBuf, int blkEnd,
             struct asc_data *ascdat, char cclass)
#else
DataDoBlock (refsList, iBuf, int blkEnd, ascdat, cclass)
    struct rof_extrn **refsList; char *iBuf; int blkEnd;
             struct asc_data *ascdat, char cclass;
#endif
{
    /*struct rof_extrn *srch;*/
    CMD_ITMS Ci;
    char lblString[200];

    memset (&Ci, 0, sizeof(CMD_ITMS));

    /* Insert Label if applicable */

    if ((*lblList)->myaddr == CmdEnt)
    {
        strcpy (lblString, (*lblList)->sname);
        Ci.lblname = lblString;

        if ((*lblList)->global)
        {
            strcat (Ci.lblname, ":");
        }

        (*lblList) = (*lblList)->Next;
    }

    while (PCPos < blkEnd)
    {
        int bump = 2,
            my_val;

        CmdEnt = PCPos;
        /* First check that refsList is not null. If this vsect has no
         * references, 'refsList' will be null
         */

        if ( *refsList && ((*refsList)->Ofst == CmdEnt) )
        {
            strcpy (Ci.mnem, "dc.");

            switch (((*refsList)->Type >> 3) & 3)
            {
            case 1:         /* SIZ_BYTE */
                strcat (Ci.mnem, "b");
                my_val = *(iBuf++);
                ++PCPos;
                break;
            case 2:         /* SIZ_WORD */
                strcat (Ci.mnem, "w");
                my_val = bufReadW(&iBuf) & 0xffff;
                /*iBuf += 2;*/
                /*my_val = (*(iBuf++) << 8) | (*(iBuf++) & 0xff);*/
                PCPos += 2;
                break;
            default:         /* SIZ_LONG */
                strcat (Ci.mnem, "l");
                my_val = bufReadL(&iBuf);
                /*iBuf += 4;*/
                /*my_val = (*(iBuf++) << 24) | ((*(iBuf++) & 0xff) << 16) | ((*(iBuf++) & 0xff) << 8) |
                    (*(iBuf++) & 0xff);*/
                PCPos += 4;
            }

            if ((*refsList)->Extrn)
            {
                strcpy(Ci.opcode, (*refsList)->EName.nam);
            }
            else
            {
                strcpy (Ci.opcode, (*refsList)->EName.lbl->sname);
            }

            PrintLine(pseudcmd, &Ci, cclass, CmdEnt, CmdEnt);
            CmdEnt = PCPos;
            Ci.lblname = NULL;
            Ci.opcode[0] = '\0';
            Ci.mnem[0] = '\0';
            /*switch ((*refsList)->Type)
            {
            case REFGLBL:
                strcpy(Ci.opcode, (*refsList)->EName.nam);
                break;
            case REFLOCAL:
                strcpy (Ci.opcode, (*refsList)->EName.lbl->sname);
                break;
            default:
                strcpy (Ci.opcode, "???");
            }*/

        }
        else      /* No reference entry for this area */
        {
            struct asc_data *mydat;
            register int bytCount,
                bytSize;

            if (bytCount = DoAsciiBlock (&Ci, iBuf, blkEnd, cclass))
            {
                iBuf += bytCount;
                continue;
            }
            else
            {
                register char *fmt;

                switch ((blkEnd - PCPos) % 4)
                {
                case 0:
                    bytSize = 4;
                    bytCount = (blkEnd - PCPos) >> 2;
                    strcpy (Ci.mnem, "dc.l");
                    fmt = "$%08x";
                    break;
                case 2:
                    bytSize = 2;
                    strcpy (Ci.mnem, "dc.w");
                    bytCount = (blkEnd - PCPos) >> 1;
                    fmt = "$%04x";
                    break;
                default:
                    bytSize = 1;
                    strcpy (Ci.mnem, "dc.b");
                    bytCount = blkEnd - PCPos;
                    fmt = "$%02x";
                }

                while (bytCount--)
                {
                    int val = 0;
                    register int byteNum;

                    switch (bytSize)
                    {
                    case 1:
                        val = *(iBuf++) & 0xff;
                        break;
                    case 4:
                        /*for (byteNum = 0; byteNum < 4; byteNum++)
                        {
                            val = (val << 8) | (*(iBuf++) & 0xff);
                        }*/
                        val = bufReadL(&iBuf);
                        /*iBuf += 4;*/
                        break;
                        /*val = ((*(iBuf++) & 0xff) << 8) | (*(iBuf++) & 0xff);
                        val <<= 16;*/
                        /* Fall through to the 'word' function to pick up next two bytes */
                    default:
                        val = bufReadW(&iBuf);
                        /*iBuf +=2;*/
                        /*val = val | ((*(iBuf++) & 0xff) << 8) | (*(iBuf++) & 0xff);*/
                    }

                    PCPos += bytSize;
                    sprintf (Ci.opcode, fmt, val);
                    PrintLine(pseudcmd, &Ci, cclass, CmdEnt, CmdEnt);
                    CmdEnt = PCPos;
                    Ci.lblname = NULL;
                    Ci.opcode[0] = '\0';
                }
                
                Ci.mnem[0] = '\0';
            }

            /* Check for ASCII definition, and print it out if so */

        //    mydat = rof_find_asc (ascdat, CmdEnt);

        //    if (mydat)
        //    {
        //        PCPos = CmdEnt;    /* MovASC sets CmdEnt = Pc */
        //        MovASC (mydat->length, cclass);
        //        CmdEnt += mydat->length;
        //        PrevEnt = CmdEnt;
        //        blkEnd -= mydat->length;
        //        continue;
        //    }

        //    my_val = fgetc (ModFP);
        //    bump = 1;
        //    strcpy (Ci.mnem, "dc.b");
        //    sprintf (Ci.opcode, "$%02x", my_val);
        }

        //PrintLine (realcmd, &Ci, cclass, CmdEnt, CmdEnt + blkEnd);
        //CmdEnt += bump;
        //PrevEnt = CmdEnt;
        //blkEnd -= bump;
        //(*refsList) = (*refsList)->ENext;
        //nl = nl->Next;
    }

    return iBuf;
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
                /*DataDoBlock (&my_ref, "", datasize, ascdat, cclass);*/
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
ListInitROF (char * hdr, struct rof_extrn *refsList, char *iBuf, int isize, char iClass)
#else
ListInitROF (hdr, refsList, iBuf, isize, iClass)
    hdr; struct rof_extrn *refsList; char *iBuf; int isize; char iClass;
#endif
{
    struct rof_extrn *srchlst;
    struct asc_data *ascdat;
    CMD_ITMS Ci;
    int r;
    int rcount;
    LBLDEF *lblList = labelclass(iClass) ? labelclass(iClass)->cEnt : NULL;

    ascdat = data_ascii;
    PCPos = 0;
    {
        LBLDEF *lbls = labelclass(iClass) ? labelclass(iClass)->cEnt : NULL;
    }

    while (PCPos < (isize))
    {
        char *dstname;
        register int blkEnd;

        blkEnd = isize;     /* Make this a default */

        if (lblList)
        {
            if (lblList->Next)
            {
                blkEnd = lblList->Next->myaddr;
            }
        }

        iBuf = DataDoBlock (&refsList, &lblList, iBuf, blkEnd, ascdat, iClass);

        //memset (&Ci, 0, sizeof(CMD_ITMS));
        //Ci.cmd_wrd = refsList->Ofst;
        //CmdEnt = PCPos;
        // 
        //if (refsList && (refsList->Ofst == PCPos))
        //{
        //    register int myVal;

        //    switch ((refsList->Type >> 3) & 3)
        //    {
        //    case 1: /*SIZ_BYTE:*/
        //        strcpy (Ci.mnem, "dc.b");
        //        myVal = *(iBuf++);
        //        Ci.cmd_wrd = myVal & 0xff;
        //        ++PCPos;
        //        break;
        //    case 2: /*SIZ_WORD:*/
        //        strcpy (Ci.mnem, "dc.w");
        //        myVal = (*(iBuf++) << 8) | (*(iBuf++) & 0xff);
        //        Ci.cmd_wrd = myVal & 0xffff;
        //        PCPos += 2;
        //        break;
        //    default:
        //        strcpy (Ci.mnem, "dc.l");
        //        myVal = (*(iBuf++) << 24) | (((*iBuf++) & 0xff) << 16) | ((*(iBuf++) & 0xff) << 8) |
        //            (*(iBuf++) & 0xff);
        //        myVal = fread_l(ModFP);
        //        Ci.cmd_wrd = (myVal >> 16) & 0xffff;
        //        Ci.code[0] = myVal & 0xffff;
        //        Ci.wcount = 1;
        //        PCPos += 4;
        //    }

        //    //if ((mylbl = findlbl(iClass, refsList->Ofst)))
        //    if (lblList->myaddr == PCPos)
        //    {
        //        Ci.lblname = lblList->sname;
        //        lblList  = lblList->Next;
        //    }

        //    /*if (refsList->dstClass)
        //    {
        //        mylbl = findlbl(*iClass, myVal);
        //        strcpy (Ci.opcode, mylbl ? mylbl->sname :
        //    }*/

        //    strcpy (Ci.opcode, refsList->Extrn ? refsList->EName.nam : refsList->EName.lbl->sname);
        //    PrintLine(pseudcmd, &Ci, iClass, 0, 0);

        //    refsList = refsList->ENext;
        //}
        //else
        //{
        //    register int nxtVal;

        //    if (refsList)
        //    {
        //        nxtVal = refsList->Ofst;        /* This Value should always be greater than PCPos */
        //    }

        //}
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
            else
            {
                strcat(dst, refs_code->EName.lbl->sname);
                retVal = dst;
            }

            ep_tmp = refs_code->ENext;

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
    }       /* End "if (refs_code && (refs_code->Ofst == val)) */
    
    return retVal;
}
