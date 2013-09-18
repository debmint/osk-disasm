#include "disglobs.h"
#include "userdef.h"
#include "proto.h"



/* ************************************************************************* */
/* 				 68def 					     */
/* ************************************************************************* */

#if(DEVICE==68040 || COPROCESSOR==TRUE)
/* ************************************************************************* */
/* ROUTINE: FPMATCH							     */
/* ************************************************************************* */

int
fpmatch (start)
     int start;
{
    extern error;
    extern char sizechar[];

    int operationword;
    int commandword;

    int type;


    operationword = get16 (start);
    commandword = get16 (start + 2);
    type = bitnum (8, 6, operationword);
    if (bitnum (15, 9, operationword) != 0x79   /* F-Line reserved          */
        || (type == 0x6)        /* type undefined reserved  */
        || (type == 0x7))       /* type undefined reserved  */
    {
        error = TRUE;
        return (-1);
    }
    switch (type)
    {
        case 0:                /* General Instructions  */
            sizechar[0] = 'x';  /* As a default to prevent a failure 
                                   in disasm68.c */

            return (200);       /* A SPECIFIC DETERMINATION WILL 
                                   BE MADE IN DISASM68.C        */
        case 1:                /* FDBcc,FScc or FTRAPcc */
            if ((bitnum (16, 6, commandword) != 0x0)
                || (bitnum (5, 5, commandword) == 0x1))
            {
                error = TRUE;
                return (-1);
            }
            if (bitnum (5, 3, operationword) == 0x1)
            {
                return (204);
            }
            else if (bitnum (5, 3, operationword) == 0x7)
            {
                if (bitnum (2, 0, operationword) == 0x2)
                {
                    sizechar[0] = 'w';
                    return (214);
                }
                else if (bitnum (2, 0, operationword) == 0x3)
                {
                    sizechar[0] = 'l';
                    return (214);
                }
                else if (bitnum (2, 0, operationword) == 0x4)
                    return (213);
            }
            sizechar[0] = 'b';
            return (212);
        case 2:                /* FBcc.W */
        case 3:                /* FBcc.L */
            if (bitnum (5, 5, get16 (start)) == 1)
            {
                error = TRUE;
                return (-1);
            }
            if (type == 2 && get16 (start + 2) == 0x0
                && bitnum (5, 0, operationword) == 0x0)
                return (209);
            else if (type == 2)
                sizechar[0] = 'w';
            else
                sizechar[0] = 'l';
            return (203);
        case 4:                /* FSAVE */
            return (211);
        case 5:                /* FRESTORE */
            return (210);
    }
}
#endif

/* ************************************************************************* */


/* ************************************************************************* */
/* ROUTINE: TABLEMATCH							     */
/* ************************************************************************* */

OPSTRUCTURE *
tablematch (opword, entry)
     int opword;
     int entry;
{
    extern OPSTRUCTURE syntax1[];
    extern int error;
    int i,
      j,
      Bmatch,
      a,
      b,
      c,
      maxinstr;


    Bmatch = 1;
    for (j = 15; j > -1; j--)
    {
        a = syntax1[entry].opwordstr[j] != 'x';
        b = (syntax1[entry].opwordstr[j] == '1' && !(opword & 0x0001));
        c = (syntax1[entry].opwordstr[j] == '0' && (opword & 0x0001));
        if (a && (b || c))
        {
            Bmatch = 0;         /* MATCH FLAG IS SET TO FALSE */
            break;
        }
        opword = opword >> 1;
    }

    if (Bmatch == 0)
    {
        error = TRUE;
    }

    return &syntax1[entry];
}


/* ************************************************************************* */
/* ROUTINE: BITNUM							     */
/* ************************************************************************* */

int
bitnum (startbit, endbit, opword)
     int startbit,
       endbit,
       opword;
{
    int shifted_word,
      i,
      value;
    value = 0;

    for (i = 0; i <= (startbit - endbit); i++)
    {
        shifted_word = (opword >> i + endbit);
        if ((shifted_word & 0x1) == 1)
            value = value + (1 << i);
    }
    return (value);
}


