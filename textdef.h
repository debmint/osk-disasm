
/* ***************************************************************** */
/*
This is a file for defining strings used by the monitor. If you want
to change the prompt, an error message, etc. this is the place you
will find it.
Note, if a new string is added, then userdef's extern statement must
be changed for the commands to recognize it.
*/
/* ***************************************************************** */
 
/*
userdef.h contains the define fields. It will be included at the start
of every command, general, print, getline,main, and assembley routine.
*/
 
#include"userdef.h"
 
/*
vardef.h has the variables that are global and live in ram.
*/
 
#include"vardef.h"

/*
 * proto.h includes defs for the jump routines in the tables
*/
#include "proto.h"

/*
these are the external declerations for use by main. Any command
added/deleted should be added/deleted from here.
*/
/* Commented out by DLB 
extern dccmd(),bfcmd(),bmcmd(),brcmd(),bscmd(),hecmd(),locmd();
extern tmcmd(),rdcmd(),rmcmd(),mmcmd(),mdcmd(),gocmd();
extern trcmd(),ascmd(),sdcmd();
*/

//extern notimplemented(),bit_movep_immediate(),move_instr();
//extern biti_reg(), bit_static(), bit_dynamic(), biti_size();

/* ****************ADDITIONS*********************** */

EAFIELD seperatorfield[5];
EAFIELD actualfield[5];


#if(COPROCESSOR==TRUE || DEVICE==68040)

COPROCCONTROLREG COPROCCRreg[]=
{
	{"xxxx",		0    	}, /* entry 0  */
	{"xxxx",		0	}  /* entry 1  */

};


COPROCCONDITIONALS typepredicate[32]=
{
	{"f"},/* entry 0  */
	{"eq"},/* entry 1  */
	{"ogt"},/* entry 2  */
	{"oge"},/* entry 3  */
	{"olt"},/* entry 4  */
	{"ole"},/* entry 5  */
	{"ogl"},/* entry 6  */
	{"or"},/* entry 7  */
	{"un"},/* entry 8  */
	{"ueq"},/* entry 9  */
	{"ugt"},/* entry 10  */
	{"uge"},/* entry 11  */
	{"ult"},/* entry 12  */
	{"ule"},/* entry 13  */
	{"ne"},/* entry 14  */
	{"t"},/* entry 15  */
	{"sf"},/* entry 16  */
	{"seq"},/* entry 17  */
	{"gt"},/* entry 18  */
	{"ge"},/* entry 19  */
	{"lt"},/* entry 20  */
	{"le"},/* entry 21  */
	{"gl"},/* entry 22  */
	{"gle"},/* entry 23  */
	{"ngle"},/* entry 24  */
	{"ngl"},/* entry 25  */
	{"nle"},/* entry 26  */
	{"nlt"},/* entry 27  */
	{"nge"},/* entry 28  */
	{"ngt"},/* entry 29  */
	{"sne"},/* entry 30  */
	{"st"} /* entry 31  */

};

MONADICDYADIC MONAorDYA[]=
{
/* HARDWARE SUPPORTED */
	{"fabs",	0x18    }, /* entry 0  */
	{"fadd",	0x22    }, /* entry 1  */
	{"fcmp",	0x38    }, /* entry 2  */
	{"fdiv",	0x20    }, /* entry 3  */
	{"fmul",	0x23    }, /* entry 4  */
	{"fneg",	0x1a    }, /* entry 5  */
	{"fsqrt",	0x04    }, /* entry 6  */
	{"fsub",	0x28    }, /* entry 7  */
/* EMULATOR SUPPORTED */
	{"facos",	0x1c    }, /* entry 8  */
	{"fasin",	0x0c    }, /* entry 9  */
	{"fatan",	0x0a    }, /* entry 10  */
	{"fatanh",	0x0d    }, /* entry 11  */
	{"fcos",	0x1d    }, /* entry 12  */
	{"fcosh",	0x19    }, /* entry 13  */
	{"fetox",	0x10    }, /* entry 14  */
	{"fetoxm1",	0x08    }, /* entry 15  */
	{"fgetexp",	0x1e    }, /* entry 16  */
	{"fgetman",	0x1f    }, /* entry 17  */
	{"fint",	0x01    }, /* entry 18  */
	{"fintrz",	0x03    }, /* entry 19  */
	{"flog10",	0x15    }, /* entry 20  */
	{"flog2",	0x16    }, /* entry 21  */
	{"flogn",	0x14    }, /* entry 22  */
	{"flognp1",	0x06    }, /* entry 23  */
	{"fsin",	0x0e    }, /* entry 24  */
	{"fsinh",	0x02    }, /* entry 25  */
	{"ftan",	0x0f    }, /* entry 26  */
	{"ftanh",	0x09    }, /* entry 27  */
	{"ftentox",	0x12    }, /* entry 28  */
	{"ftwotox",	0x11    }, /* entry 29  */
	{"fmod",	0x21    }, /* entry 30  */
	{"frem",	0x25    }, /* entry 31  */
	{"fscale",	0x26    }, /* entry 32  */
	{"fsgldiv",	0x24    }, /* entry 33  */
	{"fsglmul",	0x27    }  /* entry 34  */
};

#endif/* COPROCESSOR==TRUE */


CONTROLREG CRreg[]=
{
	{"sfc",		0    }, /* entry 0  */
	{"dfc",		1    }, /* entry 1  */
	{"cacr",	2    }, /* entry 2  */
	{"usp",		0x800}, /* entry 3  */
	{"vbr",		0x801}, /* entry 4  */
	{"caar",	0x802}, /* entry 5  */
	{"msp",		0x803}, /* entry 6  */
#if (DEVICE <= 68030)
	{"isp",		0x804} /* entry 7  */
#endif
#if (DEVICE >= 68040)
	{"isp",		0x804}, /* entry 7  */
	{"tc",		0x003}, /* entry 8  */
	{"itt0",	0x004}, /* entry 9  */
	{"itt1",	0x005}, /* entry 10 */
	{"dtt0",	0x006}, /* entry 11 */
	{"dtt1",	0x007}, /* entry 12 */
	{"psr",		0x805}, /* entry 13 */
	{"urp",		0x806}, /* entry 14 */
	{"srp",		0x807}  /* entry 15 */
#endif

};

