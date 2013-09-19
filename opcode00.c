/* ************************************************************************ *
 * opcode00.c - Resolves cases where the Opcode is 0000                     $
 *                                                                          $
 * $Id::                                                                    $
 * ************************************************************************ */

#include <string.h>
#include "disglobs.h"
#include "userdef.h"
#include "proto.h"

char *SizSufx[] = {"b", "w", "l"};
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

//int 
//#ifdef __STDC__
//    bit_movep_immediate (CMD_ITMS *cmditms)
//#else
//    bit_movep_immediate (CMD_ITMS *cmdims)
//
//    CMD_ITMS *cmditms;
//#endif
//{
//    register int firstword = cmditms->cmd_wrd;    /* To save calculations */
//    short ext1, ext2;
//    int mode,reg;
//
//    /* 68020+ code
//    if ((firstword & 0xfff0) == 0x6c)  // "rtm"
//    {
//        //if (cpu >= 2)
//        {
//            return rtm_020(cmditms);
//        }
//       // else
//            //return 0;
//    }*/
//
//    switch (firstword)
//    {
//    case 0x3c:
//    case 0x7c:
//        return biti_reg(cmditms, "ori");
//    case 0x023c:
//    case 0x027c:
//        return biti_reg(cmditms, "andi");
//    case 0x0a3c:
//    case 0x0a7c:
//        return biti_reg(cmditms, "eori");
//    default:
//        break;
//    }
//
//    /* Handle static bit cmds */
//
//    switch (firstword & 0xffd0)
//    {
//    case 0x0800:
//        return bit_static(cmditms, "btst");
//    case 0x0840:
//        return bit_static(cmditms, "bchg");
//    case 0x0880:
//        return bit_static(cmditms, "bclr");
//    case 0x8a0:
//        return bit_static(cmditms, "bset");
//    }
//
//    /* Dynamic bit commands */
//
//    switch ((firstword >> 6) & 7)
//    {
//        case 4:
//            if (bit_dynamic(cmditms, "btst"))
//            {
//                return 1;
//            }
//            break;
//        case 5:
//            if (bit_dynamic(cmditms, "bchg"))
//            {
//                return 1;
//            }
//            break;
//        case 6:
//            if (bit_dynamic(cmditms, "bclr"))
//            {
//                return 1;
//            }
//            break;
//        case 7:
//            if (bit_dynamic(cmditms, "bset"))
//            {
//                return 1;
//            }
//            break;
//
//    }
//
//    switch ((firstword >> 8) & 0x0f)
//    {
//    case 0:
//        if (cpu >= 2)
//        {
//            if ((firstword & 0x1c) == 0x0c)    /* "cmp2" or "chk2"? */
//            {
//                /* For the time being, if this is not a match, simply
//                 * continue */
//
//                if (cmp2_chk2(cmditms))
//                    return 1;
//            }
//        }
//
//        return (biti_size(cmditms, "ori"));
//        break;
//    case 0x01:
//        return (biti_size(cmditms, "andi"));
//        /* Eliminate ccr & SR */
//        break;
//    case 0x02:
//        return (biti_size(cmditms, "ori"));
//    case 0x03:
//        if ((firstword & 0x1f0) == 0xf0)
//        {
//            if (cpu > 2)
//            {
//                /* Process rtm, callm, cmp2, chk2 */
//            }
//            else
//            {
//                return 0;
//            }
//        }
//        else
//        {
//            return (biti_size(cmditms, "andi"));
//        }
//
//        break;
//    case 4:
//        break;
//    case 0x6:
//
//        if ((firstword & 0xc0) == 0xc0)
//        {
//            strcpy(cmditms->mnem, "callm");
//            /* special case */
//        }
//        else
//        {
//            return biti_size(cmditms, "addi");
//        }
//        break;
//    case 0x0a:
//        strcpy(cmditms->mnem, "eori");
//        /* Eliminate ccr & SR */
//        /* Go process extended command */
//        break;
//    case 0x0c:
//        return biti_size(cmditms, "cmpi");
//        break;
//    case 0x08:
//        {
//            switch ((firstword >> 6) & 3)
//            {
//            case 0:
//                strcpy(cmditms->mnem, "btst");
//                break;
//            case 1:
//                strcpy(cmditms->mnem, "bchg");
//                break;
//            case 2:
//                strcpy(cmditms->mnem, "bclr");
//                break;
//            case 3:
//                strcpy(cmditms->mnem, "bset");
//                break;
//            }
//        }
//    case 0x0ff:
//        strcpy(cmditms->mnem, "moves");
//        break;
//    }
//
//    /* We need to now check a different set of bits */
//    switch ((firstword >>6) & 0x07)
//    {
//    case 3:
//        if (firstword & 0x800)
//        {
//            if ((firstword & 0x3f) == 0x3d)
//            {
//                strcpy(cmditms->mnem, "cas");
//                break;
//            }
//            else
//            {
//                /* Possible tests needed */
//                strcpy(cmditms->mnem, "cas");
//                break;
//            }
//        }
//
//        break;
//    case 0x4:
//        strcpy(cmditms->mnem, "btst");
//        break;
//    case 5:
//        strcpy(cmditms->mnem, "bchg");
//        break;
//    case 6:
//        strcpy(cmditms->mnem, "bclr");
//        break;
//    case 7:
//        strcpy(cmditms->mnem, "bset");
//        break;
//    case 10:
//        ext1 = getnext_w(cmditms);
//        /* Eliminate ccr & SR */
//        switch (cmditms->cmd_wrd & 0xff)
//        {
//            int regflag = cmditms->cmd_wrd & 0x40; /* 0 if ccr, 1 if sr */
//
//        case 0x3c:
//        case 0x7c:
//            /* If it's ccr and it's negative, sign extend it */
//            if ((regflag == 0) && ( ext1 & 0x80))
//            {
//                ext1 |= 0xffff0000;
//            }
//
//            /* Verify if it's a label */
//            if (regflag == 0)
//            {
//                sprintf(cmditms->opcode, "#%s,ccr", ext1);
//            }
//            else
//            {
//                sprintf (cmditms->opcode, "#%s,sr", ext1);
//            }
//
//            strcpy(cmditms->mnem,"eori");
//            return 1;
//        default:
//            strcpy (cmditms->mnem, "eori");
//        }
//    }
//
//    if ((firstword & 0x38) == 0x08)
//    {
//        strcpy(cmditms->mnem, "movep");
//    }
//
//
//    return 0;
//}

