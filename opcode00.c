/* ************************************************************************ *
 * opcode00.c - Resolves cases where the Opcode is 0000                     $
 *                                                                          $
 * $Id::                                                                    $
 * ************************************************************************ */

#include "disglobs.h"
#include <string.h>

int
#ifdef __STDC__
bitModeRegLegal(int mode, int reg)
#else
bitModeRegLegal(mode, reg)
    int mode;
    int reg;
#endif
{
    if ( ((2 ^ mode) & 0x7d) == 0)
    {
        return 0;
    }

    if (mode == 6)
    {
        if ( ((2 ^ reg) & 3) == 0)
        {
            return 0;
        }
    }
}

int 
#ifdef __STDC__
    bit_movep_immediate (CMD_ITMS *cmditms)
#else
    bit_movep_immediate (CMD_ITMS *cmdims)

    CMD_ITMS *cmditms;
#endif
{
    register int firstword = cmditms->cmd_wrd;    /* To save calculations */
    short ext1, ext2;
    int mode,reg;

    /* 68020+ code
    if ((firstword & 0xfff0) == 0x6c)  // "rtm"
    {
        //if (cpu >= 2)
        {
            return rtm_020(cmditms);
        }
       // else
            //return 0;
    }*/

    switch (firstword)
    {
    case 0x3c:
    case 0x7c:
        return biti_reg(cmditms, "ori");
    case 0x023c:
    case 0x027c:
        return biti_reg(cmditms, "andi");
    case 0x0a3c:
    case 0x0a7c:
        return biti_reg(cmditms, "eori");
    default:
        break;
    }

    /* Handle static bit cmds */

    switch (firstword & 0xffd0)
    {
    case 0x0800:
        return bit_static(cmditms, "btst");
    case 0x0840:
        return bit_static(cmditms, "bchg");
    case 0x0880:
        return bit_static(cmditms, "bclr");
    case 0x8a0:
        return bit_static(cmditms, "bset");
    }

    /* Dynamic bit commands */

    switch ((firstword >> 6) & 7)
    {
        case 4:
            if (bit_dynamic(cmditms, "btst"))
            {
                return 1;
            }
            break;
        case 5:
            if (bit_dynamic(cmditms, "bchg"))
            {
                return 1;
            }
            break;
        case 6:
            if (bit_dynamic(cmditms, "bclr"))
            {
                return 1;
            }
            break;
        case 7:
            if (bit_dynamic(cmditms, "bset"))
            {
                return 1;
            }
            break;

    }

    switch ((firstword >> 8) & 0x0f)
    {
    case 0:
        if (cpu >= 2)
        {
            if ((firstword & 0x1c) == 0x0c)    /* "cmp2" or "chk2"? */
            {
                /* For the time being, if this is not a match, simply
                 * continue */

                if (cmp2_chk2(cmditms))
                    return 1;
            }
        }

        return (biti_size(cmditms, "ori"));
        break;
    case 0x01:
        return (biti_size(cmditms, "andi"));
        /* Eliminate ccr & SR */
        break;
    case 0x02:
        return (biti_size(cmditms, "ori"));
    case 0x03:
        if ((firstword & 0x1f0) == 0xf0)
        {
            if (cpu > 2)
            {
                /* Process rtm, callm, cmp2, chk2 */
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return (biti_size(cmditms, "andi"));
        }

        break;
    case 4:
        break;
    case 0x6:

        if ((firstword & 0xc0) == 0xc0)
        {
            strcpy(cmditms->mnem, "callm");
            /* special case */
        }
        else
        {
            return biti_size(cmditms, "addi");
        }
        break;
    case 0x0a:
        strcpy(cmditms->mnem, "eori");
        /* Eliminate ccr & SR */
        /* Go process extended command */
        break;
    case 0x0c:
        return biti_size(cmditms, "cmpi");
        break;
    case 0x08:
        {
            switch ((firstword >> 6) & 3)
            {
            case 0:
                strcpy(cmditms->mnem, "btst");
                break;
            case 1:
                strcpy(cmditms->mnem, "bchg");
                break;
            case 2:
                strcpy(cmditms->mnem, "bclr");
                break;
            case 3:
                strcpy(cmditms->mnem, "bset");
                break;
            }
        }
    case 0x0ff:
        strcpy(cmditms->mnem, "moves");
        break;
    }

    /* We need to now check a different set of bits */
    switch ((firstword >>6) & 0x07)
    {
    case 3:
        if (firstword & 0x800)
        {
            if ((firstword & 0x3f) == 0x3d)
            {
                strcpy(cmditms->mnem, "cas");
                break;
            }
            else
            {
                /* Possible tests needed */
                strcpy(cmditms->mnem, "cas");
                break;
            }
        }

        break;
    case 0x4:
        strcpy(cmditms->mnem, "btst");
        break;
    case 5:
        strcpy(cmditms->mnem, "bchg");
        break;
    case 6:
        strcpy(cmditms->mnem, "bclr");
        break;
    case 7:
        strcpy(cmditms->mnem, "bset");
        break;
    case 10:
        ext1 = getnext_w(cmditms);
        /* Eliminate ccr & SR */
        switch (cmditms->cmd_wrd & 0xff)
        {
            int regflag = cmditms->cmd_wrd & 0x40; /* 0 if ccr, 1 if sr */

        case 0x3c:
        case 0x7c:
            /* If it's ccr and it's negative, sign extend it */
            if ((regflag == 0) && ( ext1 & 0x80))
            {
                ext1 |= 0xffff0000;
            }

            /* Verify if it's a label */
            if (regflag == 0)
            {
                sprintf(cmditms->opcode, "#%s,ccr", ext1);
            }
            else
            {
                sprintf (cmditms->opcode, "#%s,sr", ext1);
            }

            strcpy(cmditms->mnem,"eori");
            return 1;
        default:
            strcpy (cmditms->mnem, "eori");
        }
    }

    if ((firstword & 0x38) == 0x08)
    {
        strcpy(cmditms->mnem, "movep");
    }


    return 0;
}

/*
 * Immediate bit operations involving the status registers
 * Returns 1 on success, 0 on failure
 *
 */

int
#ifdef __STDC__
biti_reg(CMD_ITMS *ci, char *mnem)
#else
biti_reg(ci, mnem)
    CMD_ITMS *ci;
    char *mnem;
#endif
{
    static char *sr[2] = {"ccr", "sr"};
    register int size = (ci->code[1] >> 6) & 1;

    ci->code[1] = getnext_w(ci);

    if (ci->code[1] & 0xff00)
    {
        ungetnext_w(ci);
        return 0;
    }

    /* Add functions to retrieve label */
    if (Pass == 2) {
        strcpy (ci->mnem, mnem);
        sprintf (ci->opcode, "#%d,%s", ci->code[1], sr[size]);
        return 1;
    }
}

/*
 * Immediate bit operations not involving status registers
 *
 */

int
#ifdef __STDC__
biti_size(CMD_ITMS *ci, char *mnem)
#else
biti_size(ci, mnem)
    CMD_ITMS *ci;
    char *mnem;
#endif
{
    register int size = (ci->cmd_wrd >> 6) & 3;
    register int mode = (ci->cmd_wrd >> 3) & 7;
    register int reg;
    register int firstword = ci->wcount;
    char ea[30];
    long data;

    if (size > SIZ_LONG)
    {
        return 0;
    }

    if (mode == 1)
    {
        return 0;
    }
   
    reg = (ci->cmd_wrd) & 7;

    if ((mode == 7) && (reg > 3))
    {
        return 0;
    }

    ci->code[firstword] = getnext_w(ci);

    switch (size)
    {
    case SIZ_BYTE:
    case SIZ_WORD:
        data = ci->code[firstword];
        break;
    case SIZ_LONG:
        ci->code[ci->wcount] = getnext_w(ci);
        data = (ci->code[firstword] << 8) | ci->code[firstword + 1];
        break;
    }
    if (get_eff_addr(ci, ea, mode, reg, SIZ_LONG))
    {

        /* We need to add feature to include Labels here... */
        sprintf (ci->opcode, "#%ld,%s", data, ea);
        strcpy (ci->mnem, mnem);
        return 1;
    }
    else
    {
        return 0;
    }
}

#ifdef __STDC__
bit_static(CMD_ITMS *ci, char *mnem)
#else
bit_static(ci, mnem)
    CMD_ITMS *ci;
    char *mnem;
#endif
{
    register int ext0,
                 mode, reg;
    char ea[30];

    mode = (ci->cmd_wrd >> 3) & 7;
    reg = ci->cmd_wrd & 7;

    if (bitModeRegLegal(mode, reg) == 0)
    {
        return 0;
    }


    ext0 = getnext_w(ci);

    /* The MS byte must be zero */
    if (ext0 & 0xff00)
    {
        ungetnext_w(ci);
        return 0;
    }

    /* Also the but number has limits */
    if (((mode > 1) && (ext0 > 7)) || ext0 > 0x1f)
    {
        ungetnext_w(ci);
        return 0;
    }

    strcpy(ci->mnem, mnem);

    if (get_eff_addr (ci, ea, mode, reg, (ext0 >7) ? SIZ_LONG : SIZ_BYTE))
    {
        sprintf(ci->opcode, "#%d,%s", ext0, ea);
        return 1;
    }

    return 0;
}


#ifdef __STDC__
bit_dynamic(CMD_ITMS *ci, char *mnem)
#else
bit_dynamic(ci, mnem)
    CMD_ITMS *ci;
    char *mnem;
#endif
{
    register int mode, reg;
    register int ext0;
    char ea[30];

    mode = (ci->cmd_wrd >> 3) & 7;
    reg = ci->cmd_wrd & 7;

    if (bitModeRegLegal(mode, reg) == 0)
    {
        return 0;
    }

    strcpy(ci->mnem, mnem);

    if (get_eff_addr (ci, ea, mode, reg, (ext0 >7) ? SIZ_LONG : SIZ_BYTE))
    {
        sprintf(ci->opcode,"d%d,%s", (ci->cmd_wrd >>9) & 7, ea);
        return 1;
    }

    return 0;
}

/*
 *  Build the "move"/"movea" commands
 *
 */

char *
#ifdef __STDC__
move_instr(CMD_ITMS *ci)
#else
    move_instr(ci)
    CMD_ITMS *ci;
#endif
{
    char *dst_opcode, *src_opcode;
    int d_mode, d_reg, src_mode, src_reg;
    char src_ea[50], dst_ea[50];
    register int size;
    
    /* Move instructions have size a bit different */
    switch ((ci->cmd_wrd >> 12) & 3)
    {
        case 1:
            size = SIZ_BYTE;
            break;
        case 3:
            size = SIZ_WORD;
            break;
        case 2:
            size = SIZ_LONG;
            break;
        default:
            return 0;
    }

    /* Get Destination EA */
    d_mode = (ci->cmd_wrd >> 6) & 7;
    d_reg = (ci->cmd_wrd >>9) & 7;

    if ((d_mode == 7) && (d_mode > 1))
    {
        return 0;
    }

    src_mode = (ci->cmd_wrd >> 3) & 7;
    src_reg = ci->cmd_wrd & 7;

    if (get_eff_addr (ci, src_ea, src_mode, src_reg, size))
    {
        if (get_eff_addr (ci, dst_ea, d_mode, d_reg, size))
        {
            sprintf(ci->opcode, "%s,%s", src_ea, dst_ea);
            strcpy (ci->mnem, "move");

            if (((ci->cmd_wrd >> 6) & 7) == 1)
            {
                strcat(ci->mnem, "a");
            }
            else
            {
                switch (size)
                {
                case SIZ_BYTE:
                    strcat(ci->mnem, ".b");
                    break;
                case SIZ_WORD:
                    strcat(ci->mnem, ".w");
                    break;
                case SIZ_LONG:
                    strcat(ci->mnem, ".l");
                }
            }

            return ci->opcode;
        }
    }

    return 0;
}
