#include "kasumi.h"

// define roll bits operation used in kasumi ciphering
#define ROL16(a,b) (u16)((a<<b)|(a>>(16-b)))

//define typedef to deal with little and big endian issues
//we experimented with it and deleting it not affects program for intel
//so conversion is more about working on big endian like Sparc machines

typedef union {
    u32 b32;
    u16 b16[2];
    u8 b8[4];
} doubleWord; //double word 32 bit
typedef union {
    u16 b16;
    u8 b8[2];
} singleWord; //word 16 bit

//subkey arrays KL, KO, KI 

static u16 KLi1[8], KLi2[8];			
static u16 KOi1[8], KOi2[8], KOi3[8];
static u16 KIi1[8], KIi2[8], KIi3[8];

//FI function sixteen bit input is divided into inequal halves of 9 and 7 bit subkey
// then operate on it like specification says

static u16 FI(u16 fi_input, u16 sub)
{
    u16 subkey_nine, subkey_seven;
    static u16 S7[] = {
        54, 50, 62, 56, 22, 34, 94, 96, 38, 6, 63, 93, 2, 18,123, 33,
        55,113, 39,114, 21, 67, 65, 12, 47, 73, 46, 27, 25,111,124, 81,
        53, 9,121, 79, 52, 60, 58, 48,101,127, 40,120,104, 70, 71, 43,
        20,122, 72, 61, 23,109, 13,100, 77, 1, 16, 7, 82, 10,105, 98,
        117,116, 76, 11, 89,106, 0,125,118, 99, 86, 69, 30, 57,126, 87,
        112, 51, 17, 5, 95, 14, 90, 84, 91, 8, 35,103, 32, 97, 28, 66,
        102, 31, 26, 45, 75, 4, 85, 92, 37, 74, 80, 49, 68, 29,115, 44,
        64,107,108, 24,110, 83, 36, 78, 42, 19, 15, 41, 88,119, 59, 3 };
    static u16 S9[] = {
        167,239,161,379,391,334, 9,338, 38,226, 48,358,452,385, 90,397,
        183,253,147,331,415,340, 51,362,306,500,262, 82,216,159,356,177,
        175,241,489, 37,206, 17, 0,333, 44,254,378, 58,143,220, 81,400,
        95, 3,315,245, 54,235,218,405,472,264,172,494,371,290,399, 76,
        165,197,395,121,257,480,423,212,240, 28,462,176,406,507,288,223,
        501,407,249,265, 89,186,221,428,164, 74,440,196,458,421,350,163,
        232,158,134,354, 13,250,491,142,191, 69,193,425,152,227,366,135,
        344,300,276,242,437,320,113,278, 11,243, 87,317, 36, 93,496, 27,
        487,446,482, 41, 68,156,457,131,326,403,339, 20, 39,115,442,124,
        475,384,508, 53,112,170,479,151,126,169, 73,268,279,321,168,364,
        363,292, 46,499,393,327,324, 24,456,267,157,460,488,426,309,229,
        439,506,208,271,349,401,434,236, 16,209,359, 52, 56,120,199,277,
        465,416,252,287,246, 6, 83,305,420,345,153,502, 65, 61,244,282,
        173,222,418, 67,386,368,261,101,476,291,195,430, 49, 79,166,330,
        280,383,373,128,382,408,155,495,367,388,274,107,459,417, 62,454,
        132,225,203,316,234, 14,301, 91,503,286,424,211,347,307,140,374,
        35,103,125,427, 19,214,453,146,498,314,444,230,256,329,198,285,
        50,116, 78,410, 10,205,510,171,231, 45,139,467, 29, 86,505, 32,
        72, 26,342,150,313,490,431,238,411,325,149,473, 40,119,174,355,
        185,233,389, 71,448,273,372, 55,110,178,322, 12,469,392,369,190,
        1,109,375,137,181, 88, 75,308,260,484, 98,272,370,275,412,111,
        336,318, 4,504,492,259,304, 77,337,435, 21,357,303,332,483, 18,
        47, 85, 25,497,474,289,100,269,296,478,270,106, 31,104,433, 84,
        414,486,394, 96, 99,154,511,148,413,361,409,255,162,215,302,201,
        266,351,343,144,441,365,108,298,251, 34,182,509,138,210,335,133,
        311,352,328,141,396,346,123,319,450,281,429,228,443,481, 92,404,
        485,422,248,297, 23,213,130,466, 22,217,283, 70,294,360,419,127,
        312,377, 7,468,194, 2,117,295,463,258,224,447,247,187, 80,398,
        284,353,105,390,299,471,470,184, 57,200,348, 63,204,188, 33,451,
        97, 30,310,219, 94,160,129,493, 64,179,263,102,189,207,114,402,
        438,477,387,122,192, 42,381, 5,145,118,180,449,293,323,136,380,
        43, 66, 60,455,341,445,202,432, 8,237, 15,376,436,464, 59,461 };
 
    subkey_nine = (u16)(fi_input >> 7);
    subkey_seven = (u16)(fi_input & 0x7F);

    subkey_nine = (u16)(S9[subkey_nine] ^ subkey_seven);
    subkey_seven = (u16)(S7[subkey_seven] ^ (subkey_nine & 0x7F));
    subkey_seven ^= (sub >> 9);
    subkey_nine ^= (sub & 0x1FF);
    subkey_nine = (u16)(S9[subkey_nine] ^ subkey_seven);
    subkey_seven = (u16)(S7[subkey_seven] ^ (subkey_nine & 0x7F));
    fi_input = (u16)((subkey_seven << 9) + subkey_nine);



    return(fi_input);
}