SIZETYPES sizefield[]=
{
	{"bwl~~~~"}, /* entry 0  */
	{"b~~~~~~"}, /* entry 1  */
	{"w~~~~~~"}, /* entry 2  */
	{"lbbbb~~"}, /* entry 3  */
	{"wl~~~~~"}, /* entry 4  */
	{"l~~~~~~"}, /* entry 5  */
	{"bbbbb~~"}, /* entry 6  */
	{"~wl~~~~"}, /* entry 7  */
	{"~b~~~~~"}, /* entry 8  */
	{"~w~l~~~"}, /* entry 9  */
	{"~~wl~~~"}, /* entry 10  */
	{"~~~w~~~"}, /* entry 11  */
	{"~l~~~~~"}, /* entry 12  */
	{"wwwww~~"},  /* entry 13  */
	{"~bwl~~~"},   /* entry 14  */
#if (EMULATOR==TRUE || COPROCESSOR==TRUE)
	{"lsxpwdb"},   /* entry 15  */
#else
	{"lsx~wdb"},   /* entry 15  */
#endif
	{"x~~~~~~"},   /* entry 16  */
	{"ls~~w~b"},   /* entry 17  */
	{"~~~p~~~"},   /* entry 18  */
	{"lsx~wdb"}   /* entry 19  */
};

TYPES typebitfield[8]=
{
	{"tst"}, /* entry 0  */
	{"extu"},/* entry 1  */
	{"chg"}, /* entry 2  */
	{"exts"},/* entry 3  */
	{"clr"}, /* entry 4  */
	{"ffo"}, /* entry 5  */
	{"set"}, /* entry 6  */
	{"ins"}  /* entry 7  */
};

CONDITIONALS typecondition[16]=
{
	{"t"},/* entry 0  */
	{"f"},/* entry 1  */
	{"hi"},/* entry 2  */
	{"ls"},/* entry 3  */
	{"cc"},/* entry 4  */
	{"cs"},/* entry 5  */
	{"ne"},/* entry 6  */
	{"eq"},/* entry 7  */
	{"vc"},/* entry 8  */
	{"vs"},/* entry 9  */
	{"pl"},/* entry 10  */
	{"mi"},/* entry 11  */
	{"ge"},/* entry 12 */
	{"lt"},/* entry 13 */
	{"gt"},/* entry 14 */
	{"le"} /* entry 15 */
};

EAALLOWED_TYPE EAtype[]=
{
	{0xbf8},	/* entry 0 */
	{0xbfe},	/* entry 1 */
	{0xbff},	/* entry 2 */
	{0x400},	/* entry 3 */
	{0x800},	/* entry 4 */
	{0x27e},	/* entry 5 */
	{0x2f8},	/* entry 6 */
	{0x37e},	/* entry 7 */
	{0x001},	/* entry 8 */
	{0xff8},	/* entry 9 */
	{0xfff},	/* entry 10 */
	{0x600},	/* entry 11 */
	{0x3f8},	/* entry 12 */
	{0x080},	/* entry 13 */
	{0x278},	/* entry 14 */
	{0xa78},	/* entry 15 */
	{0xa7e}, 	/* entry 16 */
	{0x1000},	/* entry 17 'C'	ccr		*/
	{0x2000},	/* entry 18 'S' status register	*/
	{0x040},	/* entry 19 */
	{0x3000},	/* entry 20 'R'	register list	*/
	{0x4000},	/* entry 21 'N'	none		*/
	{0x015},	/* entry 22 */
	{0x019},	/* entry 23 */
	{0x100},	/* entry 24 */
	{0xc00},	/* entry 25 */
	{0x5000},	/* entry 26 'U' USP		*/
	{0x6000},	/* entry 27 'Z'	control register*/
	{0x7000},	/* entry 28 'Y'	D_:D_		*/
	{0x8000},	/* entry 29 'W'	A/D_:A/D_	*/
	{0x9000},	/* entry 30 			*/
	{0xa000},	/* entry 31 			*/
	{0xb000},	/* entry 32 'X'	E{_:_}		*/
	{0xc000},	/* entry 33 			*/
	{0xd000},	/* entry 34 'Q'	melds into EA[SOURCE]		*/
	{0xe000},	/* entry 35 			*/
	{0xf000},	/* entry 36 			*/
	{0x10000},	/* entry 37 			*/
	{0x11000},	/* entry 38 'F'	function code	*/
	{0x12000},	/* entry 39 'M'	mmu register	*/
	{0x13000},	/* entry 40 'FE,E'		*/
	{0x020}, 	/* entry 41 */
	{0x001}, 	/* entry 42 */
	{0x008}, 	/* entry 43 */
	{0x300}, 	/* entry 44 */
	{0x14000}, 	/* entry 45 'H'	FPm list	*/
	{0x15000}, 	/* entry 46 'I'	FPm		*/
	{0x16000}, 	/* entry 47 'J'	<ea>{Dn} or {#k}*/
	{0x3ff}, 	/* entry 48 */
	{0x801}, 	/* entry 49 */
	{0x17000}, 	/* entry 50 'K'	single FPcr	*/
	{0xaff}, 	/* entry 51 */
	{0x18000}, 	/* entry 52 'L'	FPIAR only	*/
	{0x19000}, 	/* entry 53 'M'	FPm:FPn		*/
	{0x1a000} 	/* entry 54 'O' Fpcr list	*/
};

ACTUALFIELD_T_F Field_option[5];

EASPEC EA[2];	/* where SorD will be either TRUE or FALSE and be used to
		   determine which is being used ie. EA[SorD]		*/


/* The DEVICE that the target system uses will determine which OPSTRUCTURE
   will be compiled into the monitor */

/* NOTE: FOR INSTRUCTION ENTRIES OF THE SAME TYPE THE LONGEST SIZE OPTION
	 SHOULD BE ENTERED FIRST!!!!!!!!!!!!!				*/

