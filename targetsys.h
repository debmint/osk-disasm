
/*
This is the easiest place to make change. If you make a change here,
then all the .c files must be recompiled. This can be done with the
make command and the appropriate makefile.
*/

/*
the boolean logic symbols defined for code readability.
*/

#define TRUE 1
#define FALSE 0

/*
if you want the monitor to automatically convert all input capitals to
lower case, excpet after a textdel, then this should be true.
Otherwise, this should be false.
*/

#define CONVCAP  TRUE

/*
these are the delimiters. Most are selfexpalnitory.
*/

#define BINDEL '%'	/* binary number */
#define OCTDEL '@'	/* octal number */
#define DECDEL '&'	/* decimalnumber */
#define HEXDEL '$'	/* hexadecimal */
#define PLUSDEL '+'	/* the arithmatic plus */
#define MINUSDEL '-'	/* the arithmatic minus */
#define SYMBOLDEL '/'	/* start of a symbol in the symbol table */
#define OPTDEL '-'	/* start of an option on a command line */
#define COUNTDEL ':'	/* indicates range uses a count, not address */
#define ADDRDEL ','	/* Used for assembly,change getnum if you change this */
#define TEXTDEL ';'	/* start of a text field */

/*
these are the charachters used for halting, continuing or canceling
the output of commands that are taking along time.
*/

#define HALTCHAR 'h'	/* halt display */
#define CONTCHAR 'c'	/* continue (after a halt) */
#define STOPCHAR 's'	/* stop the display */
#define ESCDEF 0x01	/* escape from transparent mode */

/*
these mark various breaks in the structures and strings for such
things as commands, register displays, etc. 
*/

#define ENDCMD ' '	/* used to show the end of a command */
#define BREAK '='	/* used to request a break in a display */
#define LASTCMD 0 	/* ends a structure of indeterminate length */

#define ENDSTR '\0'	/* end of a string */
#define LF 10		/* line feed */
#define CR 13		/* carriage return */
/*
directives for while in the mm and as commands.
*/

#define BACKUP '-'	/* back up a number of bytes */
#define FINISH '.'	/* quit the mm or as command */
#define NEXT '+'	/* advance a number of bytes */
#define HELP '?'	/* print out the directives available */

/*
numerical scales used in decoding command lines, or printing information to
the terminal.
*/

#define DEFAULTSCALE 16	/* the numerical scale used in most situations */
#define REGSCALE 10	/* numerical scale used to display register numbers */
#define COUNTSCALE 10	/* numerical scale for <count> in <range> */
#define DEFCOUNT 48	/* default on <count> if none used in <range> */

#define FPSCALE  10	/* numerical scale used to display/except floating
				point numbers */
#define FPDIGITS 4	/* number of decimal places to display floating point 
				numbers to */
#define FIELD_DISPLAY TRUE	/* display fields of floating point # data */ 

/* ******************************************************************


			begining of MPU and processor dependant defines



   ******************************************************************* */
/* 
the maximums for displaying o the screen.
*/

#define MAXLINE 80	/* maximum characters on a line on the screen */
#define MAXSCREEN 20 	/* maximum lines to display on the screen at once */
#define REGPERLINE 4	/* number of registers per line to display */
#define REGPERSCREEN 80	/* number of registers to display per screen */
#define MEMLINEDIS 6	/* number of address spaces to print per line */

#define MAXCMMU 2	/* number of cmmus for a 88k */


#define MAXBR 8		/* number of breakpoints available to user */
#define MAXSYMBOL 20	/* number of symbols in symbol table */
#define MAXSYMBLEN 10	/* maximum length of symbol name */

/*
memory locations, used for memory map purposes.
*/

/* *************************** ASM/DISASM DEFINES ************************ */

/* Defines that determine which 68xxx device the target system uses 
   and will determine what device the asm/disasm mimics.  If size
   is a factor then deletions in the textdef.h instruction file
   may be made based on the number of instructions that will be referenced 
   the user should define both the DEVICE, EMULATOR and the COPROCESSOR features
   directly below this statement */

/* ********************** */
/* ********************** */
/* ********************** */
/* ********************** */
/* ********************** */
/* ********************** */
#define DEVICE 68000
/* ********************** */
/* ********************** */
/* ********************** */
/* ********************** */
/* ********************** */
/* ********************** */

	/* known devices are 68000,68008,68010,68020,68030,68040 
	   this determines what version of the asm/disasm
	   will be compiled  */

	/* NOTE: The 68HC000 should be defined as a 68000 */

