#include "kasumi.h"
#include <stdio.h>

//co is output, cl is length, cc given, ck key build earlier, ca given, cb given, cd given

void KGcore(u8 ca, u8 cb, u32 cc, u8 cd, u8* ck, u8* co, int cl)
{
    Struct1 A; // a 64-bit register that is used within the KGCORE function to hold an intermediate value. 
    Struct1 reg2; // this is our current register used in blocks
    u8 inputkey[16], modifiedkey[16]; 
    u16 blockcounter; // block counter which increments with every our "turn"
    int i, n;

    for (i = 0; i < 16; ++i) 
    {
        inputkey[i] = ck[i];  //copying a key
    }
        

    reg2.b32[0] = reg2.b32[1] = 0; //current register
    A.b32[0] = A.b32[1] = 0;        //modifier register

    // initialize modifier register and its values
    // different size of inputs cc cb etc so different shifts
    // ce at the end 15 bits all 0's

    A.b8[0] = (u8)(cc >> 24);     
    A.b8[1] = (u8)(cc >> 16);
    A.b8[2] = (u8)(cc >> 8);
    A.b8[3] = (u8)(cc);
    A.b8[4] = (u8)(cb << 3);
    A.b8[4] |= (u8)(cd << 2);
    A.b8[5] = (u8)ca;


    for (n = 0; n < 16; ++n) 
    {
        modifiedkey[n] = (u8)(ck[n] ^ 0x55);  //Kasumi used to construct modified key
    }

    KeySchedule(modifiedkey); //this is 1st use of kasumi on block diagram
    Kasumi(A.b8); 


    blockcounter = 0;
    KeySchedule(inputkey);  //now key schedule for other multiple blocks of kasumi


    while (cl > 0)
    {
        reg2.b32[0] ^= A.b32[0];  
        reg2.b32[1] ^= A.b32[1];
        reg2.b8[7] ^= blockcounter;
 
        Kasumi(reg2.b8);   //block production by kasumi

        if (cl >= 64)   // n should be equal to number of bytes of input data
        {
            n = 8;         // it equals to 8 if cl=>64
        }
        else
        {
            n = (cl + 7) / 8;       // if cl<64 then it goes 7,6,5,4 etc
        }

        for (i = 0; i < n; ++i) 
        {
            *co++ = reg2.b8[i];    //assigning to output
        }


        cl -= 64; // bit counter
        ++blockcounter; //block diagram counter
    }
}

//Reverse of KGcore to get original CO

void KGcoreDecrypt(u8 ca, u8 cb, u32 cc, u8 cd, u8* ck, u8* co, int cl)
{
    Struct1 A; // a 64-bit register that is used within the KGCORE function to hold an intermediate value. 
    Struct1 reg2; // this is our current register used in blocks
    u8 inputkey[16], modifiedkey[16];
    u16 blockcounter, coindex; // block counter which increments with every our "turn"
    int i, n;

    for (i = 0; i < 16; ++i)
    {
        inputkey[i] = ck[i];  //copying a key
    }


    reg2.b32[0] = reg2.b32[1] = 0; //current register
    A.b32[0] = A.b32[1] = 0;        //modifier register

    // initialize modifier register and its values
    // different size of inputs cc cb etc so different shifts
    // ce at the end 15 bits all 0's

    A.b8[0] = (u8)(cc >> 24);
    A.b8[1] = (u8)(cc >> 16);
    A.b8[2] = (u8)(cc >> 8);
    A.b8[3] = (u8)(cc);
    A.b8[4] = (u8)(cb << 3);
    A.b8[4] |= (u8)(cd << 2);
    A.b8[5] = (u8)ca;

    for (n = 0; n < 16; ++n)
    {
        modifiedkey[n] = (u8)(ck[n] ^ 0x55);  //Kasumi used to construct modified key
    }

    KeySchedule(modifiedkey); //this is 1st use of kasumi on block diagram
    Kasumi(A.b8);

    blockcounter = cl / 64;
    coindex = cl / 8;
    KeySchedule(inputkey);  //now key schedule for other multiple blocks of kasumi

    while (cl > 0)
    {
        if (cl >= 64)   // n should be equal to number of bytes of input data
        {
            n = 7;         // it equals to 8 if cl=>64
            cl -= 64;      // counter 
        }

        else if (cl % 64 != 0)
        {
            n = ((cl % 64 + 7) / 8) - 1;       // if remainer is not 0 then it goes 7,6,5,4 etc
            cl -= cl % 64;          //counter to make remainder equal to 0
        }

        for (i = n; i >= 0; --i)
        {
            reg2.b8[i] = co[coindex];    //assigning to output
            coindex--;
        }

        KasumiDecrypt(reg2.b8);

        reg2.b8[7] ^= blockcounter;
        reg2.b32[1] ^= A.b32[1];
        reg2.b32[0] ^= A.b32[0];

        --blockcounter; //block diagram counter
    }



}