OPSTRUCTURE syntax1[]=
/* ******************************************************************** */
/* **************************68000 INSTRUCTIONS************************ */
/* ******************************************************************** */
{
	{"dc",			13,8,21,"xxxxxxxxxxxxxxxx",0,0, notimplemented},
/* OR Immediate */
/*1*/	{"ori.",		0,8,0, 	"00000000xxxxxxxx",7,6, biti_size},
/* OR Immediate to CCR */
/*2*/	{"ori.",		1,8,17, "0000000000111100",0,0, biti_reg},
/* OR Immediate to SR */
/*3*/	{"ori.",		2,8,18, "0000000001111100",0,0, biti_reg},
/* DYNAMIC  BIT */
/*4*/	{"btst.",		3,4,2, 	"0000xxx100xxxxxx",5,3, bit_dynamic},
/*5*/	{"bchg.",		3,4,0, 	"0000xxx101xxxxxx",5,3, bit_dynamic},
/*6*/	{"bclr.",		3,4,0, 	"0000xxx110xxxxxx",5,3, bit_dynamic},
/*7*/	{"bset.",		3,4,0, 	"0000xxx111xxxxxx",5,3, bit_dynamic},
/* MOVEP */
/*8*/	{"movep.",		4,19,4, "0000xxx10x001xxx",6,6, notimplemented},
/*9*/	{"movep.",		4,4,19, "0000xxx11x001xxx",6,6, notimplemented},
/* AND Immediate to CCR */
/*10*/	{"andi.",		1,8,17, "0000001000111100",0,0, biti_reg},
/* AND Immediate to SR */
/*11*/	{"andi.",		2,8,18, "0000001001111100",0,0, biti_reg},
/* AND Immediate */
/*12*/	{"andi.",		0,8,0, 	"00000010xxxxxxxx",7,6, biti_size},
/* SUB Immediate */
/*13*/	{"subi.",		0,8,0, 	"00000100xxxxxxxx",7,6, biti_size},
/* ADD Immediate */
/*14*/	{"addi.",		0,8,0, 	"00000110xxxxxxxx",7,6, biti_size},
/* STATIC BIT */ 
/*15*/	{"btst.",		3,8,1, 	"0000100000xxxxxx",5,3, bit_static},
/*16*/	{"bchg.",		3,8,0, 	"0000100001xxxxxx",5,3, bit_static},
/*17*/	{"bclr.",		3,8,0, 	"0000100010xxxxxx",5,3, bit_static},
/*18*/	{"bset.",		3,8,0, 	"0000100011xxxxxx",5,3, bit_static},
/* EOR IMMEDIATE to CCR */ 
/*19*/	{"eori.",		1,8,17, "0000101000111100",0,0, biti_reg},
/* EOR IMMEDIATE to SR*/ 
/*20*/	{"eori.",		2,8,18, "0000101001111100",0,0, biti_reg},
/* EOR IMMEDIATE */ 
/*21*/	{"eori.",		0,8,0, 	"00001010xxxxxxxx",7,6, biti_size},
/* CMP IMMEDIATE */ 
/*22*/	{"cmpi.",		0,8,1, 	"00001100xxxxxxxx",7,6, biti_size},
/* MOVE LONG */ 
/*23*/	{"move.",		5,10,0, "0010xxxxxxxxxxxx",15,15, move_instr},
/* MOVEA LONG */ 
/*24*/	{"movea.",		5,10,3, "0010xxx001xxxxxx",15,15, move_instr},
/* MOVE BYTE */ 
/*25*/	{"move.",		1,2,0, 	"0001xxxxxxxxxxxx",15,15, move_instr},
/* MOVEA WORD */ 
/*26*/	{"movea.",		2,10,3, "0011xxx001xxxxxx",15,15, move_instr},
/* MOVE WORD */ 
/*27*/	{"move.",		2,10,0, "0011xxxxxxxxxxxx",15,15, move_instr},
/* MOVE from SR */ 
/*28*/	{"move.",		2,18,0, "0100000011xxxxxx",8,8, notimplemented},
/* NEGX */ 
/*29*/	{"negx.",		0,0,21, "01000000xxxxxxxx",7,6, notimplemented},
/* CHK 68000 */ 
/*30*/	{"chk.",		2,2,4, 	"0100xxx110xxxxxx",6,6, notimplemented},
/* LEA */ 
/*31*/	{"lea.",		5,5,3, 	"0100xxx111xxxxxx",12,12, notimplemented},
/* CLR */ 
/*32*/	{"clr.",		0,0,21, "01000010xxxxxxxx",7,6, notimplemented},
/* MOVE to CCR */ 
/*33*/	{"move.",		2,2,17, "0100010011xxxxxx",8,8, notimplemented},
/* NEG */ 
/*34*/	{"neg.",		0,0,21, "01000100xxxxxxxx",7,6, notimplemented},
/* MOVE to SR */ 
/*35*/	{"move.",		2,2,18, "0100011011xxxxxx",8,8, notimplemented},
/* NOT */ 
/*36*/	{"not.",		0,0,21, "01000110xxxxxxxx",7,6, notimplemented},
/* NBCD */ 
/*37*/	{"nbcd.",		1,0,21, "0100100000xxxxxx",6,6, notimplemented},
/* SWAP */ 
/*38*/	{"swap.",		2,4,21, "0100100001000xxx",3,3, notimplemented},
/* PEA */ 
/*39*/	{"pea.",		5,5,21, "0100100001xxxxxx",7,7, notimplemented},
/* MOVEM Registers to EA */ 
/*40*/	{"movem.",		4,20,6, "010010001xxxxxxx",6,6, notimplemented},
/*ILLEGAL  */ 
/*41*/	{"illegal",		6,21,21, "0100101011111100",0,0, notimplemented},
/* TAS */ 
/*42*/	{"tas.",		1,0,21, "0100101011xxxxxx",8,8, notimplemented},
/* TST */ 
/*43*/	{"tst.",		0,1,21, "01001010xxxxxxxx",7,6, notimplemented},
/* MOVEM EA to Registers */ 
/*44*/	{"movem.",		4,7,20, "010011001xxxxxxx",6,6, notimplemented},
/* TRAP */ 
/*45*/	{"trap",		6,8,21, "010011100100xxxx",0,0, notimplemented},
/* LINK WORD */ 
/*46*/	{"link.",		2,3,8, "0100111001010xxx",3,3, notimplemented},
/* UNLK */ 
/*47*/	{"unlk",		6,3,21, "0100111001011xxx",0,0, notimplemented},
/* MOVE to USP */ 
/*48*/	{"move.",		5,3,26, "0100111001100xxx",4,4, notimplemented},
/* MOVE from USP */ 
/*49*/	{"move.",		5,26,3, "0100111001101xxx",4,4, notimplemented},
/* RESET */ 
/*50*/	{"reset",		6,21,21, "0100111001110000",0,0, notimplemented},
/* NOP */ 
/*51*/	{"nop",			6,21,21, "0100111001110001",0,0, notimplemented},
/* STOP */ 
/*52*/	{"stop",		6,8,21, "0100111001110010",0,0, notimplemented},
/* RTE */ 
/*53*/	{"rte",			6,21,21, "0100111001110011",0,0, notimplemented},
/* RTS */ 
/*54*/	{"rts",			6,21,21, "0100111001110101",0,0, notimplemented},
/* TRAPV */ 
/*55*/	{"trapv",		6,21,21, "0100111001110110",0,0, notimplemented},
/* RTR */ 
/*56*/	{"rtr",			6,21,21, "0100111001110111",0,0, notimplemented},
/* JSR */ 
/*57*/	{"jsr",			6,5,21, "0100111010xxxxxx",0,0, notimplemented},
/* JMP */ 
/*58*/	{"jmp",			6,5,21, "0100111011xxxxxx",0,0, notimplemented},
/* DBcc */ 
/*59*/	{"db~~.",		2,4,23, "0101xxxx11001xxx",4,4, notimplemented},
/* Scc */ 
/*60*/	{"s~~.",		8,0,21, "0101xxxx11xxxxxx",6,6, notimplemented},
/* ADDQ */ 
/*61*/	{"addq.",		0,8,0, "0101xxx0xxxxxxxx",7,6, notimplemented},
/* SUBQ */ 
/*62*/	{"subq.",		0,8,0, "0101xxx1xxxxxxxx",7,6, notimplemented},
/*63*/	{"subq.",		7,8,3, "0101xxx1xxxxxxxx",7,6, notimplemented},
/* BRA */ 
/*64*/	{"bra.",		0,23,21, "01100000xxxxxxxx",10,10, notimplemented},
/* BSR */ 
/*65*/	{"bsr.",		0,23,21, "01100001xxxxxxxx",10,10, notimplemented},
/* Bcc */ 
/*66*/	{"b~~.",		0,23,21, "0110xxxxxxxxxxxx",10,10, notimplemented},
/* MOVEQ */ 
/*67*/	{"moveq.",		5,8,4, "0111xxx0xxxxxxxx",8,8, notimplemented},
/* SBCD */ 
/*68*/	{"sbcd.",		1,4,4, "1000xxx100000xxx",4,4, notimplemented},
/*69*/	{"sbcd.",		1,13,13, "1000xxx100001xxx",4,4, notimplemented},
/* DIVU */ 
/*70*/	{"divu.",		2,2,4, "1000xxx011xxxxxx",8,8, notimplemented},
/* DIVS */ 
/*71*/	{"divs.",		2,2,4, "1000xxx111xxxxxx",12,12, notimplemented},
/* SUBA */ 
/*72*/	{"suba.",		9,10,3, "1001xxxx11xxxxxx",8,7, notimplemented},
/* SUBX */ 
/*73*/	{"subx.",		0,4,4, "1001xxx1xx000xxx",7,6, notimplemented},
/*74*/	{"subx.",		0,13,13, "1001xxx1xx001xxx",7,6, notimplemented},
/* CMPA */ 
/*75*/	{"cmpa.",		9,10,3, "1011xxxx11xxxxxx",8,7, notimplemented},
/* CMP */ 
/*76*/	{"cmp.",		0,10,4, "1011xxx0xxxxxxxx",7,6, notimplemented},
/* EOR */ 
/*77*/	{"eor.",		0,4,0, "1011xxx1xxxxxxxx",7,6, notimplemented},
/* CMPM */ 
/*78*/	{"cmpm.",		0,24,24, "1011xxx1xx001xxx",7,6, notimplemented},
/* MULU WORD */ 
/*79*/	{"mulu.",		2,2,4, "1100xxx011xxxxxx",8,8, notimplemented},
/* MULS WORD */ 
/*80*/	{"muls.",		2,2,4, "1100xxx111xxxxxx",12,12, notimplemented},
/* ABCD */ 
/*81*/	{"abcd.",		1,4,4, "1100xxx100000xxx",7,6, notimplemented},
/*82*/	{"abcd.",		1,13,13, "1100xxx100001xxx",7,6, notimplemented},
/* EXG data registers  */ 
/*83*/	{"exg.",		5,4,4, "1100xxx101000xxx",3,3, notimplemented},
/* EXG address registers  */ 
/*84*/	{"exg.",		5,3,3, "1100xxx101001xxx",4,4, notimplemented},
/* EXG data register and address  */ 
/*85*/	{"exg.",		5,4,3, "1100xxx110001xxx",4,4, notimplemented},
/* AND  */ 
/*86*/	{"and.",		0,2,4, "1100xxx0xxxxxxxx",7,6, notimplemented},
/*87*/	{"and.",		0,4,12, "1100xxx1xxxxxxxx",7,6, notimplemented},
/* ADDA */ 
/*88*/	{"adda.",		9,10,3, "1101xxxx11xxxxxx",8,7, notimplemented},
/* ADDX */ 
/*89*/	{"addx.",		0,4,4, "1101xxx1xx000xxx",7,6, notimplemented},
/*90*/	{"addx.",		0,13,13, "1101xxx1xx001xxx",7,6, notimplemented},
/* ADD */ 
/*91*/	{"add.",		0,2,4, "1101xxx0xxxxxxxx",7,6, notimplemented},
/*92*/	{"add.",		7,3,4, "1101xxx0xxxxxxxx",7,6, notimplemented},
/*93*/	{"add.",		0,4,12, "1101xxx1xxxxxxxx",7,6, notimplemented},
/* SHIFT ROTATE memory */ 
/*94*/	{"rol.",		11,12,21, "1110011111xxxxxx",7,6, notimplemented},
/*95*/	{"ror.",		11,12,21, "1110011011xxxxxx",7,6, notimplemented},
/* SHIFT ROTATE logical memory */ 
/*96*/	{"roxl.",		11,12,21, "1110010111xxxxxx",7,6, notimplemented},
/*97*/	{"roxr.",		11,12,21, "1110010011xxxxxx",7,6, notimplemented},
/* SHIFT ROTATE */ 
/*98*/	{"rol.",		0,8,4, "1110xxx1xx011xxx",7,6, notimplemented},
/*99*/	{"rol.",		0,4,4, "1110xxx1xx111xxx",7,6, notimplemented},
/* SHIFT ROTATE */ 
/*100*/	{"ror.",		0,8,4, "1110xxx0xx011xxx",7,6, notimplemented},
/*101*/	{"ror.",		0,4,4, "1110xxx0xx111xxx",7,6, notimplemented},
/* SHIFT ROTATE */ 
/*102*/	{"roxl.",		0,8,4, "1110xxx1xx010xxx",7,6, notimplemented},
/*103*/	{"roxl.",		0,4,4, "1110xxx1xx110xxx",7,6, notimplemented},
/* SHIFT ROTATE */ 
/*104*/	{"roxr.",		0,8,4, "1110xxx0xx010xxx",7,6, notimplemented},
/*105*/	{"roxr.",		0,4,4, "1110xxx0xx110xxx",7,6, notimplemented},
/* EXT */ 
/*106*/	{"ext.",		10,4,21, "01001000xx000xxx",7,6, notimplemented},
/* ADDQ */ 
/*107*/	{"addq.",		7,8,3, "0101xxx0xxxxxxxx",7,6, notimplemented},
/* SHIFT ROTATE */ 
/*108*/	{"asl.",		11,12,21, "1110000111xxxxxx",7,6, notimplemented},
/*109*/	{"asl.",		0,8,4, "1110xxx1xx000xxx",7,6, notimplemented},
/*110*/	{"asl.",		0,4,4, "1110xxx1xx100xxx",7,6, notimplemented},
/*111*/	{"asr.",		11,12,21, "1110000011xxxxxx",7,6, notimplemented},
/*112*/	{"asr.",		0,8,4, "1110xxx0xx000xxx",7,6, notimplemented},
/*113*/	{"asr.",		0,4,4, "1110xxx0xx100xxx",7,6, notimplemented},
/* LOGICAL SHIFT ROTATE */ 
/*114*/	{"lsl.",		11,12,21, "1110001111xxxxxx",7,6, notimplemented},
/*115*/	{"lsl.",		0,8,4, "1110xxx1xx001xxx",7,6, notimplemented},
/*116*/	{"lsl.",		0,4,4, "1110xxx1xx101xxx",7,6, notimplemented},
/*117*/	{"lsr.",		11,12,21, "1110001011xxxxxx",7,6, notimplemented},
/*118*/	{"lsr.",		0,8,4, "1110xxx0xx001xxx",7,6, notimplemented},
/*119*/	{"lsr.",		0,4,4, "1110xxx0xx101xxx",7,6, notimplemented},
/* OR  */
/*120*/	{"or.",			0,4,12, "1000xxx1xxxxxxxx",7,6, notimplemented},
/*121*/	{"or.",			0,2,4, "1000xxx0xxxxxxxx",7,6, notimplemented},
/* SUB */ 
/*122*/	{"sub.",		0,2,4, "1001xxx0xxxxxxxx",7,6, notimplemented},
/*123*/	{"sub.",		7,3,4, "1001xxx0xxxxxxxx",7,6, notimplemented},
#if ((DEVICE==68000) || (DEVICE==68008))
/*124*/	{"sub.",		0,4,12, "1001xxx1xxxxxxxx",7,6, notimplemented}

};
#endif /* end for OPSTRUCTURE */

