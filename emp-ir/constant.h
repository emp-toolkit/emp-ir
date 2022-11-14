#ifndef EMP_IR_CONSTANT_H__
#define EMP_IR_CONSTANT_H__

#include "emp-ir/constants_only.h"

inline uint64_t compute_inverse(uint64_t x) {
	uint64_t pows[61];
	pows[0] = x;
	for(int i = 1; i < 61; ++i)
		pows[i] = mult_mod(pows[i-1], pows[i-1]);
	uint64_t res = x;
	for(int i = 2; i < 61; ++i)
		res = mult_mod(res, pows[i]);
	return res;
}
#endif
