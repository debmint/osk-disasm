/* ************************************************************************ *
 * commonsubs.c - Common subroutines and functions that are used            $
 *          by several calls.                                               $
 *                                                                          $
 * $Id::                                                                    $
 * ************************************************************************ */

#include "userdef.h"
#include "disglobs.h"
#include "proto.h"

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
    {"(%s,A%d,%c%d)", 0}
};
/* Need to add for 68020-up modes.  Don't know if they can be included in these two arrays or not..*/
MODE_STR Mode07Strings[] = {
    {"(%s).w", 0},
    {"(%s).l", 0},
    {"(%s},pc)",0},
    {"(%s,pc},%c%d", 0},
    {"#%s", 0}
};

char dispRegNam[2] = {'d','a'};

/*
 * get_eff_addr() - Build the appropriate opcode string for the command
 *     and store it in the command structure opcode string
 *
 */

int
#ifdef __STDC__
get_eff_addr(CMD_ITMS *ci, char *ea, int mode, int reg, int size)
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
    struct extWbrief ew_b;

    switch (mode)
    {
    default: return 0;
    case 0:   /* "Dn" */
    case 1:   /* "An" */
    case 2:   /* (An) */
    case 3:   /* (An)+ */
    case 4:   /* -(An) */
        sprintf(ea, ModeStrings[mode].str,reg);
        return 1;
        break;
    case 5:             /* d{16}An */
        ext1 = getnext_w(ci);
        //++(ci->wcount);
        displac_w = (ext1 & 0xffff);

        /* The system biases the data Pointer (a6) by 0x8000 bytes,
         * so compensate
         */

        if (reg == 6) {
            displac_w += 0x8000;
        }

        /* NOTE:: NEED TO TAKE INTO ACCOUNT WHEN DISPLACEMENT IS A LABEL !!! */
        sprintf(dispstr, "%d", displac_w);
        sprintf (ea, ModeStrings[mode].str,dispstr,reg);
        return 1;
        break;
    case 6:             /* d{8}(An)Xn or 68020-up */
        //ext1 = getnext_w(ci);

        if (get_ext_wrd_brief (ci, &ew_b, mode, reg))
        {
            /* the displacement should be a string for it may sometimes
             * be a label */
            char a_disp[50];
            sprintf (a_disp, "%d", ew_b.displ);
            sprintf (ea, ModeStrings[mode].str, a_disp, reg,
                    ew_b.d_a, ew_b.regno);
            return 1;
        }
        else
        {
            return 0;
        }

        /* NOTE:: NEED TO TAKE INTO ACCOUNT WHEN DISPLACEMENT IS A LABEL !!! */
        break;

        /* We now go to mode %111, where the mode is determined by the register field */
    case 7:
        switch (reg) {
        case 0:                 /* (xxx).W */
            ext1 = getnext_w(ci);
            //++(ci->wcount);
            displac_w = ext1 & 0xffff;
            /* NOTE:: NEED TO TAKE INTO ACCOUNT WHEN DISPLACEMENT IS A LABEL !!! */
            sprintf (dispstr, "%d", displac_w);
            sprintf (ea, ModeStrings[reg].str, dispstr);
            return 1;
        case 1:                /* (xxx).L */
            ext1 = getnext_w(ci);
            //++(ci->wcount);
            ext2 = getnext_w(ci);
            //++(ci->wcount);
            sprintf (dispstr, "%dL", (ext1 <<16) | ext2);
            sprintf (ea, Mode07Strings[reg].str, dispstr);
            return 1;
        case 4:                 /* #<data> */
            ext1 = getnext_w(ci);
            //++(ci->wcount);

            switch (size)
            {
                char b;
            case 0:    /* byte */
                b = ext1 & 0xff;
                sprintf (dispstr, "%d", (int)b);
                break;
            case 1:    /* word */
                displac_w = ext1 & 0xffff;
                sprintf (dispstr, "%d", displac_w);
                break;
            case 2:    /* long */
                ext2 = getnext_w(ci);
                //++(ci->wcount);
                displac_l = (ext1 << 16) | ext2;
                sprintf (dispstr, "%dL", displac_l);
                break;
            }

             sprintf (ea, Mode07Strings[reg].str, dispstr);
            return 1;
        case 2:              /* (d16,PC) */
            ext1 = getnext_w(ci);
            //++(ci->wcount);
            sprintf (dispstr, "%d", ext1);
            sprintf (ea, Mode07Strings[reg].str, dispstr);
            return 1;
        case 3:              /* d8(PC)Xn */
            //ext1 = getnext_w(ci);

            if (get_ext_wrd_brief (ci, &ew_b, mode, reg))
            {
                char a_disp[50];
                sprintf (a_disp, "%d", ew_b.displ);
                sprintf (ea, Mode07Strings[mode].str, a_disp, reg,
                        ew_b.d_a, ew_b.regno);
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }

    return 0;    /* Return 0 means no effective address was found */
}

/* ------------------------------------------------------------------------ *
 * get_ext_wrd_brief() - Retrieves the extended command word, and sets up   *
 *      the values.                                                         *
 * Returns 1 if valid, 0 if not                                             *
 * ------------------------------------------------------------------------ */

#ifdef __STDC__
get_ext_wrd_brief (CMD_ITMS *ci, struct extWbrief *extW, int mode, int reg)
#else
get_ext_wrd_brief (ci)
    CMD_ITMS *ci;
    struct extWbrief *extW;
    int mode, reg;
#endif
{
    int ew;     /* A local copy of the extended word (stored in ci->code[0] */

    ew = getnext_w(ci);

    if ((cpu < 20) && (ew & 0x0100))
    {
        ungetnext_w(ci);
        return 0;
    }

    /* get the values common to all */
    extW->d_a = (ew & 0x8000) ? 'a' : 'd';
    extW->regno = (ew >> 14) & 7;
    extW->isize = (ew >> 11) & 1;
    extW->scale = (ew >>10) * 3;
    extW->displ = ew & 0xff;

}

/* ------------------------------------------------------------------------ *
 * get_extends_common() - Gets the extended data, sets up the ea for calls  *
 *       where the size is the 3 MS=bits of the lower byte of the command,  *
 *       and the EA is the lower 5 bytes of the command word.               *
 * Passed: (1) - the command items structure                                *
 *         (2) - the mnemonic for the call.  This routine adds the size     *
 *               descriptor                                                 *
 * ------------------------------------------------------------------------ */

char *sizebits[] = {".b", ".w", ".l"};

int
#ifdef __STDC__
get_extends_common(CMD_ITMS *ci, char *mnem)
#else
get_extends_common(ci, mnem)
    CMD_ITMS *ci;
    char *mnem;
#endif
{
    int mode, reg;
    int size;
    char addr_mode[20];              /* Destination for the opcode */
    int data_word;

    mode = (ci->cmd_wrd >> 3) & 7;
    reg = ci->cmd_wrd & 7;
    size = (ci->cmd_wrd >> 6) & 3;

    get_eff_addr(ci, addr_mode, mode, reg, size);
    getnext_w(ci);
    
    if (size > 1) {
        getnext_w(ci);
    }
}
