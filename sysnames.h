/* ******************************************************************** *
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
 * ******************************************************************** */

char *SysNames[] = {
    "F$Link", /* $00*/
    "F$Load", /* $01*/
    "F$UnLink", /* $02*/
    "F$Fork", /* $03*/
    "F$Wait", /* $04*/
    "F$Chain", /* $05*/
    "F$Exit", /* $06*/
    "F$Mem", /* $07*/
    "F$Send", /* $08*/
    "F$Icpt", /* $09*/
    "F$Sleep", /* $0a*/
    "F$SSpd", /* $0b*/
    "F$ID", /* $0c*/
    "F$SPrior", /* $0d*/
    "F$STrap", /* $0e*/
    "F$PErr", /* $0f*/
    "F$PrsNam", /* $10*/
    "F$CmpNam", /* $11*/
    "F$SchBit", /* $12*/
    "F$AllBit", /* $13*/
    "F$DelBit", /* $14*/
    "F$Time", /* $15*/
    "F$STime", /* $16*/
    "F$CRC", /* $17*/
    "F$GPrDsc", /* $18*/
    "F$GBlkMp", /* $19*/
    "F$GModDr", /* $1a*/
    "F$CpyMem", /* $1b*/
    "F$SUser", /* $1c*/
    "F$UnLoad", /* $1d*/
    "F$RTE", /* $1e*/
    "F$GPrDBT", /* $1f*/
    "F$Julian", /* $20*/
    "F$TLink", /* $21*/
    "F$DFork", /* $22*/
    "F$DExec", /* $23*/
    "F$DExit", /* $24*/
    "F$DatMod", /* $25*/
    "F$SetCRC", /* $26*/
    "F$SetSys", /* $27*/
    "F$SRqMem", /* $28*/
    "F$SRtMem", /* $29*/
    "F$IRQ", /* $2a*/
    "F$IOQu", /* $2b*/
    "F$AProc", /* $2c*/
    "F$NProc", /* $2d*/
    "F$VModul", /* $2e*/
    "F$FindPD", /* $2f*/
    "F$AllPD", /* $30*/
    "F$RetPD", /* $31*/
    "F$SSvc", /* $32*/
    "F$IODel", /* $33*/
    "",
    "",
    "",

    "F$GProcP", /* $37*/
    "F$Move", /* $38*/
    "F$AllRAM", /* $39*/
    "F$Permit", /* $3a*/
    "F$Protect", /* $3b*/
    "F$SetImg", /* $3c*/
    "F$FreeLB", /* $3d*/
    "F$FreeHB", /* $3e*/
    "F$AllTsk", /* $3f*/
    "F$DelTsk", /* $40*/
    "F$SetTsk", /* $41*/
    "F$ResTsk", /* $42*/
    "F$RelTsk", /* $43*/
    "F$DATLog", /* $44*/
    "F$DATTmp", /* $45*/
    "F$LDAXY", /* $46*/
    "F$LDAXYP", /* $47*/
    "F$LDDDXY", /* $48*/
    "F$LDABX", /* $49*/
    "F$STABX", /* $4a*/
    "F$AllPrc", /* $4b*/
    "F$DelPrc", /* $4c*/
    "F$ELink", /* $4d*/
    "F$FModul", /* $4e*/
    "F$MapBlk", /* $4f*/
    "F$ClrBlk", /* $50*/
    "F$DelRAM", /* $51*/
    "F$SysDbg", /* $52*/
    "F$Event", /* $53*/
    "F$Gregor", /* $54*/
    "F$SysID", /* $55*/
    "F$Alarm", /* $56*/
    "F$SigMask", /* $57*/
    "F$ChkMem", /* $58*/
    "F$UAcct", /* $59*/
    "F$CCtl", /* $5a*/
    "F$GSPUMp", /* $5b*/
    "F$SRqCMem", /* $5c*/
    "F$POSK", /* $5d*/
    "F$Panic", /* $5e*/
    "F$MBuf", /* $5f*/
    "F$Trans", /* $60*/
    "", /* $61*/
    "", /* $62*/
    "", /* $63*/
    "", /* $64*/
    "", /* $65*/
    "", /* $66*/
    "", /* $67*/
    "", /* $68*/
    "", /* $69*/
    "", /* $6a*/
    "", /* $6b*/
    "", /* $6c*/
    "", /* $6d*/
    "", /* $6e*/
    "", /* $6f*/
    "", /* $70*/
    "", /* $71*/
    "", /* $72*/
    "", /* $73*/
    "", /* $74*/
    "", /* $75*/
    "", /* $76*/
    "", /* $77*/
    "", /* $78*/
    "", /* $79*/
    "", /* $7a*/
    "", /* $7b*/
    "", /* $7c*/
    "", /* $7d*/
    "", /* $7e*/
    "", /* $7f*/

    /*"I$Org",         $80 */
    "I$Attach", /* $80*/
    "I$Detach", /* $81*/
    "I$Dup", /* $82*/
    "I$Create", /* $83*/
    "I$Open", /* $84*/
    "I$MakDir", /* $85*/
    "I$ChgDir", /* $86*/
    "I$Delete", /* $87*/
    "I$Seek", /* $88*/
    "I$Read", /* $89*/
    "I$Write", /* $8a*/
    "I$ReadLn", /* $8b*/
    "I$WritLn", /* $8c*/
    "I$GetStt", /* $8d*/
    "I$SetStt", /* $8e*/
    "I$Close", /* $8f*/
    "",
    "",

    "I$SGetSt", /* $92*/
    "I$Last", /* $93*/
};

