#ifndef EMP_IR_PRINT_HELPER_H__
#define EMP_IR_PRINT_HELPER_H__
#include "emp-ir/constants_only.h"
#include <cstdlib>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

void EMP_setup() {
	fprintf(stdout,
			"// Header start\n"
			"version 1.0.0;\n"
			"field characteristic 2 degree 1;\n"
			"relation\n"
			"gate_set: boolean;\n"
			"features: simple;\n"
			"// Header end\n"
			"\n"
			"@begin\n");

}
void EMP_finalize() {
	fprintf(stdout,"\n@end\n");
}
const static int PUBLIC = 0;
class Field { public:
	bool value, ispublic;
	uint64_t index;
	static uint64_t global_cnt;
	Field(bool x = 0, int source = PUBLIC) {
		if(source == WITNESS) {
			index = global_cnt++;
			ispublic = false;
			fprintf(stdout, "$%" PRIu64 " <- @short_witness;\n", index);
		}
		else if (source == INSTANCE) {
			index = global_cnt++;
			ispublic = false;
			fprintf(stdout, "$%" PRIu64 " <- @instance;\n", index);
		}
		else {
			value = x; 
			ispublic = true;
		}
	}
	Field operator&(const Field& rhs) const {
		if (ispublic and value) {
			return rhs;
		} else if (rhs.ispublic and rhs.value) {
			return *this;
		} else if((ispublic and not value) or (rhs.ispublic and not rhs.value)) {
			return Field(false, true);
		} else {
			Field res;
			res.value = value and rhs.value;
			res.index = global_cnt++;
			res.ispublic = false;
			fprintf(stdout, "$%" PRIu64 " <- @and($%" PRIu64 ", $%" PRIu64 ");\n", res.index, index, rhs.index);
			return res;
		}
	}

	Field operator^(const Field& rhs) const {
		if (ispublic and value) {
			return !rhs;
		} else if (rhs.ispublic and rhs.value) {
			return !(*this);
		} else if(ispublic and not value) {
			return rhs;
		} else if (rhs.ispublic and not rhs.value) {
			return *this;
		} else {
			Field res;
			res.value = value xor rhs.value;
			res.index = global_cnt++;
			res.ispublic = false;
			fprintf(stdout, "$%" PRIu64 " <- @xor($%" PRIu64 ", $%" PRIu64 ");\n", res.index, index, rhs.index);
			return res;
		}
	}
	Field operator!() const {
		if(ispublic) {
			return Field(not value, true);
		} else {
			Field res;
			res.value = not value;
			res.index = global_cnt++;
			res.ispublic = false;
			fprintf(stdout, "$%" PRIu64 " <- @not($%" PRIu64 ");\n", res.index, index);
			return res;
		}
	}
	Field select(Field a, Field b) const {
		return ((b ^ *this) & a) ^ (*this);
	}
	Field operator==(Field rhs) const {
		return !(*this ^ rhs);
	}
	void assert_zero() const {
		fprintf(stdout,  "@assert_zero($%" PRIu64 ");\n", index);
	}
};
uint64_t Field::global_cnt = 0;

Field EMP_next_input(int source) {
	return Field(false, source);
}

#include "emp-ir/bool/exec_helper.h"
int main(int argc, char** argv) {
  EMP_setup();
  emp_main();
  EMP_finalize();
}
#endif// EMP_IR_PRINT_HELPER_H__