/* ************************************************************************* */
/*			EFFECTIVE ADDRESS TABLE				     */
/* ************************************************************************* */



struct EA_TABLE
{
    char EAentry[7];
};


EA_TABLE EffectiveAddrTable[] = {
    "000xxx",
    "001xxx",
    "010xxx",
    "011xxx",
    "100xxx",
    "101xxx",
    "110xxx",
    "111000",
    "111001",
    "111010",
    "111011",
    "111100"
};


/* ************************************************************************* */
/* ROUTINE: illegalEA							     */
/* ************************************************************************* */

int
illegalEA (EAentry, opword, extensionword)
     int EAentry;               /* FROM TABLE CONSISTING OF MASKS */
     int opword;
     int extensionword;
{
    extern EAALLOWED_TYPE EAtype[];
    int i,
      j,
      matchattempt,
      Bmatch,
      allowableEA,
      a,
      b,
      c;
    char EAchar;

    allowableEA = EAtype[EAentry].allowableEA;
    matchattempt = opword;
    for (i = 0; i < 12; i++)
    {
        Bmatch = 1;
        for (j = 5; j > -1; j--)
        {
            EAchar = EffectiveAddrTable[i].EAentry[j];
            a = (EAchar != 'x');
            b = ((EAchar == '1') && !(opword & 0x0001));
            c = ((EAchar == '0') && (opword & 0x0001));
            if (a && (b || c) || (((allowableEA >> (11 - i)) & 0x1) == 0))
            {
                Bmatch = 0;
                break;
            }
            opword = opword >> 1;
        }

        if (Bmatch == 1)
        {
            if (i == 6 || i == 10)
            {
#if (DEVICE>=68020)
                if ((bitnum (8, 8, extensionword) == 1) &&
                    ((bitnum (5, 4, extensionword) == 0
                      || bitnum (3, 3, extensionword) != 0)
                     || (bitnum (6, 6, extensionword) == 1
                         && bitnum (2, 0, extensionword) > 3)
                     || (bitnum (6, 6, extensionword) == 1
                         && bitnum (5, 4, extensionword) == 0)
                     || (bitnum (6, 6, extensionword) == 0
                         && bitnum (2, 0, extensionword) == 4)))
                {
                    return (255);
                }
#else
                if (bitnum (8, 8, extensionword) == 1)
                {
                    return (255);
                }
#endif
            }
            return (i);
        }
        opword = matchattempt;
    }                           /* try another EAentry */
    return (255);
}



/* ************************************************************************* */
/* ROUTINE: printEA							     */
/* ************************************************************************* */