char *MathCalls[] = {
    "T$LMul",     /* $00*/
    "T$UMul",     /* $01*/
    "T$LDiv",     /* $02*/
    "T$LMod",     /* $03*/
    "T$UDiv",     /* $04*/
    "T$UMod",     /* $05*/
    "T$FAdd",     /* $06*/
    "T$FInc",     /* $07*/
    "T$FSub",     /* $08*/
    "T$FDec",     /* $09*/
    "T$FMul",     /* $0a*/
    "T$FDiv",     /* $0b*/
    "T$FCmp",     /* $0c*/
    "T$FNeg",     /* $0d*/
    "T$DAdd",     /* $0e*/
    "T$DInc",     /* $0f*/
    "T$DSub",     /* $10*/
    "T$DDec",     /* $11*/
    "T$DMul",     /* $12*/
    "T$DDiv",     /* $13*/
    "T$DCmp",     /* $14*/
    "T$DNeg",     /* $15*/
    "T$AtoN",     /* $16*/
    "T$AtoL",     /* $17*/
    "T$AtoU",     /* $18*/
    "T$AtoF",     /* $19*/
    "T$AtoD",     /* $1a*/
    "T$LtoA",     /* $1b*/
    "T$UtoA",     /* $1c*/
    "T$FtoA",     /* $1d*/
    "T$DtoA",     /* $1e*/
    "T$LtoF",     /* $1f*/
    "T$LtoD",     /* $20*/
    "T$UtoF",     /* $21*/
    "T$UtoD",     /* $22*/
    "T$FtoL",     /* $23*/
    "T$DtoL",     /* $24*/
    "T$FtoU",     /* $25*/
    "T$DtoU",     /* $26*/
    "T$FtoD",     /* $27*/
    "T$DtoF",     /* $28*/
    "T$FTrn",     /* $29*/
    "T$DTrn",     /* $2a*/
    "T$FInt",     /* $2b*/
    "T$DInt",     /* $2c*/
    "T$DNrm",     /* $2d*/
    "T$Sin",      /* $2e*/
    "T$Cos",      /* $2f*/
    "T$Tan",      /* $30*/
    "T$Asn",      /* $31*/
    "T$Acs",      /* $32*/
    "T$Atn",      /* $33*/
    "T$Log",      /* $34*/
    "T$Log10",    /* $35*/
    "T$Sqrt",     /* $36*/
    "T$Exp",      /* $37*/
    "T$Power"     /* $38*/
};

