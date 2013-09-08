/* ********************************************************************************** *
 * commonsubs.c - Common subroutines and functions that are used by several calls.    $
 *                                                                                    $
 * $Id::                                                                              $
 * ********************************************************************************** */

#include "disglobs.h"

typedef struct modestrs {
    char *str;
    int CPULvl;
} MODE_STR;

MODE_STR ModeStrings[] = {
    {"D%d", 0},
    {"A%d", 0},
    {"(A%d)", 0},
    {"(A%d)+", 0},
    {"-(A%d)", 0},
    {"%s(A%d)", 0},
    {"(%s,A%d,%s%d)", 0}
};
/* Need to add for 68020-up modes.  Don't know if they can be included in these two arrays or not..*/
MODE_STR Mode07Strings[] = {
    {"(%s).w", 0},
    {"(%s).l", 0},
    {"(%s},pc)",0},
    {"(%s,pc},%s%d", 0},
    {"#%s", 0}
};

char dispRegNam[2] = {'d','a'};

/* ---------------------------------------------------------------------------------- *
 * get_eff_addr() - Build the appropriate opcode string for the command and store it  *
 *     in the command structire opcode string                                         *
 * ---------------------------------------------------------------------------------- */

/* NOTE: SHOULD THE VALID MODES BE CONSIDERED HERE? */


int
#ifdef __STDC__
get_eff_addr(CMD_ITMS *ci, char *ea, int mode, int reg)
#else
get_eff_addr(ci, ea, mode, reg)
    CMD_ITMS *ci;
    char * ea;
int mode;
int reg;
#endif
{
    short ext1;
    short ext2;
    short displac_w;
    int displac_l;
    char dispstr[50];
    char dispReg[4];

    switch (mode)
    {
    case 0:   /* "Dn" */
    case 1:
    case 2:
    case 3:
    case 4:
        sprintf(ea, ModeStrings[mode].str,reg);
        return 1;
        break;
    case 5:
        ext1 = get_extw(ci);
        ++(ci->wcount);
        displac_w = (ext1 & 0xffff);

        /* The system biases the data Pointer (a6) by 0x8000 bytes, so compensate */

        if (reg == 6) {
            displac_w += 0x8000;
        }

        /* NOTE:: NEED TO TAKE INTO ACCOUNT WHEN DISPLACEMENT IS A LABEL !!! */
        sprintf(dispstr, "%d", displac_w);
        sprintf (ea, ModeStrings[mode].str,dispstr,reg);
        return 1;
        break;
    case 6:
        ext1 = get_extw(ci);
        ++(ci->wcount);
        displac_w = (ext1 & 0xffff);

        if (reg == 6) {     /* Compensate for Data Register bias */
            displac_w += 0x8000;
        }
        sprintf (dispReg, "%c%d", dispRegNam[ext1 >> 15], (ext1 >> 12) & 7);
        /* NOTE:: NEED TO TAKE INTO ACCOUNT WHEN DISPLACEMENT IS A LABEL !!! */
        sprintf (dispstr, "%d", displac_w);
        sprintf (ea, ModeStrings[mode].str, dispReg, dispstr, reg);
        return 1;
        break;

        /* We now go to mode %111, where the mode is determined by the register field */
    case 7:
        switch (reg) {
        case 0: /* (xx).W */
            ext1 = get_extw(ci);
            ++(ci->wcount);
            displac_w = ext1 & 0xffff;
            /* NOTE:: NEED TO TAKE INTO ACCOUNT WHEN DISPLACEMENT IS A LABEL !!! */
            sprintf (dispstr, "%d", displac_w);
            sprintf (ea, ModeStrings[reg].str, dispstr);
            return 1;
        case 1:                               /* (xxx).L */
            ext1 = get_extw(ci);
            ++(ci->wcount);
            ext2 = get_extw(ci);
            ++(ci->wcount);
            sprintf (dispstr, "%dL", (ext1 <<16) | ext2);
            sprintf (ea, ModeStrings[reg].str, dispstr);
            return 1;
        case 4:
            ext1 = get_extw(ci);
            ++(ci->wcount);

            switch (ci->code[0] >> 12)
            {
                char b;
            case 1:    /* byte */
                b = ext1 & 0xff;
                sprintf (dispstr, "%d", (int)b);
                break;
            case 3:    /* word */
                displac_w = ext1 & 0xffff;
                sprintf (dispstr, "%d", displac_w);
                break;
            case 2:    /* long */
                ext2 = get_extw(ci);
                ++(ci->wcount);
                displac_l = (ext1 << 8) | ext2;
                sprintf (dispstr, "%dL", displac_l);
                break;
            }

             sprintf (ea, Mode07Strings[reg].str, dispstr);
            return 1;
        case 2:              /* (d16,PC) */
            ext1 = get_extw(ci);
            ++(ci->wcount);
            sprintf (dispstr, "%d", ext1);
            sprintf (ea, Mode07Strings[reg].str, dispstr);
            return 1;
        case 3:
            ext1 = get_extw(ci);
            ++(ci->wcount);
            displac_w = (ext1 & 0xffff);
            sprintf (dispReg, "%c%d", dispRegNam[ext1 >> 15], (ext1 >> 12) & 7);
            /* NOTE:: NEED TO TAKE INTO ACCOUNT WHEN DISPLACEMENT IS A LABEL !!! */
            sprintf (dispstr, "%d", displac_w);
            sprintf (ea, Mode07Strings[mode].str, dispReg, dispstr, reg);
            return 1;
        }
    }

    return 0;    /* Return 0 means no effective address was found */
}

/* ---------------------------------------------------------------------------------- *
 * get_extends_common() - Gets the extended data, sets up the ea for calls where the  *
 * the the size is the 3 MS=bits of the lower byte of the command, and the EA is the  *
 * lower 5 bytes of the command word.                                                 *
 * Passed: (1) - the command items structure                                          *
 *         (2) - the mnemonic for the call.  This routine adds the size descriptor    *
 * ---------------------------------------------------------------------------------- */

int sizebits[] = {".b", ".w", ".l"};

int
#ifdef __STDC__
    get_extends_common(CMD_ITMS *ci, char *mnem)
#else
    get_extends_common(ci)
    CMD_ITMS *ci;
#endif
{
    int mode, reg;
    int size;
    char addr_mode[20];              /* Destination for the opcode */
    int data_word;

    mode = (ci->code[0] >> 3) & 7;
    reg = ci->code[0] & 7;
    size = (ci->code[0] >> 6) & 3;

    get_eff_addr(ci, addr_mode, mode, reg);
    get_extw(ci);
    
    if (size > 1) {
        get_extw(ci);
    }
}