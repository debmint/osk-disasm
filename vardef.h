/* ******************************************************************** *
 *                                                                      *
 * This is a collection of variables used in the monitor. This section  *
 * of code is "included" in the main.c, so the declerations are made    *
 * only once. It is meant to be located in RAM space during linking.    *
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

char argv[MAXLINE];	/* Command line that is typed in by user */
int argc;		/* Number of arguments on the command line */
int asmdata;		/* Memory location with data for assembly routines */
int asmaddr;		/* Memory location with address for assembly routines */
int error;		/* Global error flag for getnum routine */
int tracecount;		/* Trace count */
int oldbraddr;		/* Break point just hit address */
int regdata[MAXREGS];	/* Memory with reg values for assembly routine */
int stackptr;		/* Pointer to intterupt stack frame */
int snum;		/* Check sum value used by locmd */
int csptr;		/* Monitor stack pointer */
int handlerflag;

/* ******************** ADDITIONS *************************** */

char lastcmd[MAXLINE];	/* Last command line that was typed in by user */
int overheadRTS;
int startbenchmark;
int *ptr_to_overheadRTS = &overheadRTS;
int assemblycode[11];
int instructnum,condition;
char sizechar[2];
ACTUALFIELD_T_F Fieldoption[5];
int MIPOST;
int packasm;
char *opwordsyntax[50];
int confres;

#if ((COPROCESSOR==TRUE) || (DEVICE==68040))
char optionSorD;
int predicate;

struct regelem fpu[MAXREGS];		/* MPU register table */
#endif

/* ******************** ADDITIONS *************************** */

struct breakelem brtable[MAXBR];	/* Breakpoint table */

struct symbelem symb[MAXSYMBOL];	/* Symbol table */

struct regelem mpu[MAXREGS];		/* MPU register table */

/* ***************************************************************** */

int m8[255];