#if DEVICE>=68010
/* SUB */ 
/*124*/	{"sub.",		0,4,12, "1001xxx1xxxxxxxx",7,6, notimplemented},
/* ******************************************************************** */
/* **************************68010 INSTRUCTIONS************************ */
/* ******************************************************************** */

/* MOVES 68010 */ 
/*125*/	{"moves.",		0,25,12, "00001110xxxxxxxx",7,6, notimplemented},
/* MOVES 68010 */ 
/*126*/	{"moves.",		0,12,25, "00001110xxxxxxxx",7,6, notimplemented},
/* MOVE from CCR */ 
/*127*/	{"move.",		2,17,0, "0100001011xxxxxx",8,8, notimplemented},
/* RTD */ 
/*128*/	{"rtd",			6,8,21,	"0100111001110100",7,6, notimplemented},
/* MOVEC */ 
/*129*/	{"movec.",		5,27,21, "010011100111101x",2,2, notimplemented},
#endif /* endif for 68010 OPSTRUCTURE */
#if DEVICE==68010
/* BKPT */ 
/*130*/	{"bkpt",		6,8,21, "0100100001001xxx",4,4, notimplemented}

};
#endif /* end for OPSTRUCTURE */

#if DEVICE>=68020
/* BKPT */ 
/*130*/	{"bkpt",		6,8,21, "0100100001001xxx",4,4, notimplemented},

