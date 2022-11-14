#ifndef EMP_IR_ZK_HELPER_H__
#define EMP_IR_ZK_HELPER_H__
#include "emp-tool/emp-tool.h"
#include "emp-zk/emp-zk.h"
#include "emp-ir/constant.h"//bool/emp-ir0-printer.h"
#include <vector>
using std::vector;

vector<BoolIO<NetIO>*> netios;
static ZKBoolCircExecVer<BoolIO<NetIO>> * c_pvr = nullptr;
static ZKVerifier<BoolIO<NetIO>> * p_pvr = nullptr;

class Field { public:
	block b;	
	Field (bool value = false, int source = PUBLIC) {
		p_pvr->feed(&b, source, &value, 1);
	}
	Field operator &(const Field& rhs) const {
		Field res;
		res.b = c_pvr->and_gate(b, rhs.b);
		return res;
	}
	Field operator ^(const Field& rhs) const {
		Field res;
		res.b = c_pvr->xor_gate(b, rhs.b);
		return res;
	}
	Field operator==(const Field& rhs) const {
		return !(*this ^ rhs);
	}
	Field operator!() const {
		Field res;
		res.b = c_pvr->not_gate(b);
		return res;
	}
	Field select(const Field & select, const Field & new_v) const{
		Field tmp = *this;
		tmp = tmp ^ new_v;
		tmp = tmp & select;
		return *this ^ tmp;
	}
	void assert_zero() const {
		bool res;
		p_pvr->reveal(&res, PUBLIC, &b, 1);
		if(res == true) {
			cerr<<"assert_zero failed!\n";
		}
	}

};

void EMP_setup(int party, const char * ip, int threads, int port) {
	for(int i = 0; i < threads; ++i)
		netios.push_back(new BoolIO<NetIO>(new NetIO(party == ALICE?nullptr:ip,port, true), party==ALICE));
	c_pvr = new ZKBoolCircExecVer<BoolIO<NetIO>>();
	p_pvr = new ZKVerifier<BoolIO<NetIO>>(netios.data(), threads, c_pvr, nullptr);	
}
void EMP_finalize() {
	delete c_pvr;
	delete p_pvr; 
	bool cheat = CheatRecord::cheated();
	if(cheat)error("cheat!\n");
	for(size_t i = 0; i < netios.size(); ++i) {
		delete netios[i]->io;
		delete netios[i];
	}
}
#include "emp-ir/bool/zk_helper.h"
#include "emp-ir/bool/exec_helper.h"
int main(int argc, char** argv) {
	int party = 2;
	int port = atoi(argv[2]);
	EMP_setup(party, argv[1], atoi(argv[3]), port);
	load_instance(argv[4]);
	emp_main();
	EMP_finalize();
}
#endif// EMP_IR_ZK_HELPER_H__
