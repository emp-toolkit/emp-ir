#ifndef EMP_EXEC_HELP_H__
#define EMP_EXEC_HELP_H__
#include "emp-ir/flex_vector.h"
#include "emp-ir/AB_helper.h"

template<typename T>
void execute(T & bm, const uint64_t * gate, uint64_t gate_size) {
	for(uint64_t i = 0; i < gate_size; ++i) {
		switch(gate[4*i]) {
			case 1: {
				bm.put(gate[4*i+1], bm[gate[4*i+2]] & bm[gate[4*i+3]]);
				break;
			} case 2: {
				bm.put(gate[4*i+1], bm[gate[4*i+2]] ^ bm[gate[4*i+3]]);
				break;
			} case 3: {
				bm.put(gate[4*i+1],  !bm[gate[4*i+2]]);
				break;
			} case 4: {
				bm.put(gate[4*i+1], bm[gate[4*i+2]]);
				break;
			} case 5: {
				bm.put(gate[4*i+1],  Field(gate[4*i+2], PUBLIC));
				break;
			}
		}
	}
}

void conditional_assert_zero(const Field & b) {
  if(prefixCon.size() == 1)
		b.assert_zero();
  else
		(b & prefixCon.top()).assert_zero();
}
#endif