//FO function operates on 32 bit value, uses index to identify subkey which use

static u32 FO(u32 fo_input, int ID)
{
    // split into two halves 
    u16 left, right;
    
    left = (u16)(fo_input >> 16);
    right = (u16)fo_input;
    
    left ^= KOi1[ID];
    left = FI(left, KIi1[ID]);
    left ^= right;
    right ^= KOi2[ID];
    right = FI(right, KIi2[ID]);
    right ^= left;
    left ^= KOi3[ID];
    left = FI(left, KIi3[ID]);
    left ^= right;
    fo_input = (((u32)right) << 16) + left;
    
    return(fo_input);
}

// FL function operating on 32 bit input

static u32 FL(u32 fl_input, int ID)
{
    // left and right halves
    u16 left, right, tmp1, tmp2;

    left = (u16)(fl_input >> 16);
    right = (u16)(fl_input);
    tmp1 = (u16)(left & KLi1[ID]);
    right ^= ROL16(tmp1, 1);
    tmp2 = (u16)(right | KLi2[ID]);
    left ^= ROL16(tmp2, 1);
    fl_input = (((u32)left) << 16) + right;

    return(fl_input);
}

// core of kasumi coding, input data is array with 8 bit parts

void Kasumi(u8* datain)
{
    u32 left, right, tmp;
    doubleWord* data;
    int n;
    
    data = (doubleWord*)datain;

    // division into right and left part
    left = (((u32)data[0].b8[0]) << 24) + (((u32)data[0].b8[1]) << 16) + (data[0].b8[2] << 8) + (data[0].b8[3]);

    right = (((u32)data[1].b8[0]) << 24) + (((u32)data[1].b8[1]) << 16) + (data[1].b8[2] << 8) + (data[1].b8[3]);


    n = 0;
    do {
        tmp = FL(left, n);
        tmp = FO(tmp, n++);

        right ^= tmp;

        tmp = FO(right, n);
        tmp = FL(tmp, n++);
        left ^= tmp;

    } while (n <= 7);



    //returning the results
    data[0].b8[0] = (u8)(left >> 24);
    data[1].b8[0] = (u8)(right >> 24);
    data[0].b8[1] = (u8)(left >> 16);
    data[1].b8[1] = (u8)(right >> 16);
    data[0].b8[2] = (u8)(left >> 8); 
    data[1].b8[2] = (u8)(right >> 8);
    data[0].b8[3] = (u8)(left); 
    data[1].b8[3] = (u8)(right);
}

//without unions, theyre not needed here

void KasumiDecrypt(u8* datain)
{
    u32 left, right, tmp;
    u8* data;
    int n;

    data = (u8*)datain;
    left = (((u32)data[0]) << 24) + (((u32)data[1]) << 16) + (data[2] << 8) + (data[3]);
    
    right = (((u32)data[4]) << 24) + (((u32)data[5]) << 16) + (data[6] << 8) + (data[7]);

    n = 7;
    do {
        tmp = FO(right, n);
        tmp = FL(tmp, n--);

        left ^= tmp;

        tmp = FL(left, n);
        tmp = FO(tmp, n--);

        right ^= tmp;
    } while (n >= 0);

    data[0] = (u8)(left >> 24);
    data[4] = (u8)(right >> 24);
    data[1] = (u8)(left >> 16); 
    data[5] = (u8)(right >> 16);
    data[2] = (u8)(left >> 8); 
    data[6] = (u8)(right >> 8);
    data[3] = (u8)(left);      
    data[7] = (u8)(right);
}




