/* ***************************************************** *
 {* modtypes.h - Definitions for the module types found   $
 *    in the module header.                              $
 *                                                       $
 * $Id::                                                 $
 * ***************************************************** */

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

struct modnam ModTyps[] = {
    {"Prgrm", 1},
    {"Sbrtn", 2},
    {"Multi", 3},
    {"Data", 4},
    {"CSDData", 5},
    {"TrapLib", 11},
    {"Systm", 12},
    {"FlMgr", 13},
    {"Drivr", 14},
    {"Devic", 15},
    {"",0}
};

/* Module Language values */
#define ML_ANY		0
#define ML_OBJECT	1
#define ML_ICODE	2

struct modnam ModLangs[] = {
    {"Objct",    1},
    {"ICode",    2},
    {"PCode",    3},
    {"CCode",    4},
    {"CblCode",  5},
    {"FrtnCode", 6},
    {"", 0}
};

#define mktypelang(type,lang)	(((type)<<8)|(lang))

/* Module Attribute values */
#define MA_REENT	0x80
#define MA_GHOST	0x40
#define MA_SUPER	0x20

struct modnam ModAtts[] = {
    {"ReEnt", 0x80},
    {"Ghost", 0x40},
    {"SupStat", 0x20},
    {"",0}
};
