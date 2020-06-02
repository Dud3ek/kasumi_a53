#include "kasumi.h"
#include <cstdlib>
#include "kgcore.h"

// BuildKey is supposed to build a key of length 128 bit, this is the exact 
// length of key which our function kgcore needs


static u8* BuildKey(u8* inputkey, int keylength)
{
    static u8 ck[16]; // a 128-bit input to the KGCORE function. 
    u8 mask[] = { 0x1,0x3,0x7,0xF,0x1F,0x3F,0x7F,0xFF };

    int restofdivision;
    int i, n;


    i = (keylength + 7) / 8; // rounding 
    
    
    if (i > 16) 
    {
        i = 16; //set limit to 128 bits
    }

    
    
    for (n = 0; n < i; ++n) 
    {
        ck[n] = inputkey[n];  // copy key
    }


    restofdivision = keylength % 8; //need for checking if key is odd or even

    if (keylength < 128)
    {
        n = 0;
        if (restofdivision) //byte limits checking
        {
            ck[i - 1] &= mask[restofdivision];  // operation executed when our key is not equal bytes
            ck[i - 1] += ck[0] << restofdivision;
            
            while (i < 16)
            {
                ck[i] = (ck[n] >> (8 - restofdivision)) + (ck[n + 1] << restofdivision);
                ++n;
                ++i;
            }

        }
        else
            while (i < 16) 
            {
                ck[i++] = ck[n++];
            }
                
    }
    return(ck);
}

// inputkey, legth of it, count (input data), DownLink transmission block, UPlink transmission block

void GSM( u8 *inputkey, int keylength, int count, u8 *DLblock, u8 *UPblock )
{
    u8* ck; // CK key, its input to our kgcore 
    u8 data[32]; // output of kgcore to DL and UP blocks
    int i;

    ck=BuildKey( inputkey, keylength ); // BuildKey our Kc (CK) cipher key


    KGcore( 0x0F, 0, count, 0, ck, data, 228 ); // 0x0F is typical representation for GSM type of transmission
    // others got other parameters, specification tell about it briefly
    
    for( i=0; i<15; ++i )
    {
        DLblock[i] = data[i];
        UPblock[i] = (data[i+14]<<2) + (data[i+15]>>6);
    }
    // final look of Downlink and Uplink blocks
    DLblock[14] &= 0xC0;  
    UPblock[14] &= 0xC0;
} 