#if (DEVICE==68040)

/* ********************** */
/* ********************** */
/* ********************** */
/* ********************** */
/* ********************** */
/* ********************** */
#define EMULATOR TRUE       /* options are TRUE or FALSE */
/* ********************** */
/* ********************** */
/* ********************** */
/* ********************** */
/* ********************** */
/* ********************** */

	/* ***************** !!!!!!!!!!!!!!!!!!!!!!! ******************* */
	/* 	NOTE: This will provide entire 68881 support to 68040	 */
	/* ***************** !!!!!!!!!!!!!!!!!!!!!!! ******************* */

#endif



#if (DEVICE>=68020)

#if (DEVICE==68040)
#define COPROCESSOR TRUE      
#else
/* ********************** */
/* ********************** */
/* ********************** */
/* ********************** */
/* ********************** */
#define COPROCESSOR TRUE       /* options are TRUE or FALSE */
/* ********************** */
/* ********************** */
/* ********************** */
/* ********************** */
/* ********************** */
/* ********************** */

	/* ***************** !!!!!!!!!!!!!!!!!!!!!!! ******************* */
	/* 		NOTE: This is for entire 68881 support		 */
	/* ***************** !!!!!!!!!!!!!!!!!!!!!!! ******************* */

#endif
#endif

/* ********************************************************************* */


/* ********************************************************************* */
/* ********************** MEMORY MAP FOR DEBUGGER *********************** */

#define DUARTALOC 0xffe02000	/* MC68681 duart channel a */
#define DUARTBLOC 0xffe02008	/* MC68681 duart channel b */

#define ROMLOC 0x100000		/* system ROM space			*/
				/* .text segment should be loaded here	*/
				/* .data segment should be loaded here	*/

#define SYSRAMLOC  0x000fe000	
				/* system RAM space 			*/
				/* NOTE: space must be reserved for the
					 location of the .bss used by
					 the monitor.			*/
				/* Provided the .bss is not larger than
				   1000 (ie. VBRLOC resides at SYSRAMLOC 
				   + 1000) placing the .bss from the 
				   monitor at this location is allowed 	*/

#define ISPLOC SYSRAMLOC+0x2000 /* defines for monitor 	*/
#define MSPLOC SYSRAMLOC+0x1c00
#define USPLOC SYSRAMLOC+0x1b00
#define VBRLOC SYSRAMLOC+0x1000

#define USERPCLOC 0x4000 	/* defines for usercode	*/
#define USERVBRLOC 0x0

/* *********************************************************************** */
#if (DEVICE<68020)
#define USERSSPLOC USERVBRLOC+0x2000	/* Note: Only one of these is
						 used based on the device
						 being implemented */
#else
#define USERISPLOC USERVBRLOC+0x2000
#endif
/* *********************************************************************** */

#define USERMSPLOC USERVBRLOC+0x1c00
#define USERUSPLOC USERVBRLOC+0x1b00

#define SRSTART 0x2700		/* status register	*/

#define TERMINAL DUARTALOC	/* location of terminal (i.e. screen) */
#define HOST DUARTBLOC		/* location of host computer port */


/* ************************** END MEMMORY MAP ************************** */
/* ********************************************************************* */

/*
number of bytes word different sizes.
*/

/* We don't use these, and 'WORD' conflicts with WORD in OSK stdio.h */
/*#define WORD 2*/		/* word 2 for 68k, 4 for 88k */
/*#define BYTE 1*/		/* byte, always 1 */
/*#define LONG 4*/		/* long, 4 for 68k, not used for 88k */
/*#define HALF 2*/		/* half, 2 for 88k, not used for 68k */
/*#define DFSIZE 2*/	/* default size to use if none specified */


#ifndef COPROCESSOR
#define COPROCESSOR FALSE
#endif


#if (DEVICE==68000)
#ifndef COPROCESSOR
#define COPROCESSOR FALSE
#endif
#endif
#if (DEVICE==68008)
#define MAXINST 124
#define COPROCESSOR FALSE
#endif
#if (DEVICE==68010)
#define MAXINST 130
#define COPROCESSOR FALSE
#endif
#if (DEVICE==68020)
#define MAXINST 159
#endif
#if (DEVICE==68030)
#define MAXINST 171
#endif
#if (DEVICE==68040)
#define CRENTRIES 15
#define MAXINST 186		/* Note: This is the entire table */
#endif