/* ******************************************************************** */
/* **************************68020 INSTRUCTIONS************************ */
/* ******************************************************************** */

/* CMP2 */ 
/*131*/	{"cmp2.",		0,5,25, "00000xx011xxxxxx",10,9, notimplemented},
/* CHK2 */ 
/*132*/	{"chk2.",		0,5,25, "00000xx011xxxxxx",10,9, notimplemented},
/* RTM */ 
/*133*/	{"rtm",			6,25,21, "000001101100xxxx",4,4, notimplemented},
/* CALLM */ 
/*134*/	{"callm",		6,8,5, "0000011011xxxxxx",8,8, notimplemented},
/* CAS */ 
/*135*/	{"cas.",		14,37,34, "00001xx011xxxxxx",10,9, notimplemented},
/* CAS2 */ 
/*136*/	{"cas2.",		10,29,21, "000011x011111100",10,9, notimplemented},
/* CHK 68020 */ 
/*137*/	{"chk.",		5,2,4, "0100xxx100xxxxxx",6,6, notimplemented},
/* LINK LONG */ 
/*138*/	{"link.",		5,3,8, "0100100000001xxx",4,4, notimplemented},
/* EXTB */ 
/*139*/	{"extb.",		5,4,21, "0100100111000xxx",9,9, notimplemented},
/* MULS LONG */ 
/*140*/	{"muls.",		5,2,4, "0100110000xxxxxx",6,6, notimplemented},
/*141*/	{"muls.",		5,2,28, "0100110000xxxxxx",6,6, notimplemented},
/* MULU LONG */ 
/*142*/	{"mulu.",		5,2,4, "0100110000xxxxxx",6,6, notimplemented},
/*143*/	{"mulu.",		5,2,28, "0100110000xxxxxx",6,6, notimplemented},
/* DIVU LONG */ 
/*144*/	{"divu.",		12,2,4, "0100110001xxxxxx",6,6, notimplemented},
/*145*/	{"divu.",		12,2,28, "0100110001xxxxxx",6,6, notimplemented},
/* DIVUL LONG */ 
/*146*/	{"divul.",		12,2,28, "0100110001xxxxxx",6,6, notimplemented},
/* DIVS LONG */ 
/*147*/	{"divs.",		12,2,4, "0100110001xxxxxx",6,6, notimplemented},
/*148*/	{"divs.",		12,2,28, "0100110001xxxxxx",6,6, notimplemented},
/* DIVSL LONG */ 
/*149*/	{"divsl.",		12,2,28, "0100110001xxxxxx",6,6, notimplemented},
/* TRAPcc */ 
/*150*/	{"trap~~",		6,21,21, "0101xxxx11111100",0,0, notimplemented},
/* TRAPcc */ 
/*151*/	{"trap~~.",		10,8,21, "0101xxxx1111101x",1,0, notimplemented},
/* PACK */ 
/*152*/	{"pack",		6,36,35, "1000xxx101001xxx",0,0, notimplemented},
/*153*/	{"pack",		6,37,35, "1000xxx101000xxx",0,0, notimplemented},
/* UNPK */ 
/*154*/	{"unpk",		6,36,35, "1000xxx110001xxx",0,0, notimplemented},
/*155*/	{"unpk",		6,37,35, "1000xxx110000xxx",0,0, notimplemented},
/* BITFIELD */ 
/* BFCHG and BFCLR and BFSET*/
/*156*/	{"bf~~~",		6,32,21, "11101xx011xxxxxx",8,8, notimplemented},
/* BFTST */
/*157*/	{"bf~~~",		6,33,21, "1110100011xxxxxx",8,8, notimplemented},
/* BFEXTS and BFEXTU and BFFFO */
/*158*/	{"bf~~~",		6,33,4, "11101xx111xxxxxx",8,8, notimplemented},
#endif /* endif for 68020 OPSTRUCTURE */
#if DEVICE==68020
/* BFINS */
/*159*/	{"bf~~~",		6,4,32, "1110111111xxxxxx",8,8, notimplemented}

};
#endif /* end for OPSTRUCTURE */

