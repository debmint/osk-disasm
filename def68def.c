/* ************************************************************************ *
 * def68def.c -                                                             *
 *                                                                          *
 * ************************************************************************ *
 *                                                                          *
 * This file is from Motorola's FBug system and is public domain.           *
 *                                                                          *
 * ************************************************************************ */

#include "disglobs.h"
#include "userdef.h"
#include "proto.h"

static char sizechar[2];

/* ********************************************* */
/* ROUTINE: BITNUM							     */
/* ********************************************* */

static int
#ifdef __STDC__
bitnum (int startbit, int endbit, int opword)
#else
bitnum (startbit, endbit, opword)
     int startbit,
         endbit,
         opword;
#endif
{
    int shifted_word,
        i,
        value;
    value = 0;

    for (i = 0; i <= (startbit - endbit); i++)
    {
        shifted_word = (opword >> (i + endbit));

        if ((shifted_word & 0x1) == 1)
            value = value + (1 << i);
    }
    return (value);
}

#if (DEVICE==68040 || COPROCESSOR==TRUE)

/* ********************************************* */
/* ROUTINE: FPMATCH							     */
/* ********************************************* */

int
#ifdef __STDC__
fpmatch (int start)
#else
fpmatch (start)
     int start;
#endif
{
    extern error;
    extern char sizechar[];

    int operationword;
    int commandword;
    int type;

    operationword = get16 (start);
    commandword = get16 (start + 2);
    type = bitnum (8, 6, operationword);

    if (bitnum (15, 9, operationword) != 0x79   /* F-Line reserved          */
        || (type == 0x6)        /* type undefined reserved  */
        || (type == 0x7))       /* type undefined reserved  */
    {
        error = TRUE;
        return (-1);
    }
    switch (type)
    {
        case 0:                /* General Instructions  */
            sizechar[0] = 'x';  /* As a default to prevent a failure 
                                   in disasm68.c */

            return (200);       /* A SPECIFIC DETERMINATION WILL 
                                   BE MADE IN DISASM68.C        */
        case 1:                /* FDBcc,FScc or FTRAPcc */
            if ((bitnum (16, 6, commandword) != 0x0)
                || (bitnum (5, 5, commandword) == 0x1))
            {
                error = TRUE;
                return (-1);
            }
            if (bitnum (5, 3, operationword) == 0x1)
            {
                return (204);
            }
            else if (bitnum (5, 3, operationword) == 0x7)
            {
                if (bitnum (2, 0, operationword) == 0x2)
                {
                    sizechar[0] = 'w';
                    return (214);
                }
                else if (bitnum (2, 0, operationword) == 0x3)
                {
                    sizechar[0] = 'l';
                    return (214);
                }
                else if (bitnum (2, 0, operationword) == 0x4)
                    return (213);
            }
            sizechar[0] = 'b';
            return (212);
        case 2:                /* FBcc.W */
        case 3:                /* FBcc.L */
            if (bitnum (5, 5, get16 (start)) == 1)
            {
                error = TRUE;
                return (-1);
            }
            if (type == 2 && get16 (start + 2) == 0x0
                && bitnum (5, 0, operationword) == 0x0)
                return (209);
            else if (type == 2)
                sizechar[0] = 'w';
            else
                sizechar[0] = 'l';
            return (203);
        case 4:                /* FSAVE */
            return (211);
        case 5:                /* FRESTORE */
            return (210);
    }
}
#endif

/* ********************************************* */
/* ROUTINE: TABLEMATCH							 */
/* ********************************************* */

OPSTRUCTURE *
#ifdef __STDC__
tablematch (int opword, OPSTRUCTURE *entry)
#else
tablematch (opword, entry)
     int opword;
     OPSTRUCTURE *entry;
#endif
{
    /*extern OPSTRUCTURE syntax1[];*/
    extern int error;
	int Bmatch;
    int j,
        b,
        c;

    Bmatch = 1;

    for (j = 15; j > 3; j--)
    {
        if (entry->opwordstr[j] != 'x')
        {
            b = (entry->opwordstr[j] == '1' && !(opword & 0x0001));
            c = (entry->opwordstr[j] == '0' && (opword & 0x0001));

            if (b || c)
            {
                Bmatch = 0;         /* MATCH FLAG IS SET TO FALSE */
                break;
            }
        }

        opword = opword >> 1;
    }

    if (Bmatch == 0)
    {
        error = TRUE;
    }

    return entry;
}

