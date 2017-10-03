/* ******************************************************************** *
 * modtypes.h - Definitions for the module types found                  *
 *    in the module header.                                             *
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

struct modnam {
    char *name;
    int val;
};

/* *************************** *
 * Module Type/Language values *
 * *************************** */

#define MT_ANY		0
#define MT_PROGRAM	1
#define MT_SUBROUT	2
#define MT_MULTI	3
#define MT_DATA		4
#define MT_CSDDATA	5
#define MT_TRAPLIB	11
#define MT_SYSTEM	12
#define MT_FILEMAN	13
#define MT_DEVDRVR	14
#define MT_DEVDESC	15

/* Module Language values */
#define ML_ANY		0
#define ML_OBJECT	1
#define ML_ICODE	2

#define mktypelang(type,lang)	(((type)<<8)|(lang))

/* Module Attribute values */
#define MA_REENT	0x80
#define MA_GHOST	0x40
#define MA_SUPER	0x20