#if DEVICE>=68030
/* BITFIELD */ 
/* BFINS */
/*159*/	{"bf~~~",		6,4,32, "1110111111xxxxxx",8,8, notimplemented},

/* ******************************************************************** */
/* **************************68030 INSTRUCTIONS************************ */
/* ******************************************************************** */
/* ******************************************************************** */

/* PFLUSHA */
/*160*/	{"pflusha",		6,21,21, "1111000000xxxxxx",0,0, notimplemented},
/* PFLUSH */
/*161*/	{"pflush",		6,30,14, "1111000000xxxxxx",0,0, notimplemented},
/*162*/	{"pflush",		6,30,21, "1111000000xxxxxx",0,0, notimplemented},
/* PLOADR */
/*163*/	{"ploadr",		6,38,14, "1111000000xxxxxx",0,0, notimplemented},
/* PLOADW */
/*164*/	{"ploadw",		6,38,14, "1111000000xxxxxx",0,0, notimplemented},
/* PMOVE */
/*165*/	{"pmove",		6,39,14, "1111000000xxxxxx",0,0, notimplemented},
/*166*/	{"pmove",		6,14,39, "1111000000xxxxxx",0,0, notimplemented},
/* PMOVEFD */
/*167*/	{"pmovefd",		6,14,39, "1111000000xxxxxx",0,0, notimplemented},
/* PTESTR */
/*168*/	{"ptestr",		6,40,3, "1111000000xxxxxx",0,0, notimplemented},
/*169*/	{"ptestr",		6,40,21, "1111000000xxxxxx",0,0, notimplemented},
/* PTESTW */
/*170*/	{"ptestw",		6,40,3, "1111000000xxxxxx",0,0, notimplemented},

#endif /* endif for 68030 OPSTRUCTURE */
#if DEVICE==68030
/*171*/	{"ptestw",		6,40,21, "1111000000xxxxxx",0,0, notimplemented}

};
#endif /* end for OPSTRUCTURE */

#if DEVICE>=68040
/* PTESTW */
/*171*/	{"ptestw",		6,40,21, "1111000000xxxxxx",0,0, notimplemented},
/* PFLUSH */
/*172*/	{"pflush",		6,41,21, "11110101000xxxxx",0,0, notimplemented},
/* PFLUSHN */
/*173*/	{"pflushn",		6,41,21, "11110101000xxxxx",0,0, notimplemented},
/* PFLUSHA */
/*174*/	{"pflusha",		6,21,21, "11110101000xxxxx",0,0, notimplemented},
/* PFLUSHAN */
/*175*/	{"pflushan",		6,21,21, "11110101000xxxxx",0,0, notimplemented},
/* PTESTR */
/*176*/	{"ptestr",		6,41,21, "1111010101101xxx",0,0, notimplemented},
/* PTESTW */
/*177*/	{"ptestw",		6,41,21, "1111010101001xxx",0,0, notimplemented},
/* CINVL */
/*178*/	{"cinvl",		6,42,41, "11110100xx001xxx",0,0, notimplemented},
/* CINVP */
/*179*/	{"cinvp",		6,42,41, "11110100xx010xxx",0,0, notimplemented},
/* CINVA */
/*180*/	{"cinva",		6,42,21, "11110100xx011xxx",0,0, notimplemented},
/* CPUSHL */
/*181*/	{"cpushl",		6,42,41, "11110100xx101xxx",0,0, notimplemented},
/* CPUSHP */
/*182*/	{"cpushp",		6,42,41, "11110100xx110xxx",0,0, notimplemented},
/* CPUSHA */
/*183*/	{"cpusha",		6,42,21, "11110100xx111xxx",0,0, notimplemented},
/* MOVE16 */
/*184*/	{"move16",		6,42,42, "1111011000100xxx",0,0, notimplemented},
/*185*/	{"move16",		6,43,44, "11110110000xxxxx",0,0, notimplemented},
/* MOVE16 */
/*186*/	{"move16",		6,44,43, "11110110000xxxxx",0,0, notimplemented}

};
#endif /* endif for 68040 OPSTRUCTURE */