void KeySchedule(u8* inputkey)
{
    static u16 Slv[] = {
     0x0123,0x4567,0x89AB,0xCDEF, 0xFEDC,0xBA98,0x7654,0x3210 };
    
    u16 our_key[8], primekey[8];
    singleWord* k16;


    k16 = (singleWord*)inputkey;

    for (int n = 0; n < 8; ++n)
    {
        our_key[n] = (u16)((k16[n].b8[0] << 8) + (k16[n].b8[1]));
    }


    for (int n = 0; n < 8; ++n)
    {
        primekey[n] = (u16)(our_key[n] ^ Slv[n]);
    }


    for (int n = 0; n < 8; ++n)
    {
        KLi1[n] = ROL16(our_key[n], 1);
        KLi2[n] = primekey[(n + 2) & 0x7];
        KOi1[n] = ROL16(our_key[(n + 1) & 0x7], 5);
        KOi2[n] = ROL16(our_key[(n + 5) & 0x7], 8);
        KOi3[n] = ROL16(our_key[(n + 6) & 0x7], 13);
        KIi1[n] = primekey[(n + 4) & 0x7];
        KIi2[n] = primekey[(n + 3) & 0x7];
        KIi3[n] = primekey[(n + 7) & 0x7];
    }
}





/*

void Kasumi(u8* datain)
{
    u32 left, right, tmp;
    u8* data;
    int n;

    data = (u8*)datain;
    // division into right and left part
    left = (((u32)data[0]) << 24) + (((u32)data[1]) << 16)
        + (data[2] << 8) + (data[3]);

    right = (((u32)data[4]) << 24) + (((u32)data[5]) << 16)
        + (data[6] << 8) + (data[7]);

    n = 0;
    do {
        tmp = FL(left, n);
        tmp = FO(tmp, n++);

        right ^= tmp;

        tmp = FO(right, n);
        tmp = FL(tmp, n++);
        left ^= tmp;

    } while (n <= 7);


    //returning the results 
    data[0] = (u8)(left >> 24); data[4] = (u8)(right >> 24);
    data[1] = (u8)(left >> 16); data[5] = (u8)(right >> 16);
    data[2] = (u8)(left >> 8);  data[6] = (u8)(right >> 8);
    data[3] = (u8)(left);       data[7] = (u8)(right);
}

void KasumiDecrypt(u8* datain)
{
    u32 left, right, tmp;
    u8* data;
    int n;

    data = (u8*)datain;
    left = (((u32)data[0]) << 24) + (((u32)data[1]) << 16)
        + (data[2] << 8) + (data[3]);
    right = (((u32)data[4]) << 24) + (((u32)data[5]) << 16)
        + (data[6] << 8) + (data[7]);

    n = 7;
    do {
        tmp = FO(right, n);
        tmp = FL(tmp, n--);

        left ^= tmp;

        tmp = FL(left, n);
        tmp = FO(tmp, n--);

        right ^= tmp;
    } while (n >= 0);

    data[0] = (u8)(left >> 24); data[4] = (u8)(right >> 24);
    data[1] = (u8)(left >> 16); data[5] = (u8)(right >> 16);
    data[2] = (u8)(left >> 8);  data[6] = (u8)(right >> 8);
    data[3] = (u8)(left);       data[7] = (u8)(right);
}


*/

/*
void KeySchedule(u8* key)
{
    static unsigned short Slv[] = {
        0x0123,0x4567,0x89AB,0xCDEF, 0xFEDC,0xBA98,0x7654,0x3210
    };

    unsigned short primekey[8];

    for (int n = 0; n < 8; ++n) {
        primekey[n] = (unsigned short)(key[n] ^ Slv[n]);
    };

    for (int n = 0; n < 8; ++n)
    {
        KLi1[n] = ROL16(key[n], 1);
        KLi2[n] = primekey[(n + 2) & 0x7];
        KOi1[n] = ROL16(key[(n + 1) & 0x7], 5);
        KOi2[n] = ROL16(key[(n + 5) & 0x7], 8);
        KOi3[n] = ROL16(key[(n + 6) & 0x7], 13);
        KIi1[n] = primekey[(n + 4) & 0x7];
        KIi2[n] = primekey[(n + 3) & 0x7];
        KIi3[n] = primekey[(n + 7) & 0x7];
    }

}
*/
//alternative for 16 bit inputs division and without endians issue

/*
void KeySchedule(u16* key)
{
    static unsigned short Slv[] = {
        0x0123,0x4567,0x89AB,0xCDEF, 0xFEDC,0xBA98,0x7654,0x3210
    };

    unsigned short primekey[8];

    for (int n = 0; n < 8; ++n) {
        primekey[n] = (unsigned short)(key[n] ^ Slv[n]);
    };

    for (int n = 0; n < 8; ++n)
    {
        KLi1[n] = ROL16(key[n], 1);
        KLi2[n] = primekey[(n + 2) & 0x7];
        KOi1[n] = ROL16(key[(n + 1) & 0x7], 5);
        KOi2[n] = ROL16(key[(n + 5) & 0x7], 8);
        KOi3[n] = ROL16(key[(n + 6) & 0x7], 13);
        KIi1[n] = primekey[(n + 4) & 0x7];
        KIi2[n] = primekey[(n + 3) & 0x7];
        KIi3[n] = primekey[(n + 7) & 0x7];
    }

}
*/