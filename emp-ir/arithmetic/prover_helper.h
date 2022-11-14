#ifndef EMP_IR_PROVER_HELPER_H__
#define EMP_IR_PROVER_HELPER_H__
#include "emp-tool/emp-tool.h"
#include "emp-zk/emp-zk.h"
#include "emp-ir/constant.h"
#include <vector>
using std::vector;

vector<NetIO*> netios;
static ZKFpExecPrv<NetIO> * prot = nullptr;

class Field { public:
	__uint128_t value;
	Field(uint64_t x = 0, int source = PUBLIC) {
		if(source == PUBLIC)
			value = prot->pub_label(x);
		else
			prot->feed(value, x);
	}
	void assert_zero() const {
		__uint128_t tmp = value;
		prot->reveal_check_zero(&tmp, 1);
	}
	Field operator*(const Field& rhs) const {
		Field res;
		res.value = prot->mul_gate(value, rhs.value);
		return res;
	}
	Field operator+(const Field& rhs) const {
		Field res;
		res.value = prot->add_gate(value, rhs.value);
		return res;
	}
	Field operator*(uint64_t v) const {
		Field res;
		res.value = prot->mul_const_gate(value, v);
		return res;
	}
	Field operator+(uint64_t value) const {
		return (*this) + Field(value, PUBLIC);
	}
	Field operator-(const Field& rhs) const {
		return (*this) + rhs*(PRIME - 1);
	}
	Field select(Field a, Field b) const {
		return (b - *this) * a + (*this);
	}
	Field operator==(Field rhs) const { //const Field& rhs) const {
		Field diff = (*this) - rhs;
		uint64_t clear_diff = HIGH64(diff.value);
		uint64_t clear_res = (clear_diff != 0) ? 1: 0;
		uint64_t clear_diff_inv = compute_inverse(clear_diff);
		Field diff_inv(clear_diff_inv, ALICE);
		Field res(clear_res, ALICE);
		Field assert_left = res * diff * (diff_inv + Field(1, PUBLIC));
		Field assert_right = res + diff;
		(assert_left - assert_right).assert_zero();
		((res - Field(1, PUBLIC))*res).assert_zero();
		return Field(1, PUBLIC) - res;
	}
};

void EMP_setup(int party, const char * ip, int threads, int port) {
	for(int i = 0; i < threads; ++i)
		netios.push_back(new NetIO(party == ALICE?nullptr:ip,port, true));
	prot = new ZKFpExecPrv<NetIO>(netios.data(), threads);
}
void EMP_finalize() {
	delete prot; 
}
#include "emp-ir/arithmetic/zk_helper.h"
#include "emp-ir/arithmetic/exec_helper.h"
int main(int argc, char** argv) {
	int party = 1;
	int port = atoi(argv[2]);
	EMP_setup(party, argv[1], atoi(argv[3]), port);
	load_instance(argv[4]);
	load_witness(argv[5]);
	emp_main();
	EMP_finalize();
}
#endif// EMP_IR_ZK_HELPER_H__
