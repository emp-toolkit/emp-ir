#ifndef EMP_IR_PRINT_HELPER_H__
#define EMP_IR_PRINT_HELPER_H__
#include "emp-ir/constants_only.h"
#include <cstdlib>
#include <stdio.h>
#include <inttypes.h>


void EMP_setup() {
	fprintf(stdout,
"// Header start\n\
version 1.0.0;\n\
field characteristic %" PRIu64 " degree 1;\n\
relation\n\
gate_set: arithmetic;\n\
features: simple;\n\
// Header end\n\
\n\
@begin\n\
", PRIME);
}

void EMP_finalize() {
	fprintf(stdout,"\n@end\n");
}

const int static PUBLIC = 0;
class Field { public:
	uint64_t index;
	static uint64_t global_cnt;
	Field(uint64_t x = 0, int source = -1) {
		if(source != -1)
			index = global_cnt++;
		if(source == WITNESS)
			fprintf(stdout, "$%" PRIu64 " <- @short_witness;\n", index);
		else if (source == INSTANCE)
			fprintf(stdout, "$%" PRIu64 " <- @instance;\n", index);
		else if (source == PUBLIC)
			fprintf(stdout, "$%" PRIu64 " <- <%" PRIu64 ">;\n", index, x);
	}
	void assert_zero() const {
		fprintf(stdout,  "@assert_zero($%" PRIu64 ");\n", index);
	}
	Field operator*(const Field& rhs) const {
		Field res; res.index = global_cnt++;
		fprintf(stdout, "$%" PRIu64 " <- @mul($%" PRIu64 ", $%" PRIu64 ");\n", res.index, index, rhs.index);
		return res;
	}
	Field operator+(const Field& rhs) const {
		Field res; res.index = global_cnt++;
		fprintf(stdout, "$%" PRIu64 " <- @add($%" PRIu64 ", $%" PRIu64 ");\n", res.index, index, rhs.index);
		return res;	
	}
	Field operator*(uint64_t value) const {
		Field res; res.index = global_cnt++;
		fprintf(stdout, "$%" PRIu64 " <- @mulc($%" PRIu64 ", <%" PRIu64 ">);\n", res.index, index, value);
		return res;
	}
	Field operator+(uint64_t value) const {
		Field res; res.index = global_cnt++;
		fprintf(stdout, "$%" PRIu64 " <- @addc($%" PRIu64 ", <%" PRIu64 ">);\n", res.index, index, value);
		return res;
	}
	Field operator-(const Field& rhs) const {
		return (*this) + rhs*(PRIME - 1);
	}
	Field select(Field a, Field b) const {
		return (b - *this) * a + (*this);
	}
	Field operator==(Field rhs) const { //const Field& rhs) const {
		Field pows[PRIME_SIZE];
		pows[0] = (*this) - rhs;
		for(int i = 1; i < PRIME_SIZE; ++i)
			pows[i] = pows[i-1] * pows[i-1];
		// pow[i] = diff^(2^i).
		Field res;
		if(PRIME_MINUS_1_BITS[0]) res = pows[0];
		else res = Field(1, PUBLIC);
		for(int i = 1; i < PRIME_SIZE; ++i) {
			if (PRIME_MINUS_1_BITS[i])
				res = res * pows[i];
		}
		return Field(1, PUBLIC) - res;
	}
};
uint64_t Field::global_cnt = 0;

Field EMP_next_input(int source) {
	return Field(0, source);
}
#include "emp-ir/arithmetic/exec_helper.h"
int main(int argc, char** argv) {
  EMP_setup();
  emp_main();
  EMP_finalize();
}

#endif// EMP_IR_PRINT_HELPER_H__
