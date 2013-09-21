/* ************************************************************************ *
 * commonsubs.c - Common subroutines and functions that are used            $
 *          by several calls.                                               $
 *                                                                          $
 * $Id::                                                                    $
 * ************************************************************************ */

#include <string.h>
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
    {"%s(A%d,%c%d)", 0}
};
/* Need to add for 68020-up modes.  Don't know if they can be included in these two arrays or not..*/
MODE_STR Mode07Strings[] = {
    {"(%s).w", 0},
    {"(%s).l", 0},
    {"%s(pc)",0},
    {"%s(pc,%c%d)", 0},
    {"#%s", 0}
};

char dispRegNam[2] = {'d','a'};

extern char *SizSufx[];

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
                sprintf (ea, Mode07Strings[reg].str, a_disp,
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

int
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
    extW->regno = (ew >> 12) & 7;
    extW->isize = (ew >> 11) & 1;
    extW->scale = (ew >> 9) * 3;
    extW->displ = ew & 0xff;
    return 1;
}

/* ------------------------------------------------------------------------ *
 * get_extends_common() - Gets the extended data, sets up the ea for calls  *
 *       where the size is the 3 MS=bits of the lower byte of the command,  *
 *       and the EA is the lower 5 bytes of the command word.               *
 * Passed: (1) - the command items structure                                *
 *         (2) - the mnemonic for the call.  This routine adds the size     *
 *               descriptor                                                 *
 * ------------------------------------------------------------------------ */

char *sizebits[] = {".s", ".w", ".l"};

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

/* ----------------------------------------------------------------- *
 * ctl_addrmodesonly - Returns 0 if the addressing mode is anything  *
 *    but a Control addressing mode                                  *
 * ----------------------------------------------------------------- */

int
#ifdef __STDC__
ctl_addrmodesonly(int mode, int reg)
#else
ctl_addrmodesonly(mode, reg)
#endif
{
    if ((mode < 2) || (mode == 4) || (mode == 8))
        return 0;

    if (mode == 7)
    {
        if (mode == 4)
            return 0;
    }

    return 1;
}

/* ----------------------------------------------------------------- *
 * reg_ea - Functions that deal with a register and hold the reg #   *
 *      in the command word, and also have an effective address      *
 * ----------------------------------------------------------------- */

