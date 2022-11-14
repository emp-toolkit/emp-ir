#include <cstdio>
#include <iostream>

#include <wtk/bolt/Backend.h>
#include <emp-zk/emp-zk.h>
using namespace wtk;
using namespace emp;

class EMPBoolBackend: public wtk::bolt::Backend<block, uint8_t> { public:
	int party, port;
	vector<BoolIO<NetIO>*> ios;
	CircuitExecution * boolzk;
	EMPBoolBackend(const char * ip, int port, int party,
			size_t threads = 1): Backend(2, true), party(party), port(port) {
		for(size_t i = 0; i < threads; ++i)
			ios.push_back(new BoolIO<NetIO>(new NetIO(party == ALICE?nullptr:ip,port, true), party==ALICE));
		setup_zk_bool<BoolIO<NetIO>>(ios.data(), threads, party);
		boolzk = CircuitExecution::circ_exec;
	}

	~EMPBoolBackend(){
		if(CheatRecord::cheated()) {
			std::cout <<"Protocol Cheat Detected!"<<endl;
		}
		finalize_zk_bool<BoolIO<NetIO>>();
		for(size_t i = 0; i < ios.size(); ++i) {
			delete ios[i]->io;
			delete ios[i];
		}
		std::cout <<"Proof accepted!\n";
	}

	void addGate(block* const out,
			block const* const left, block const* const right) final {
		error("not supported!");
	}

	void mulGate(block* const out,
			block const* const left, block const* const right) final {
		error("not supported!");
	}

	void addcGate(block* const out,
			block const* const left, uint8_t const right) final {
		error("not supported!");
	}

	void mulcGate(block* const out,
			block const* const left, uint8_t const right) final {
		error("not supported!");
	}

	void xorGate(block* const out,
			block const* const left, block const* const right) final {
		*out = boolzk->xor_gate(*left, *right);
	}

	void andGate(block* const out,
			block const* const left, block const* const right) final {
		*out = boolzk->and_gate(*left, *right);
	}

	void notGate(block* const out, block const* const left) final {
		*out = boolzk->not_gate(*left);
	}

	void copy(block* const out, block const* const left) final {
		*out = *left;
	}

	void assign(block* const out, uint8_t const left) final {
		instance(out, left == 1);
	}

	void instance(block* const out, uint8_t const left) final {
		bool bool_value = (left == 1);
		ProtocolExecution::prot_exec->feed(out, PUBLIC, &bool_value, 1);
	}

	void witness(block* const out, uint8_t const left) final {
		bool bool_value = (left == 1);
		ProtocolExecution::prot_exec->feed(out, ALICE, &bool_value, 1);
	}

	void assertZero(block const* const wire) final {
		bool res = false;
		ProtocolExecution::prot_exec->reveal(&res, PUBLIC, wire, 1);
		if(res)
			error("AssertZero Failed!");
	}

	void caseSelect(block* const selected_bit,
			uint8_t const case_number, block const* const select_wire) final {
		Bit tmp(PUBLIC, case_number == 1);
		Bit tmp2(*select_wire);
		Bit res = (tmp == tmp2);
		*selected_bit = res.bit;
	}

	bool check() final {
		return !CheatRecord::cheated();
	}
};
