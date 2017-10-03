/* ******************************************************************** *
 * opcodes020.c - This file primarily targets code for the 68020 and    *
 *            higher                                                    *
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

#include "disglobs.h"
#include "userdef.h"
#include <string.h>
#include "proto.h"

extern CONDITIONALS typecondition[];
/*
 * ewReg() - Retrieves the register name and number from the extended word
 * Returns: The string containing the Regname and number
 */

static char *
#ifdef __STDC__
ewReg(int extWrd, char *dst)
#else
ewReg(extWrd, dst)
    int extWrd; char *dst;
#endif
{
    sprintf (dst, "%c%d", dispRegNam[(extWrd >> 15) & 1], (extWrd >> 12) & 7);
    return dst;
}

int
#ifdef __STDC__
cmp2_chk2(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
cmp2_chk2(ci, j, op)
    CMD_ITMS *ci;
    int j;
    OPSTRUCTURE *op;
#endif
{
    int w2;
    int mode, reg;
    int size;

    mode = ((ci->cmd_wrd) >> 3) & 0x07;

    if ((mode < 2) || (mode == 3) || (mode == 4))
    {
        return 0;   /* Illegal, try something else */
    }

    reg = (ci->cmd_wrd) & 0x07;
    
    if ((mode == 7) && (reg == 4))
    {
        return 0;
    }

    w2 = getnext_w(ci);

    switch (w2 & 0xfff)
    {
    case 0:
    case 0x800:
        if (Pass == 2)
        {
            strcpy(ci->mnem, op->name);
        }

        break;
    default:
        ungetnext_w (ci);
        return 0;
    }

    size = (ci->cmd_wrd >> 9) & 3;
    EaString[0] = '\0';

    if (get_eff_addr(ci, EaString, mode, reg, size))
    {
        if (Pass == 2)
        {
            char dispreg[5];
            
            strcat (ci->mnem, stdSiz[size]);
            sprintf(ci->opcode, "%s,%c%d", EaString, ewReg (w2, dispreg));
        }

        return 1;
    }

    return 0;      /* Until we finish the function */
}

int
#ifdef __STDC__
rtm_020(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
rtm_020(ci, j, op)
    CMD_ITMS *ci; int j; OPSTRUCTURE *op;
#endif
{
    sprintf (ci->opcode, "%c%d", dispRegNam[(ci->cmd_wrd >> 3) & 1], ci->cmd_wrd & 7);
    strcpy(ci->mnem, op->name);
    return 1;
}

int
#ifdef __STDC__
cmd_moves(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
cmd_moves(ci, j, op)
    CMD_ITMS *ci; int j; OPSTRUCTURE *op;
#endif
{
    int mode, reg;
    int ew,
        size;

    if (ci->cmd_wrd & 0xff00)
    {
        return 0;
    }

    mode = (ci->cmd_wrd >> 3) & 7;

    if ( mode < 2)
    {
        return 0;
    }

    reg = ci->cmd_wrd & 7;

    if ((mode == 7) && (reg > 1))
    {
        return 0;
    }

    ew = getnext_w(ci);

    if (ew & 0x07ff)
    {
        ungetnext_w(ci);
        return 0;
    }

    size = (ci->cmd_wrd >> 6) & 7;

    if (get_eff_addr (ci, EaString, mode, reg, size))
    {
        if (Pass == 2)
        {
            char myReg[5];

            strcpy (ci->mnem, op->name);
            strcat (ci->mnem, stdSiz[size]);
            ewReg (ew, myReg);  /* Get the register involved */

            switch ((ew >> 11) & 1)
            {
            case 0:
                sprintf (ci->opcode, "%s,%s", EaString, myReg);
                break;
            case 1:
                sprintf (ci->opcode, "%s,%s", myReg, EaString);
            }
        }

        return 1;
    }

    return 0;
}

int
#ifdef __STDC__
cmd_cas (CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
cmd_cas (ci, j, op)
    CMD_ITMS *ci; int j; OPSTRUCTURE *op;
#endif
{
    int reg;
    int size;
    int mode = (ci->cmd_wrd >> 3) & 7;
    int dc, du;
    int ew = getnext_w(ci);

    if ( mode < 2)
    {
        return 0;
    }

    reg = ci->cmd_wrd & 7;

    if ((mode == 7) && reg > 1)
    {
        return 0;
    }

    if (ew & 0xfe38)
    {
        ungetnext_w(ci);
        return 0;
    }

    size  = (ci->cmd_wrd >> 9) & 3;

    if (get_eff_addr (ci, EaString, mode, reg, size))
    {
        if (Pass == 2)
        {
            dc = ew & 7;
            du = (ew >> 6) & 7;
            strcpy (ci->mnem, op->name);
            strcat (ci->mnem, stdSiz[size]);
            sprintf (ci->opcode, "D%d,D%d,%s", dc, du, EaString);
        }

        return 1;
    }

    ungetnext_w(ci);
    return 0;
}

/*
 * getcas2EW() - Retrieves the data for one of the "cas2" extended words
 * Returns: the character name of the register (either "d" or "a").
 */

static char
#ifdef __STDC__
getcas2Ew(CMD_ITMS *ci, int *dc, int *du, int *rn)
#else
getcas2Ew(ci, dc, du, rn)
    CMD_ITMS *ci; int *dc; int *du; int *rn;
#endif
{
    int ew = getnext_w(ci);

    if (ew & 0xfe38)
    {
        ungetnext_w(ci);
        return 0;
    }

    *dc = ew & 7;
    *du = (ew >> 6) & 7;
    *rn = (ew >> 12) & 7;
    return dispRegNam[(ew >> 15) & 1];
}

int
#ifdef __STDC__
cmd_cas2 (CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
cmd_cas2 (ci, j, op)
    CMD_ITMS *ci; int j; OPSTRUCTURE *op;
#endif
{
    int size;
    int dc1, dc2, du1, du2, rn1, rn2;
    char r1, r2;

    size = ((ci->cmd_wrd >> 9) & 3) - 1;

    if (size < 0)
    {
        return 0;
    }

    if (!(r1 = getcas2Ew (ci, &dc1, &du1, &rn1)))
    {
        return 0;
    }

    if (!(r2 = getcas2Ew (ci, &dc2, &du2, &rn2)))
    {
        return 0;
    }

    if (Pass == 2)
    {
        sprintf (ci->mnem, op->name);
        strcat (ci->mnem, stdSiz[size]);
        sprintf (ci->opcode, "d%d:d%d,d%d:d%d,(%c%d):(%c%d)", dc1, dc2, du1, du2, r1, rn1, r2, rn2);
    }

    return 1;
}

int
#ifdef __STDC__
cmd_callm(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
cmd_callm(ci, j, op)
    CMD_ITMS *ci; int j; OPSTRUCTURE *op;
#endif
{
    int mode, reg;
    int ew;

    mode = (ci->cmd_wrd >> 3) & 7;

    if ((mode < 2) || (mode == 3) || (mode == 4))
    {
        return 0;
    }

    reg = ci->cmd_wrd & 7;

    if ((mode == 7) && (reg == 4))
    {
        return 0;
    }

    ew = getnext_w(ci);

    if (ew & 0xff00)
    {
        ungetnext_w(ci);
        return 0;
    }

    if (get_eff_addr (ci, EaString, mode, reg, 0))
    {
        if (ew & 0x80)
        {
            ew &= (-1 ^ 0xff);
        }

        /* Possibly allow for label name ??? */
        sprintf (ci->mnem, op->name);
        sprintf (ci->opcode, "#%d,%s", ew, EaString);
        return 1;
    }

    return 0;
}

int
#ifdef __STDC__
muldiv_020(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
muldiv_020(ci, j, op)
    CMD_ITMS *ci; int j; OPSTRUCTURE *op;
#endif
{
    int mode, reg;
    int ew;
    int d_hr, d_lq;

    mode = (ci->cmd_wrd >> 3) & 7;

    if (mode == 1)
    {
        return 0;
    }

    ew = getnext_w(ci);

    if (ew & 0x83f8)
    {
        ungetnext_w(ci);
        return 0;
    }

    d_hr = ew & 7;
    d_lq = (ew >> 12) & 7;

    /* The following condition causes an undefined result,
     * so abort
     */

    if ((d_hr == d_lq) && (ew & 0x0400))
    {
        ungetnext_w(ci);
        return 0;
    }

    reg = ci->cmd_wrd & 7;

    if (get_eff_addr(ci, EaString, mode, reg, 0))
    {
        if (Pass == 2)
        {
            /* Set mnem */
            strcpy (ci->mnem, op->name);
            strcat (ci->mnem, (ci->cmd_wrd & 0x80) ? "s" : "u");

            if (d_hr != d_lq)
            {
                if (!(ew & 0x0400))
                {
                    strcat (ci->mnem, "l");
                }

                sprintf (ci->opcode, "%s,d%d:d%d", EaString, d_hr, d_lq);
            }
            else
            {
                sprintf (ci->opcode, "%s,d%d", EaString, d_hr);
            }

            strcat (ci->mnem, ".l");
        }

        return 1;
    }

    ungetnext_w(ci);
    return 0;
}

int
#ifdef __STDC__
cmd_rtd(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
cmd_rtd(ci, j, op)
    CMD_ITMS *ci; int j; OPSTRUCTURE *op;
#endif
{
    int ew = getnext_w(ci);

    if (Pass == 2)
    {
        strcpy (ci->mnem, op->name);
        sprintf (ci->opcode, "#%d", ew);
    }

    return 1;
}

int
#ifdef __STDC__
cmd_trapcc(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
cmd_trapcc(ci, j, op)
    CMD_ITMS *ci; int j; OPSTRUCTURE *op;
#endif
{
    int opmode = ci->cmd_wrd & 7;
    int oprnd;

    if (opmode < 2)
    {
        return 0;
    }

    if (opmode < 4)
    {
        oprnd = getnext_w(ci);

        if (opmode == 3)
        {
            oprnd = (oprnd << 15) | getnext_w(ci);
        }

        if (Pass == 2)
        {
            sprintf (ci->opcode, "#%d", oprnd);
        }
    }
    else
    {
        ci->opcode[0] = '\0';
    }

    if (Pass == 2)
    {
        strcpy (ci->mnem, op->name);
        strcpy (strchr(ci->mnem, '~'), typecondition[(ci->cmd_wrd >> 8) & 0x0f].condition);

        switch (opmode)
        {
        case 2:
            strcat (ci->mnem, ".w");
            break;
        case 3:
            strcat (ci->mnem, ".l");
            break;
        }
    }

    return 1;
}

static char *bfty0[] = {"tst", "chg", "clr", "set"};
static char *bfty1[] = {"extu", "exts", "ffo","ins"};

static char *
#ifdef __STDC__
getbf_fld(char *dst, int flg, int val)
#else
getbf_fld(dst, flg, val)
    char *dst; int flg; int val;
#endif
{
    if (flg)
    {
        sprintf (dst, "d%d", val);
    }
    else
    {
        sprintf(dst, "%d", val);
    }

    return dst;
}

/*
 * bitfields_020() - Handles the bitfield functions
 */

int
#ifdef __STDC__
bitfields_020(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
bitfields_020(ci, j, op)
    CMD_ITMS *ci; int j; OPSTRUCTURE *op;
#endif
{
    int mode, reg;
    int v;   /* Generic value */
    int fld8;
    char *bitfunc;
    int ew;
    int Dw, Do, offset, width;
    char ofstStr[20], wdthStr[20];

    mode = (ci->cmd_wrd >>3) & 7;

    if ((mode == 1) || (mode == 3) || (mode == 4))
    {
        return 0;
    }

    reg = ci->cmd_wrd & 7;
    fld8 = (ci->cmd_wrd >> 8) & 1;
    v = (ci->cmd_wrd >> 9) & 3;

    if (mode == 0x111)
    {
        if (reg == 4)
        {
            return 0;
        }

        if (fld8 && (v == 3) && (reg > 1))
        {
            return 0;
        }
        else if (v && (reg > 1))
        {
            return 0;
        }
    }

    bitfunc = fld8 ? bfty1[v] : bfty0[v];
    ew = getnext_w(ci);

    if (v)
    {
        if (ew & 0x8000)
        {
            ungetnext_w(ci);
            return 0;
        }
    }
    else
    {
        if (ew & 0xf000)
        {
            ungetnext_w(ci);
            return 0;
        }
    }

    if (get_eff_addr(ci, EaString, mode, reg, 0))
    {
        /* Get the fields common to all functions */
        Do = (ew >> 11) & 1;
        offset = (ew >> 6) & 0x1f;

        if (Do && (offset & 0x18))
        {
            ungetnext_w(ci);
            return 0;
        }

        Dw = (ew >> 5) & 1;
        width = ew & 0x1f;

        if (Dw && (width & 0x18))
        {
            ungetnext_w(ci);
            return 0;
        }

        getbf_fld(wdthStr, Dw, width);
        getbf_fld(ofstStr, Do, offset);

        if (Pass == 2)
        {
            strcpy (ci->mnem, op->name);
            strcpy (strchr (ci->mnem, '~'), bitfunc);

            if (fld8)
            {
                if ((ci->cmd_wrd & 0xffc0) == 0xefc0)
                {
                    sprintf (ci->opcode, "d%d,%s{%s:%s}", (ew >> 12) & 7,
                                EaString, ofstStr, wdthStr);
                }
                else
                {
                    sprintf (ci->opcode, "%s{%s:%s},d%d", EaString,
                                ofstStr, wdthStr, (ew >> 12) & 7);
                }
            }
            else
            {
                sprintf (ci->opcode, "%s{%s:%s}", EaString, ofstStr, wdthStr);
            }
        }

        return 1;
    }

    ungetnext_w(ci);
    return 0;
}