#if (COPROCESSOR==TRUE || DEVICE==68040)

COPROCSTRUCTURE syntax2[]=
/* ******************************************************************** */
/* *********************68881/68882 INSTRUCTIONS*********************** */
/* ******************************************************************** */
{
	{"reserved",	13,21,21},
	{"reserved",	13,21,21},
	{"reserved",	13,21,21},
/* FBcc */
/*3*/	{"fb~~.",		4,23,21	},
/* FDBcc */ 
/*4*/	{"fdb~~",		6,4,23	},
/* FMOVEM Registers to EA */ 
/*5*/	{"fmovem.",		16,45,6	},
/* FMOVEM Dn to EA */ 
/*6*/	{"fmovem.",		16,4,6	},
/* FMOVEM EA to Registers */ 
/*7*/	{"fmovem.",		16,7,45	},
/* FMOVEM EA to Dn */ 
/*8*/	{"fmovem.",		16,7,4	},
/* FNOP */ 
/*9*/	{"fnop",		6,21,21	},
/* FRESTORE */ 
/*10*/	{"frestore",		6,7,21	},
/* FSAVE */ 
/*11*/	{"fsave",		6,6,21	},
/* FScc */ 
/*12*/	{"fs~~.",		8,0,21	},
/* FTRAPcc */ 
/*13*/	{"ftrap~~",		6,21,21	},
/* FTRAPcc */ 
/*14*/	{"ftrap~~.",		10,8,21	},
/* FMOVE <ea>,FPm */ 
/*15*/	{"fmove.",		15,48,46	},
/*16*/	{"fmove.",		17,4,46	},
/* FMOVE FPm,<ea> */ 
/*17*/	{"fmove.",		19,46,48	},
/*18*/	{"fmove.",		17,46,4	},
/* FMOVE FPm,FPn */ 
/*19*/	{"fmove.",		16,46,46	},
/* FMOVE.L <ea>,FPcr */ 
/*20*/	{"fmove.",		5,2,50	},
/*21*/	{"fmove.",		5,3,52	},
/* FMOVE.L FPcr,<ea> */ 
/*22*/	{"fmove.",		5,50,0	},
/*23*/	{"fmove.",		5,52,3	},
/* FMOVEM.L <list>,<ea> */ 
/*24*/	{"fmovem.",		5,50,4	},
/*25*/	{"fmovem.",		5,52,3	},
/*26*/	{"fmovem.",		5,54,12	},
/* FMOVEM.L <ea>,<list> */ 
/*27*/	{"fmovem.",		5,4,50	},
/*28*/	{"fmovem.",		5,3,52	},
/*29*/	{"fmovem.",		5,48,54	},
/* MONADIC or DYADIC SEARCH*/ 
/*30*/	{"f~",			15,48,46},
/*31*/	{"f~",			17,4,46	},
/*32*/	{"f~",			16,46,46},
/*33*/	{"f~",			16,46,21},
/* FTST */ 
/*34*/	{"ftst.",		15,48,21},
/*35*/	{"ftst.",		17,4,21	},
#endif /* endif for 68040 COPROCSTRUCTURE */

#if (DEVICE+COPROCESSOR+EMULATOR==68040)
/*36*/	{"ftst.",		16,46,21}
};
#endif /* end for COPROCSTRUCTURE */

#if((COPROCESSOR==TRUE) || (EMULATOR==TRUE))
/* FTST */ 
/*36*/	{"ftst.",		16,46,21},
/* FMOVE.P FPm,<ea>{Dn} or FPm,<ea>{#k} */ 
/*37*/	{"fmove.",		18,46,47},
/* FMOVECR  CONSTANT ROM */ 
/*38*/	{"fmovecr.",		16,8,46	},
/* FSINCOS <ea>,FPc:FPs*/ 
/*39*/	{"fsincos.",		17,4,53	},
/*40*/	{"fsincos.",		15,48,53},
/* FSINCOS FPm,FPc:FPs*/ 
/*41*/	{"fsincos.",		16,46,53}
};
#endif /* end for COPROCSTRUCTURE */


/* ****************ADDITIONS*********************** */

/*
these are the predefined symbols for the symbol table. The correspond
to the standard MOTOROLA memory map. To change the startup symbols,
change them here. To add or delete the startup symbols, add or delete
from here and in doinit under main.c .
*/

char ROMSYMB[] = "/rom";

/*
these are the registers known. They are currently set up for a maximum
size of 4 char plus a blank. To add or delete registers known, change
the names here and in doinit under main.c 
*/

