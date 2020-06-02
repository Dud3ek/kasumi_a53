#include <iostream>
#include <iomanip>
#include <string.h>
#include <stdlib.h>
#include "a53.h"
#include "kasumi.h"

using namespace std;

//exp means expected, got means what we got, rest pretty clear
//found test sets on some examplary implementation of other people

void printer(int which, const char* label, u8* vector, int length) //print on screen
{
    cout << " block" << which << " " << label << " = ";
    int i;
    for (i = 0; i < length; i++) {
        cout << hex << std::setw(2) << setfill('0') << (int)vector[i] << dec;
    }
}

void check(int which, u8* exp, u8* got) // check corectness function
{
    printer(which, "exp", exp, 15);
    printer(which, "got", got, 15);
    int err = 0;
    int i;
    for (i = 0; i < 15; i++) {
        if (exp[i] == got[i]) continue;
        err = 1;
        break;
    }
    if (err) {
        cout << " ERROR" << endl;
    }
    else {
        cout << " ok" << endl;
    }
}

int unhex(const char c)  //hex to decimal value
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    cout << "oops" << endl;
    exit(1);
}

int seti(const char* src) //set as 16 bit int
{
    int r = 0;
    for (; *src != 0; src++) {
        r = (r << 4) + unhex(*src);
    }
    return r;
}

void setu8(u8* dst, const char* src) //set as 8 bit uint
{
    for (; *src != 0; src += 2) {
        *dst++ = (unhex(src[0]) << 4) | (unhex(src[1]));
    }
}




int main()
{
    int i, keylen;
    unsigned int count;
    u8 key[16], exp1[15], exp2[15], got1[15], got2[15];



    printf("Kasumi part \n");

    u8 text[8] = {
        //0x12, 0x34, 0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF,
        0xFE, 0xDC, 0xBA, 0x09, 0x87, 0x65, 0x43, 0x21,
    };

    u8 keykasumi[16] = {
        0x99, 0x00, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    };

    KeySchedule(keykasumi);

    printf("plain text:");
    for (i = 0; i < 8; i++)
        printf("%02x ", text[i]);
    printf("\n");

    Kasumi(text);

    printf("cipher text:");
    for (i = 0; i < 8; i++)
        printf("%02x ", text[i]);
    printf("\n");

    KasumiDecrypt(text);

    printf("decrypted text:");
    for (i = 0; i < 8; i++)
        printf("%02x ", text[i]);
    printf("\n");

    const char* data[52] = {
        "2BD6459F82C5BC00", "24F20F", "889EEAAF9ED1BA1ABBD8436232E440", "5CA3406AA244CF69CF047AADA2DF40",
        "952C49104881FF48", "061527", "AB7DB38A573A325DAA76E4CB800A40", "4C4B594FEA9D00FE8978B7B7BC1080",
        "EFA8B2229E720C2A", "33FD3F", "0E4015755A336469C3DD8680E30340", "6F10669E2B4E18B042431A28E47F80",
        "3451F23A43BD2C87", "0E418C", "75F7C4C51560905DFBA05E46FB54C0", "192C95353CDF979E054186DF15BF00",
        "CAA2639BE82435CF", "2FF229", "301437E4D4D6565D4904C631606EC0", "F0A3B8795E264D3E1A82F684353DC0",
        "7AE67E87400B9FA6", "2F24E5", "F794290FEF643D2EA348A7796A2100", "CB6FA6C6B8A705AF9FEFE975818500",
        "58AF69935540698B", "05446B", "749CA4E6B691E5A598C461D5FE4740", "31C9E444CD04677ADAA8A082ADBC40",
        "017F81E5F236FE62", "156B26", "2A6976761E60CC4E8F9F52160276C0", "A544D8475F2C78C35614128F1179C0",
        "1ACA8B448B767B39", "0BC3B5", "A4F70DC5A2C9707F5FA1C60EB10640", "7780B597B328C1400B5C74823E8500",
    };

    for (i = 32; i >= 0; i -= 4) {
        cout << "test set " << (1 + i / 4) << endl;
        setu8(key, data[i]);
        keylen = strlen(data[i]) * 4;
        count = seti(data[i + 1]);
        GSM(key, keylen, count, got1, got2);
        setu8(exp1, data[i + 2]);
        setu8(exp2, data[i + 3]);
        check(1, exp1, got1);
        check(2, exp2, got2);
    }

    return 0;
}

