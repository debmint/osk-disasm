/* ****************************************************************** *
 * opcodes020.c - This file primarily targets code for the 68020 and  $
 *            higher                                                  $
 *                                                                    $
 * $Id::                                                              $
 * ****************************************************************** */

#include "disglobs.h"
#include "userdef.h"
#include <string.h>
#include "proto.h"

int
#ifdef __STDC__
cmp2_chk2(CMD_ITMS *ci)
#else
cmp2_chk2(ci)
    CMD_ITMS *ci;
#endif
{
    short w2;
    int mode, reg;

    mode = ((ci->cmd_wrd) >> 3) & 0x07;

    if ((mode < 3) || (mode == 3) || (mode == 5))
    {
        return 0;   /* Illegal, try something else */
    }

    reg = (ci->cmd_wrd) & 0x07;
    
    if ((mode == 3) && ((reg == 2) || (reg == 3)))
    {
        return 0;
    }

    w2 = getnext_w(ci);

    switch (w2 & 0xfff)
    {
    case 0:
        strcpy(ci->mnem, "cmp2");
        break;
    case 0x800:
        strcpy(ci->mnem, "chk2");
        break;
    default:
        return 0;
    }
    /* Function unfinished */
    return 0;      /* Until we finish the function */
}

int
#ifdef __STDC__
rtm_020(CMD_ITMS *ci)
#else
rtm_020(ci)
    CMD_ITMS *ci;
#endif
{
    register char da = DorA[(ci->cmd_wrd >> 3)] & 1;
    strcpy(ci->mnem, "rtm");
    sprintf(ci->opcode, "%c%d", da, ci->cmd_wrd & 7);
    return 1;
}