int
#ifdef __STDC__
reg_ea(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
reg_ea(ci, j, op)
    CMD_ITMS *ci;
    int j;
    OPSTRUCTURE *op;
#endif
{
    int mode = (ci->cmd_wrd >> 3) & 7;
    int reg = ci->cmd_wrd & 7;
    int size = (ci->cmd_wrd >> 7) & 3;
    char ea[50];
    char *regname = "d";

    /* Eliminate illegal Addressing modes */
    switch (j)
    {
        case 30:        /* chk */
        case 70:        /* divu */
        case 71:        /* divs */
        case 79:        /* mulu */
        case 80:        /* muls */
            if ((mode == 1))
                return 0;

            switch (size)
            {
                case 2:
                    size = SIZ_LONG;
                    break;
                case 3:
                    size = SIZ_WORD;
                    break;
                default:
                    return 0;
            }
        case 31:       /* lea */
            if (! ctl_addrmodesonly)
                return 0;
    }

    switch (j)
    {
        default:
            break;  /* Already checked above */
    }

    if (j == 31)
    {
        regname = "a";
    }

    if (get_eff_addr(ci, ea, mode, reg, size))
    {
        sprintf(ci->opcode, "%s,%c%d", ea, regname[0], (ci->cmd_wrd >> 9) & 7);
        strcpy (ci->mnem, op->name);

        switch (size)
        {
            case SIZ_BYTE:
                strcat(ci->mnem, "b");
                break;
            case SIZ_WORD:
                strcat(ci->mnem, "w");
                break;
            case SIZ_LONG:
                strcat(ci->mnem, "l");
            default:
                ci->mnem[strlen(ci->mnem) - 1] = '\0';
        }

        return 1;
    }

    return 0;

}

/*
 * Format a range of registers (``low'' to ``high'')
 * into ``s'', beginning with a ``slash'' if necessary.
 * ``ad'' specifies either address (A) or data (D) registers.
 */
static char *
regwrite(char *s, char *ad, int low, int high, int slash)
{
	if (slash)
		*s++ = '/';
	if (high - low >= 1) {
		s += sprintf(s, "%s", ad);
		*s++ = low + '0';
		*s++ = '-';
		s += sprintf(s, "%s", ad);
		*s++ = high + '0';
	} else if (high - low == 1) {
		s += sprintf(s, "%s", ad);
		*s++ = low + '0';
		*s++ = '/';
		s += sprintf(s, "%s", ad);
		*s++ = high + '0';
	} else {
		s += sprintf(s, "%s", ad);
		*s++ = high + '0';
	}
	*s = '\0';

	return s;
}

/*
 * Format ``regmask'' into ``s''.  ``ad'' is a prefix used to indicate
 * whether the mask is for address, data, or floating-point registers.
 */
char *
regbyte(char *s, unsigned char regmask, char *ad, int doslash)
{
	int	i;
	int	last;

	for (last = -1, i = 0; regmask; i++, regmask >>= 1)
    {
		if (regmask & 1) {
			if (last != -1)
				continue;
			else
				last = i;
		} else if (last != -1) {
			s = regwrite(s, ad, last, i - 1, doslash);
			doslash = 1;
			last = -1;
		}
    }

	if (last != -1)
		s = regwrite(s, ad, last, i - 1, doslash);

	return s;
}


/* ------------------------------------------------------------------ *
 * revbits() - Reverses the bit order for a value.                    *
 * ------------------------------------------------------------------ */


int
#ifdef __STDC__
revbits(int num, int lgth)
#else
revbits(num, lgth)
    int num, lgth;
#endif
{
    int n2 = 0;
    int c, i, b;

    for (c = (lgth - 1), i = 0; c >= 0; c--,i++)
    {
        n2 <<= 1;

        if (num & (1 << i))
            n2 |= 1;
    }

    return n2;
}

static void
#ifdef __STDC__
reglist(char *s, unsigned long regmask, int mode)
#else
reglist (s, regmask, mode)
    char *s;
    unsigned long regmask;
    int mode;
#endif
{
    char *t = s;

    if (mode == 4)
    {
        regmask = revbits(regmask, 16);
    }

    s = regbyte (s, regmask >> 8, "A", 0);
    s = regbyte (s, regmask & 0xff, "D", s != t);

    if (s == t)
    {
        strcpy (s, "0");
    }
}

int
#ifdef __STDC__
movem_cmd(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
movem_cmd(ci, j, op)
    CMD_ITMS *ci;
    int j;
    OPSTRUCTURE *op;
#endif
{
    int mode = (ci->cmd_wrd >> 3) & 7;
    int reg = ci->cmd_wrd & 7;
    int size = (ci->cmd_wrd & 0x40) ? SIZ_LONG : SIZ_WORD;
    char ea[50];
    char regnames[50];
    int dir = (ci->cmd_wrd >> 10) & 1;
    int regmask;

    if (mode < 2)       /* Common to both modes */
    {
        return 0;
    }

    if (dir)
    {
        if (mode == 4)
            return 0;
        if ((mode == 7) && (reg == 4))
            return 0;
    }
    else
    {
        if (mode == 3)
            return 0;
        if ((mode == 7) && (reg > 1))
            return 0;
    }

    get_eff_addr(ci, ea, mode, reg, size);
    regmask = getnext_w(ci);
    reglist (regnames, regmask, mode);
    strcpy (ci->mnem, op->name);
    strcat (ci->mnem, SizSufx[size]);

    if (dir)
    {
        sprintf(ci->opcode, "%s,%s", ea, regnames);
    }
    else
    {
        sprintf(ci->opcode, "%s,%s", regnames, ea);
    }

    return 1;
}

int
#ifdef __STDC__
link_unlk(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
link_unlk(ci, j, op)
    CMD_ITMS *ci;
    int j;
    OPSTRUCTURE *op;
#endif
{
    int regno = ci->cmd_wrd & 7;
    int ext_w;

    strcpy (ci->mnem, op->name);

    switch (j)
    {
        case 47:        /* "unlink: only needs regno for the opcode */
            sprintf (ci->opcode, "A%d", regno);

            if ((ci->mnem[strlen(ci->mnem) - 1]) == '.')
            {
               ci->mnem[strlen(ci->mnem) - 1] = '\0';
            }

            break;
        default:
            ext_w = getnext_w(ci);

            if (j == 138)
            {
                ext_w = (ext_w << 8) | (getnext_w(ci) & 0xff);
            }

            sprintf (ci->opcode, "A%d,#%d", regno, ext_w);
            strcat(ci->mnem, (j == 46) ? "w" : "l");
    }

    return 1;
}
