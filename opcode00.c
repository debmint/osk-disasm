/* ********************************************************************************* *
 * opcode00.c - Resolves cases where the Opcode is 0000                              *
 * ********************************************************************************* */

#include "disglobs.h"
#include <string.h>

int 
#ifdef __STDC__
    opcode0000 (CMD_ITMS *cmditms)
#else
    opcode0000 (struct cmditems *cmdims)

    struct cmditems *cmditms;
#endif
{
    register int firstword = cmditms->code[0];    /* To save calculations */

    switch ((firstword >> 8) & 0x0f)
    {
    case 0:
        if ((firstword & 0x1c) == 0x0c)    /* "cmp2" or "chk2"? */
        {
            int mode,
                reg;
            short w2;

            mode = (firstword >> 3) & 0x07;

            if ((mode < 3) || (mode == 3) || (mode == 5))
            {
                break;   /* Illegal, try something else */
            }

            reg = firstword & 0x07;
            
            if ((mode == 3) && ((reg == 2) || (reg == 3)))
            {
                break;
            }

            w2 = getnext_w(cmditms);

            if ((w2 & 0xfff) == 0)
            {
                strcpy(cmditms->mnem, "cmp2");
            }
            else if ((w2 & 0xfff) == 0x800)
            {
                strcpy(cmditms->mnem, "chk2");
            }
            else
            {
                unget_w(cmditms);
                break;
            }
        }

        strcpy(cmditms->mnem, "ori");
        /* Eliminate ccr & SR */
        /* Go process extended command */
        break;
    case 0x01:
        strcpy(cmditms->mnem, "andi");
        /* Eliminate ccr & SR */
        /* Go process extended command */
        break;
    case 0x02:
        if (firstword & 0x80 == 0)
        {
            strcpy(cmditms->mnem, "subi");
            break;
        }
        /* Go process extended command */
    case 0x03:
        if ((firstword & 0xc0) == 0xc0)
        {
            if ((firstword & 0xf0) == 0xc0)
            {
                strcpy(cmditms->mnem, "rtm");
            }
            else
            {
                strcpy(cmditms->mnem, "callm");
            }

            break;
        }
        else
        {
            strcpy(cmditms->mnem, "addi");
            /* Standard extended addressing mode */
        }

        break;
    case 0x6:

        if ((firstword & 0xc0) == 0xc0)
        {
            strcpy(cmditms->mnem, "callm");
            /* special case */
        }
        else
        {
            strcpy(cmditms->mnem, "addi");
            /* Generic mode get */
        }
        break;
    case 0x0a:
        strcpy(cmditms->mnem, "eori");
        /* Eliminate ccr & SR */
        /* Go process extended command */
        break;
    case 0x0c:
        /* Process standard extended mode */
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
    }

    if ((firstword & 0x38) == 0x08)
    {
        strcpy(cmditms->mnem, "movep");
    }

    return 0;
}

/* Build the "move"/"movea" commands */

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
    char src_ea[20], dst_ea[20];

    /* Get Destination EA */
    d_mode = (ci->code[0] >> 6) & 7;
    d_reg = (ci->code[0] >>9) & 7;

    if ((d_mode == 7) && (d_mode > 1))
    {
        return 0;
    }

    src_mode = (ci->code[0] >> 3) & 7;
    src_reg = ci->code[0] & 7;

    if (get_eff_addr (ci, dst_ea, d_mode, d_reg))
    {
        if (get_eff_addr (ci, src_ea, src_mode, src_reg))
        {
            sprintf(ci->opcode, "%s,%s", src_ea, dst_ea);
            strcpy (ci->mnem, "move");

            if (((ci->code[0] >> 9) & 7) == 1)
            {
                strcat(ci->mnem, "a");
            }
            else
            {
                switch ((ci->code[0] >>12) & 3)
                {
                case 1:
                    strcat(ci->mnem, ".b");
                    break;
                case 3:
                    strcat(ci->mnem, ".w");
                    break;
                case 2:
                    strcat(ci->mnem, ".l");
                }
            }

            return ci->opcode;
        }
    }

    return 0;
}