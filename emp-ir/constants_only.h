#ifndef EMP_IR_CONSTANTS_ONLY_H__
#define EMP_IR_CONSTANTS_ONLY_H__

#include <cstdint>

const static int INSTANCE = 100;
const static int WITNESS = 101;
const static uint64_t PRIME = (1ULL<<61)-1;
const static uint64_t PRIME_SIZE = 61;

const static bool PRIME_MINUS_1_BITS[] = 
{
0, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1, 
};

#endif
