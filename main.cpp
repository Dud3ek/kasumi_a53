#include<iostream>

#define ROL16(a,b) (unsigned short)((a<<b)|(a>>(16-b)))

static unsigned short KLi1[8], KLi2[8];
static unsigned short KOi1[8], KOi2[8], KOi3[8];
static unsigned short KIi1[8], KIi2[8], KIi3[8];


void Schedule(unsigned short* key)
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



int main(void) {
    
    int i;


    unsigned short key[16] = {
            0x99, 0x00, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
            0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    };


    Schedule(key);

   for (i = 0; i < 8; i++)
       printf("%04x ", KLi1[i]);
   printf("\n");
   for (i = 0; i < 8; i++)
       printf("%04x ", KLi2[i]);
   printf("\n");
   for (i = 0; i < 8; i++)
       printf("%04x ", KOi1[i]);
   printf("\n");
   for (i = 0; i < 8; i++)
       printf("%04x ", KOi2[i]);
   printf("\n");
   for (i = 0; i < 8; i++)
       printf("%04x ", KOi3[i]);
   printf("\n");
   for (i = 0; i < 8; i++)
       printf("%04x ", KIi1[i]);
   printf("\n");
   for (i = 0; i < 8; i++)
       printf("%04x ", KIi2[i]);
   printf("\n");
   for (i = 0; i < 8; i++)
       printf("%04x ", KIi3[i]);
   printf("\n");

    return 0;
}