printEA (EAentry, EAstartpoint, sizechar, opword)
     int EAentry,
       EAstartpoint;
     char sizechar;
     int opword;
{
    int d16,
      d8,
      bd,
      od,
      bdsize,
      odsize,
      extensionword;
    int extension2,
      immediatedata;
    static int scale[4] = { 1, 2, 4, 8 };
    int scalevalue;
    static char RegType[2] = { 'D', 'A' };
    static char WLsize[2] = { 'W', 'L' };

#if(DEVICE==68040 || COPROCESSOR==TRUE)
    int f0_fp,
      f1_fp,
      f2_fp;

#endif

//    extensionword = get16 (EAstartpoint);
//    bdsize = 0;
//    odsize = 0;
//    switch (EAentry)
//    {
//        case 0:
//            printf ("D%d", bitnum (2, 0, opword));
//            return;
//        case 1:
//            printf ("A%d", bitnum (2, 0, opword));
//            return;
//        case 2:
//            printf ("(A%d)", bitnum (2, 0, opword));
//            return;
//        case 3:
//            printf ("(A%d)+", bitnum (2, 0, opword));
//            return;
//        case 4:
//            printf ("-(A%d)", bitnum (2, 0, opword));
//            return;
//        case 5:
//            printf ("(%c%x,A%d)", HEXDEL, extensionword,
//                    bitnum (2, 0, opword));
//            return;
//        case 6:
//        case 10:
//            switch (bitnum (8, 8, extensionword))
//            {
//                case 0:        /*BIT 8=0       */
//                    if (EAentry == 6)
//                        printf ("(%c%x,A%d,", HEXDEL,
//                                bitnum (7, 0, extensionword),
//                                bitnum (2, 0, opword));
//                    else
//                        printf ("(%c%x,PC,", HEXDEL,
//                                bitnum (7, 0, extensionword));
//                    printf ("%c%d.%c*%d)",
//                            RegType[bitnum (15, 15, extensionword)],
//                            bitnum (14, 12, extensionword),
//                            WLsize[bitnum (11, 11, extensionword)],
//                            scale[bitnum (10, 9, extensionword)]);
//                    return;
//                case 1:        /*BIT 8=1       */
//                    switch (bitnum (5, 4, extensionword))
//                    {
//                        case 1:
//                            bdsize = 0;
//                            bd = 0;
//                            break;
//                        case 2:
//                            bdsize = 2;
//                            bd = get16 (EAstartpoint + 2);
//                            break;
//                        case 3:
//                            bdsize = 4;
//                            bd = get32 (EAstartpoint + 2);
//                            break;
//                    }
//                    if (bitnum (2, 0, extensionword) == 0)
//                    {
//                        if (EAentry == 6)
//                        {
//                            printf ("($%x,", bd);
//                            if (bitnum (7, 7, extensionword))
//                                printf ("Z");
//                            printf ("A%d,", bitnum (2, 0, opword));
//                        }
//                        else
//                        {
//                            printf ("($%x,", bitnum (7, 0, extensionword));
//                            if (bitnum (7, 7, extensionword))
//                                printf ("Z");
//                            printf ("PC,");
//                        }
//                        if (bitnum (6, 6, extensionword))
//                            printf ("Z");
//                        printf ("%c%d.%c*%d)",
//                                RegType[bitnum (15, 15, extensionword)],
//                                bitnum (14, 12, extensionword),
//                                WLsize[bitnum (11, 11, extensionword)],
//                                scale[bitnum (10, 9, extensionword)]);
//                        return;
//                    }
//                    printf ("([%c%x,", HEXDEL, bd);
//                    if (bitnum (7, 7, extensionword))
//                        printf ("Z");
//                    if (bitnum (5, 3, opword) == 6)
//                        printf ("A%d", bitnum (2, 0, opword));
//                    else
//                        printf ("PC");
//                    if (bitnum (2, 2, extensionword))
//                        printf ("],");
//                    else
//                        printf (",");
//                    if (bitnum (6, 6, extensionword))
//                        printf ("Z");
//                    if (bitnum (15, 15, extensionword))
//                        printf ("A");
//                    else
//                        printf ("D");
//                    printf ("%d.%c*%d", bitnum (14, 12, extensionword),
//                            WLsize[bitnum (11, 11, extensionword)],
//                            scale[bitnum (10, 9, extensionword)]);
//                    switch (bitnum (1, 0, extensionword))
//                    {
//                        case 1:
//                            od = 0;
//                            break;
//                        case 2:
//                            od = get16 (EAstartpoint + 2 + bdsize);
//                            break;
//                        case 3:
//                            od = get32 (EAstartpoint + 2 + bdsize);
//                            break;
//                    }
//                    if (bitnum (2, 2, extensionword))
//                        printf (",%c%x)", HEXDEL, od);
//                    else
//                        printf ("],%c%x)", HEXDEL, od);
//                    return;
//            }
//        case 7:
//            printf ("(%c%x).W", HEXDEL, extensionword);
//            return;
//        case 8:
//            extensionword = extensionword << 16;
//            extension2 = get16 (EAstartpoint + 2);
//            extensionword = (extensionword | extension2);
//            printf ("(%c%x).L", HEXDEL, extensionword);
//            return;
//        case 9:
//            printf ("(%c%x,PC)", HEXDEL, extensionword);
//            return;
//        case 11:
//            switch (sizechar)
//            {
//                case 'b':
//                case 'B':
//                    immediatedata = (get16 (EAstartpoint) & 0xff);
//                    break;
//                case 'w':
//                case 'W':
//                    immediatedata = get16 (EAstartpoint);
//                    break;
//                case 'l':
//                case 'L':
//                    immediatedata = get32 (EAstartpoint);
//                    break;
//#if(DEVICE==68040 || COPROCESSOR==TRUE)
//                default:
//                    f0_fp = get32 (EAstartpoint);
//                    f1_fp = get32 (EAstartpoint + 4);
//                    f2_fp = get32 (EAstartpoint + 8);
//                    switch (sizechar)
//                    {
//                        case 's':
//                        case 'S':
//                            if (bitnum (31, 31, f0_fp))
//                                printf ("-");
//                            printf ("1_%x_E_%x", (bitnum (22, 0, f0_fp) << 1),
//                                    bitnum (30, 23, f0_fp) - 127);
//                            break;
//                        case 'd':
//                        case 'D':
//                            if (bitnum (31, 31, f0_fp))
//                                printf ("-");
//                            printf ("1_%x%x_E_%d", bitnum (19, 0, f0_fp),
//                                    bitnum (31, 0, f1_fp), bitnum (30, 20,
//                                                                   f0_fp) -
//                                    1023);
//                            break;
//                        case 'x':
//                        case 'X':
//                            if (bitnum (31, 31, f0_fp))
//                                printf ("-");
//                            printf ("%d_%8x%8x_E%d", bitnum (31, 31, f1_fp),
//                                    (bitnum (30, 0, f1_fp) << 1), bitnum (31,
//                                                                          0,
//                                                                          f2_fp),
//                                    bitnum (30, 16, f0_fp) - 16383);
//                            break;
//                        case 'p':
//                        case 'P':
//                            printf ("#$");
//                            if (bitnum (31, 31, f0_fp))
//                                printf ("-");
//                            printf ("%x.%x%x_E", bitnum (3, 0, f0_fp), f1_fp,
//                                    f2_fp);
//                            if (bitnum (30, 30, f0_fp))
//                                printf ("-");
//                            printf ("%x", bitnum (27, 16, f0_fp));
//                            break;
//                    }
//                    return;
//#endif
//            }
//            printf ("#%c%x", HEXDEL, immediatedata);
//            return;
//    }
}