#ifndef MAXINST
/*#define DEVICE  68000*/
#define MAXINST 124
#endif

	/* variable used to determine the extent of the
			   CONTROLREG table for movec instruction  */
#ifndef CRENTRIES
#define CRENTRIES 7
#endif

#ifndef EMULATOR
#define EMULATOR FALSE
#endif

#if (COPROCESSOR==TRUE || EMULATOR==TRUE)
#define MAXCOPROCINST 41	/* NOTE: This will be changed to represent the 
				   	 entire table when the table is 
				 	 implemented	*/
#else
#define MAXCOPROCINST 36
#endif


#if (EMULATOR==TRUE || COPROCESSOR==TRUE)
#define MONADICSRCH 34
#endif

#ifndef MONADICSRCH
#define MONADICSRCH 7
#endif

#define MAXMONADIC 34

#if (DEVICE==68040 || COPROCESSOR==TRUE)
#define MAXREGS 75 	/* most registers used by any one device */
#endif

#if (DEVICE==68030)
#ifndef MAXREGS
#define MAXREGS 41 	/* most registers used by any one device */
#endif
#endif


#ifndef MAXREGS
#define MAXREGS 33 	/* most registers used by any one device */
#endif
/* ************************************************************************ */

/*
location of program counter, status register and stack pointer in mpu register
list.
*/

#define PC 0
#define SR 1
#define USP 3
#define MSP 4
#define ISP 5
#define VBR 6
#define CACR 7
#define CAAR 8
#define SFC 9
#define DFC 10
#define D0 12
#define A7 28

/*
various values used to mask of bits in assemb;er/disassember
*/

#define MASK8  0x000000ff
#define MASK4  0x0000000f
#define MASK16 0x0000ffff
#define MASK12 0x00000fff
#define MASK32 0xffffffff
#define MASK 0xfc000000
#define MASK2521 0x03f00000
#define MASK2016 0x001f0000
#define MASK50 0x0000001f
#define MASK60 0x0000003f
#define MASK80 0x000001ff
#define MASK105 0x000007e0
#define MASK95 0x000003e0
#define MASK75 0x000000e0
#define MASK98 0x00000300
#define MASK109 0x00000600
#define MASK87 0x00000180
#define MASK65 0x00000060
#define MASK1514 0x0000c000
#define MASK1511 0x0000f800
#define MASK1310 0x00003c00
#define MASK1311 0x00003800
#define MASK139 0x00003e00
#define MASK9 0x00000200
#define MASK250 0x03ffffff

/*
Hex values used in tracing and break points
An alternative to using ILLEGAL is BKPT #0 (0x4848).
The monitor will handle both identically, but the hardware will react 
differently.
*/
#define BRTRAP 0x4afc	/* hex value of ILLEGAL instruction for break points */
#define TRACEON 0x8000	/* bit to toggle to turn trace on */

/*
location in the inst struct where the set of instructions start.
*/

#define SET0 0
#define SET1 32
#define SET2 40
#define SET3 112 
#define SET4 135
#define SET5 138
#define SET6 170
#define SET7 207
#define SET8 213
#define SET9 229
#define SET10 178
#define SET11 124
#define SET12 48
#define SET13 80
#define SET14 104


/*
offsets for duart
*/

#define MR1X 0	/* mode register 1 */
#define MR2X 0	/* mode register 2 */
#define SRX  1	/* status register */
#define CSRX 1	/* clock-select register */
#define CRX  2	/* command register */
#define RBX  3	/* receive buffer */
#define TBX  3	/* transmitter buffer */
#define IPCR 4	/* input port change register */
#define ACR  4	/* auxillary contol register */
#define ISR  5	/* interrupt status register */
#define IMR  5	/* interrupt mask register */
#define CMSB 6	/* counter most significant byte */
#define CTUR 6	/* counter/timer upper register */
#define CLSB 7	/* counter least significant register */
#define CTLR 7	/* counter/timer lower register */
#define IVR 12	/* interrupt vector register */
#define IP  13	/* input port */
#define OPCR 13	/* output port configuration register */
#define STRC 14	/* start counter command */
#define BTST 14	/* bit set command */
#define STPC 15	/* stop counter command */
#define BTRST 15	/* bit reset command */


/* **********	end of hardware and MPU dependant code  *************** */