char d_0[] = "D0   ";
char d_1[] = "D1   ";
char d_2[] = "D2   ";
char d_3[] = "D3   ";
char d_4[] = "D4   ";
char d_5[] = "D5   ";
char d_6[] = "D6   ";
char d_7[] = "D7   ";
char a_0[] = "A0   ";
char a_1[] = "A1   ";
char a_2[] = "A2   ";
char a_3[] = "A3   ";
char a_4[] = "A4   ";
char a_5[] = "A5   ";
char a_6[] = "A6   ";
char a_7[] = "A7   ";
char p_c[] = "PC   ";
char s_r[] = "SR   ";
char usp[] = "USP  ";
char msp[] = "MSP  ";
#if(DEVICE<68020)
char ssp[] = "SSP  ";
#else
char isp[] = "ISP  ";
#endif
char sfc[] = "SFC  ";
char dfc[] = "DFC  ";
char vbr[] = "VBR  ";
char cac[] = "CACR ";
char caa[] = "CAAR ";
#if(DEVICE==68030)
char crp[] = "CRP  ";
char srp[] = "SRP  ";
char tc[]  = "TC   ";
char tt_0[] = "TT0  ";
char tt_1[] = "TT1  ";
char mmusr[] = "PSR  ";
#endif
#if(DEVICE==68040)
char urp[] = "URP  ";
char srp[] = "SRP  ";
char tc[]  = "TC   ";
char dtt_0[] = "DTT0 ";
char dtt_1[] = "DTT1 ";
char itt_0[] = "ITT0 ";
char itt_1[] = "ITT1 ";
char mmusr[] = "PSR  ";
#endif

/*
these are the miscellanous messages that are needed in different parts
of the program.
*/

char PROMPT[] = "\nMOTOROLA> ";
char WHICHREGMSG[] = "Which register? ";
char BRKMSG[] = "Break Point:\n";
char SYMBMSG[] = "Known Symbols:\n";
char UNKNOWNMSG[] = "UNKNOWN\t\t\t?";
char HITKEYMSG[] = "Hit any key to continue ...\n";
char MMASHELPMSG[] = "\nDirectives.\nBackup -[<number>], Advance +[<number>], Help '?', Quit 'q' or '.'\n\n";

/*
hese are the error messages that are printed out at different
procedures thoughout the program.
*/

char NOERR[] = "";
char ERR01[] = "ERROR 01: illegal number of arguments\n";
char ERR02[] = "ERROR 02: illegal address syntax\n";
char ERR03[] = "ERROR 03: illegal number syntax\n";
char ERR04[] = "ERROR 04: illegal option\n";
char ERR05[] = "ERROR 05: insufficient room to insert data\n";
char ERR06[] = "ERROR 06: breakpoint table is full\n";
char ERR07[] = "ERROR 07: illegal command call\n";
char ERR08[] = "\nERROR 08: S-record load failed, invalid checksum = ";
char ERR09[] = "ERROR 09: invalid assembly => ";

/*
this is a list of the various syntaxes for the commands. It is
accessed under he command, by typing he and then the syntax name.
*/

struct syntax_element syn[]=
{
	{"range <addr>,<addr>\nrange <addr>:<count>\n"},
	{"addr <number>\n"},
	{"offset <number>\n"},
	{"hex [$]<0-9 || A-F || a-f>[<0-9 || A-F || a-f>]...\n"},
	{"dec &<0-9>[<0-9>]...\n"},
	{"oct @<0-7>[<0-7>]...\n"},
	{"bin %<0-1>[<0-1>]...\n"},
	{"number <hex> || <dec> || <oct> || <bin> || <symbol>\n"},
	{"symbol /<ASCII character>[<ASCII character>]...\n"},
	{"count <number>\n"},
	{"data <number>\n"},
	{"size -b || -h || -w || -l\n"},
	{"exp [<sign>]<number>[<sign><number>]...\n"},
	{"sign + || -\n"},
	{"mask <number>\n"},
	{"text ;<character string>\n"},
	{"name name of command or device i.e. dc,he or host\n"},
	{"unit -c0 || -c1 || -p || -f || -m\n"},
	{"escape a control character that will be used to break the transparent mode\n"},
	{"control '^' | '=' | '.'\n"},
	{LASTCMD}
};

/*
these are the ports known by the monitor. To add ports, add them here
*/

struct port_element p[]=
{
	{"term ",TERMINAL,8},
	{"host ",HOST,8},
	{LASTCMD,0}
};

/*
this is the table of commands known, the pointer to the procedure that
handles the command and the syntax for each command. To add or delete
new commands, do so here. Alphabetical order is suggested, but not
required.
*/
/*   Commented out by DLB
struct table_element f[]=
{
	{"as Assembler/Disassembler\n",ascmd,"as <addr>\n"},
	{"bf Block Fill\n",bfcmd,"bf [<size>] <range> <data>\n"},
	{"bm Block Move\n",bmcmd,"bm [<size>] <range> <addr>\n"},
	{"br Break Point Insertion\n",brcmd,"br [<addr>][<COUNTDEL><count>]\nbr -r [<addr>]\n"},
	{"bs Block Search\n",bscmd,"bs [<size>] <range> (<data>[ <mask>])||(<text>)\n"},
	{"dc Data Conversion\n",dccmd,"dc <exp>\n"},
	{"g  \n",gocmd,"g  [-b] [<addr>]\n"},
	{"go Go Execute User Program\n",gocmd,"go [-b] [<addr>]\n"},
	{"?  \n",hecmd,"? [<name>]||[<syntax>]\n"},
	{"he \n",hecmd,"he [<name>]||[<syntax>]\n"},
	{"help Help\n",hecmd,"help [<name>]||[<syntax>]\n"},
	{"lo Load S-Records\n",locmd,"lo [<offset>] [<text>]\n"},
	{"md Memory Display\n",mdcmd,"md [<size>] <addr>||<range>\n\nMemory Display with Disassembly\nmd -di <addr>\n"},
	{"mm Memory Modify\n",mmcmd,"mm [<size>] <addr> [<control>]\n"},
	{"rd Register Display\n",rdcmd,"rd [<unit>]\n"},
	{"rm Register Modify\n",rmcmd,"rm [<register> [<New Value>]]\n"},
	{"sd Symbol Define\n",sdcmd,"sd [<symbol> <exp>]\n"},
	{"tm Transparent Mode\n",tmcmd,"tm [<escape>]\n"},
	{"tr Trace\n",trcmd,"tr \n"},
	{LASTCMD,0,0}
};
Commented out by DLB */

/*
this is a table used for determining the m5 field for bcnd and tcnd
instructions. It is used in disasm88 and can be commented out if the
assembler/disassembler is a 68000 family one.
*/

char *equality[] =
{
	"",
	"\tgt0",
	"\teq0",
	"\tge0",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"\tlt0",
	"\tne0",
	"\tle0"
};




/* ***************************************************************** */