/* ************************************************************************* */
/* ROUTINE: sizeEA							     */
/* ************************************************************************* */



int
sizeEA (EAentry, EAstartpoint, sizechar)
     int EAentry,
       EAstartpoint;
     char sizechar;
{
    int bdsize,
      odsize,
      extensionword;

//    extensionword = get16 (EAstartpoint);
//    switch (EAentry)
//    {
//        case 0:
//            return (0);
//        case 1:
//            return (0);
//        case 2:
//            return (0);
//        case 3:
//            return (0);
//        case 4:
//            return (0);
//        case 5:
//            return (2);
//        case 6:
//        case 10:
//            switch (bitnum (8, 8, extensionword))
//            {
//                case 0:
//                    return (2);
//                case 1:
//                    switch (bitnum (5, 4, extensionword))
//                    {
//                        case 1:
//                            bdsize = 0;
//                            break;
//                        case 2:
//                            bdsize = 2;
//                            break;
//                        case 3:
//                            bdsize = 4;
//                            break;
//                    }
//                    if (bitnum (2, 0, extensionword) == 0)
//                        return (bdsize + 2);
//                    switch (bitnum (1, 0, extensionword))
//                    {
//                        case 1:
//                            odsize = 0;
//                            break;
//                        case 2:
//                            odsize = 2;
//                            break;
//                        case 3:
//                            odsize = 4;
//                            break;
//                    }
//                    return (2 + bdsize + odsize);
//            }
//        case 7:
//            return (2);
//        case 8:
//            return (4);
//        case 9:
//            return (2);
//        case 11:
//            switch (sizechar)
//            {
//                case 'b':
//                case 'B':
//                    return (2);
//                case 'w':
//                case 'W':
//                    return (2);
//                case 'l':
//                case 'L':
//                case 's':
//                case 'S':
//                    return (4);
//#if(DEVICE==68040 || COPROCESSOR==TRUE)
//                case 'd':
//                case 'D':
//                    return (8);
//                case 'x':
//                case 'X':
//                case 'p':
//                case 'P':
//                    return (12);
//#endif
//            }
//    }
}