/*
 * Immediate bit operations involving the status registers
 * Returns 1 on success, 0 on failure
 *
 */

int
#ifdef __STDC__
biti_reg(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
biti_reg(ci, j, op)
    CMD_ITMS *ci;
    int j;
    OPSTRUCTURE *op;
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
        strcpy (ci->mnem, op->name);
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
biti_size(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
biti_size(ci, j, op)
    CMD_ITMS *ci;
    int j;
    OPSTRUCTURE *op;
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
        strcpy (ci->mnem, op->name);
        return 1;
    }
    else
    {
        return 0;
    }
}

#ifdef __STDC__
bit_static(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
bit_static(ci, j, op)
    CMD_ITMS *ci;
    int j;
    OPSTRUCTURE *op;
#endif
{
    register int ext0,
                 mode, reg;
    char ea[30];

    mode = (ci->cmd_wrd >> 3) & 7;
    reg = ci->cmd_wrd & 7;

    if (mode == 1)
        return 0;

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

    strcpy(ci->mnem, op->name);

    if (get_eff_addr (ci, ea, mode, reg, (ext0 >7) ? SIZ_LONG : SIZ_BYTE))
    {
        sprintf(ci->opcode, "#%d,%s", ext0, ea);
        return 1;
    }

    return 0;
}


#ifdef __STDC__
bit_dynamic(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
bit_dynamic(ci, j, op)
    CMD_ITMS *ci;
    int j;
    OPSTRUCTURE *op;
#endif
{
    register int mode, reg;
    register int ext0;
    char ea[30];

    mode = (ci->cmd_wrd >> 3) & 7;
    reg = ci->cmd_wrd & 7;

    if (mode == 1)
        return 0;

//    switch (j)
//    {
//        case 4:
//            if (mode == 4)
//                return 0;
//        default:
//            if (mode > 1)
//                return 0;
//    }

    strcpy(ci->mnem, op->name);

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

int
#ifdef __STDC__
move_instr(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
    move_instr(ci, j, op)
    CMD_ITMS *ci;
    int j;
    OPSTRUCTURE *op;
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
    d_reg = (ci->cmd_wrd >> 9) & 7;

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

            return 1;
        }
    }

    return 0;
}

int
#ifdef __STDC__
movep(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
movep(ci, j, op)
    CMD_ITMS *ci;
    int j;
    OPSTRUCTURE *op;
#endif
{
    register int addr_reg = ci->cmd_wrd & 7;
    register int opMode = (ci->cmd_wrd >> 6) & 7;
    register int size = (opMode & 1) ? SIZ_LONG : SIZ_WORD;
    char DReg[4];
    char AReg[50];
    int disp = getnext_w(ci);
    static char opcodeFmt[8];

    strcpy (opcodeFmt, "%s,%s");
    sprintf (DReg, "D%d", (ci->cmd_wrd >> 9) & 7);
    
    sprintf (AReg, "%d(A%d)", disp, addr_reg);

    if (opMode & 2)
    {
        sprintf (ci->opcode, opcodeFmt, DReg, AReg);
    }
    else
    {
        sprintf (ci->opcode, opcodeFmt, AReg, DReg);
    }

    strcpy (ci->mnem, op->name);
    strcat( ci->mnem, (size == SIZ_LONG) ? "l" : "w");
    return 1;
}
/*28	"move.",		2,18,0, "0100000011xxxxxx",8,8, one_ea},*/
/*29	"negx.",		0,0,21, "01000000xxxxxxxx",7,6, one_ea},*/
/*32	"clr.",		0,0,21, "01000010xxxxxxxx",7,6, one_ea},*/
/*33	"move.",		2,2,17, "0100010011xxxxxx",8,8, one_ea},*/
/*34	"neg.",		0,0,21, "01000100xxxxxxxx",7,6, one_ea},*/
/*35	"move.",		2,2,18, "0100011011xxxxxx",8,8, one_ea},*/
/*36	"not.",		0,0,21, "01000110xxxxxxxx",7,6, one_ea},*/
/*37	"nbcd.",		1,0,21, "0100100000xxxxxx",6,6, one_ea},*/
/*38	"swap.",		2,4,21, "0100100001000xxx",3,3, one_ea},*/
/*39	"pea.",		5,5,21, "0100100001xxxxxx",7,7, one_ea},*/
/*42	"tas.",		1,0,21, "0100101011xxxxxx",8,8, one_ea},*/
/*43	"tst.",		0,1,21, "01001010xxxxxxxx",7,6, one_ea},*/
/*57	"jsr",			6,5,21, "0100111010xxxxxx",0,0, one_ea},*/
/*58	"jmp",			6,5,21, "0100111011xxxxxx",0,0, one_ea},*/

/* --------------------------------------------------------------- *
 * one_ea - A generic handler for when the basic command is a      *
 *      single word and the EA is in the lower 6 bytes             *
 * --------------------------------------------------------------- */

int
#ifdef __STDC__
one_ea(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
one_ea(ci, j, op)
    CMD_ITMS *ci;
    int j;
    OPSTRUCTURE *op;
#endif
{
    int mode = (ci->cmd_wrd >> 3) & 7;
    int reg = ci->cmd_wrd & 7;
    int size = (ci->cmd_wrd >>6) & 3;
    char ea[50];

    if (j == 38)
    {
        sprintf(ci->opcode, "%d%d", ci->cmd_wrd & 7);
    }
    else
    {
        /* eliminate modes */
        switch (j)
        {
            case 38:    /* swap */
            case 43:    /* tst  */
                break;      /* Allow all modes */
            case 39:    /* pea  */
            case 57:    /* jsr  */
            case 58:    /* jmp  */
                if ((2 ^ mode) & 0x0b)
                    return 0;
            default:
                if (mode == 1)
                    return 0;
        }

        /* Eliminate mode-7 regs */

        if (mode == 7)
        {
            switch (j)
            {
                case 32:        /* Allow all modes */
                case 38:
                    break;
                case 39:
                case 57:    /* jsr  */
                case 58:    /* jmp  */
                    if (reg == 4)
                        return 0;
                default:
                    if (reg > 1)
                        return 0;
            }
        }

        /* Handle opcode */

        if (get_eff_addr(ci, ea, mode, reg, size))
        {
            switch (j)
            {
                const char *statreg = "ccr";

                case 28:    /* Move from SR */
                    statreg = "sr";
                case 33:    /* Move to CCR  */
                case 35:    /* Move from CCR */
                    if (ci->cmd_wrd & 0x400)
                        sprintf (ci->opcode, "%s,%s", ea, statreg);
                    else
                        sprintf (ci->opcode, "%s,%s", statreg, ea);
                    break;
                default:    /* A single ea */
                    strcpy(ci->opcode, ea);
                    break;
            }
        }
    }

    strcpy (ci->mnem, op->name);
    return 1;
}

int
#ifdef __STDC__
bra_bsr(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
bra_bsr(ci, j, op)
    CMD_ITMS *ci;
    int j;
    OPSTRUCTURE *op;
#endif
{
    register char displ_8 = ci->cmd_wrd & 0xff;
    int displ;
    int jmp_base = PCPos;
    int jmp_to;
    char siz[4];

    switch (displ_8)
    {
        case 0:
            displ = getnext_w(ci);
            strcpy (siz, "w");
            break;
        case 1:
            displ = (getnext_w(ci) << 8) | (getnext_w(ci) & 0xff);
            strcpy (siz, "l");
            break;
        default:
            displ = displ_8;
            strcpy (siz, "b");
    }

    /* We need to calculate the address here */
    jmp_to = jmp_base + displ;
    sprintf (ci->opcode, "L%05x", jmp_to);
    strcpy (ci->mnem, op->name);
    strcat (ci->mnem, siz);

    return 1;
}

int
#ifdef __STDC__
cmd_no_opcode(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
cmd_no_opcode(ci, j, op)
    CMD_ITMS *ci;
    int j;
    OPSTRUCTURE *op;
#endif
{
    ci->opcode[0] = '\0';
    strcpy(ci->mnem, op->name);

    return 1;
}


int
#ifdef __STDC__
bit_rotate_mem(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
bit_rotate_mem(ci, j, op)
    CMD_ITMS *ci;
    int j;
    OPSTRUCTURE *op;
#endif
{
    int mode = (ci->cmd_wrd >> 3) & 7;
    int reg = ci->cmd_wrd & 7;
    char ea[50];

    if (mode < 2)
    {
        return 0;
    }

    if ((mode == 7) && (reg > 1))
    {
        return 0;
    }

    get_eff_addr(ci, ea, mode, reg, 8);
    strcpy (ci->opcode, ea);
    strcpy (ci->mnem, op->name);
    return 1;
}

int
#ifdef __STDC__
bit_rotate_reg(CMD_ITMS *ci, int j, OPSTRUCTURE *op)
#else
bit_rotate_reg(ci, j, op)
    CMD_ITMS *ci;
    int j;
    OPSTRUCTURE *op;
#endif
{
    register int count_reg = (ci->cmd_wrd >> 9) & 7;
    char dest_ea[5];

    switch ((ci->cmd_wrd >> 5) & 1)
    {
        case 0:
            sprintf (ci->opcode, "#%d,", count_reg);
            break;
        default:
            sprintf (ci->opcode, "D%d,", count_reg);
    }

    sprintf(dest_ea, "D%d", ci->cmd_wrd & 7);
    strcat (ci->opcode, dest_ea);
    strcpy (ci->mnem, op->name);
    strcat(ci->mnem, SizSufx[(ci->cmd_wrd >> 6) & 3]);
    return 1;
}
