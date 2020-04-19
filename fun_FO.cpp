#include <iostream>

static uint32_t fun_FO(uint32_t in, int index) {
	uint16_t left, right;
	left = (uint16_t)(in >> 16);
	right = (uint16_t)in;
	left ^= KOi1[index];
    left = FI(left, KIi1[index]);
    left ^= right;
    right ^= KOi2[index];
    right = FI(right, KIi2[index]);
    right ^= left;
    left ^= KOi3[index];
    left = FI(left, KIi3[index]);
    left ^= right;
    in = (((uint32_t)right) << 16) + left;
}