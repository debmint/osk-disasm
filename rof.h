/* ******************************************************************** *
 * rof.h - header file for rof definitions                              *
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
 * ******************************************************************** */

/* symbol table types */
/* symbol definition/reference type/location */

/* location flags for Global Refs and Local dests */
#define REFCOMN     0x100       /* Reference is COMMON      */
#define EREFREMOTE  0x100       /* External Ref is Remote   */

#define DATREMOTE   0x02        /* Data is REMOTE           */
#define CODEEQU     0x02        /* Code/Equ is EQU          */

#define REFINIT     0x04        /* DATA is INITIALIZED      */
#define CODEREF     0x01        /* Reference is in CODE or EQU */

#define LOCLLOC     0x100       /* Local Ref is Remote      */
#define LOCLCCODE   0x200       /* Local Ref is in Code     */

#define REFSIZ(a)   ((a >> 3) & 3)
#define REFREL      0x04        /* Ref is relative to loc   */
#define NEGMSK      0x08        /* Negate the symbol offset */

#ifdef ROF_C
#   define ROF_XT
#else
#   define ROF_XT extern
#endif

/* Define flags for type of reference */
enum {
    REFGLBL = 1,
    REFXTRN,
    REFLOCAL
};

struct asc_data {
    int start,
        length;
    struct asc_data *LNext,
                    *RNext;
};


 ROF_XT struct asc_data *data_ascii;


/*struct rof_extrn *xtrn_data = 0,
                 *xtrn_idata,
                 *xtrn_code = 0,
                 *xtrn_remote,
                 *xtrn_iremote,
                 *extrns;*/                   /* Generic external pointer */


ROF_XT struct rof_extrn *refs_data,
                 *refs_idata,
                 *refs_code,
                 *refs_remote,
                 *refs_iremote,
                 *extrns,                   /* Generic external pointer */
                 *codeRefs_sav;

/*struct nlist *dp_base,
             *vsect_base,
             *code_base,
             *lblptr;*/


/*struct rof_hdr ROF_hd,
               *rofptr = &ROF_hd;*/

