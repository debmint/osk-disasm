/* Module Type/Language values */
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

#define ML_ANY		0
#define ML_OBJECT	1
#define ML_ICODE	2

#define mktypelang(type,lang)	(((type)<<8)|(lang))

/* Module Attribute values */
#define MA_REENT	0x80
#define MA_GHOST	0x40
#define MA_SUPER	0x20
