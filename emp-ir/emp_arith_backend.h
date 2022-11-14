#include <cstdio>
#include <iostream>

#include <wtk/bolt/Backend.h>
#include <emp-zk/emp-zk.h>
using namespace wtk;
using namespace emp;

class EMPArithBackend: public wtk::bolt::Backend<__uint128_t, uint64_t> { public:
	int party, port;
	vector<BoolIO<NetIO>*> ios;
	ZKFpExec * arithzk;
	FpPolyProof<BoolIO<NetIO>> * polyzk;
	EMPArithBackend(const char * ip, int port, int party, 
			size_t threads = 1): 
		Backend((1ULL<<61)-1, false), 
		party(party), 
		port(port)
	{
		for(size_t i = 0; i < threads; ++i)
			ios.push_back(new BoolIO<NetIO>(new NetIO(party == ALICE?nullptr:ip,port, true), party==ALICE));
		setup_zk_bool<BoolIO<NetIO>>(ios.data(), threads, party);
		setup_zk_arith<BoolIO<NetIO>>(ios.data(), threads, party);
		arithzk = ZKFpExec::zk_exec;
		polyzk = FpPolyProof<BoolIO<NetIO>>::fppolyproof;
	}

	~EMPArithBackend(){
		for(size_t i = 0; i < ios.size(); ++i) {
			delete ios[i]->io;
			delete ios[i];
		}
	}

	void addGate(__uint128_t* const out,
			__uint128_t const* const left, __uint128_t const* const right) final {
		*out = arithzk->add_gate(*left, *right);
	}

	void mulGate(__uint128_t* const out,
			__uint128_t const* const left, __uint128_t const* const right)  final {
		*out = arithzk->mul_gate(*left, *right);
	}

	void addcGate(__uint128_t* const out,
			__uint128_t const* const left, uint64_t const right) final {
		__uint128_t v = arithzk->pub_label(right);
		*out = arithzk->add_gate(*left, v);
	}

	void mulcGate(__uint128_t* const out,
			__uint128_t const* const left, uint64_t const right) final {
		*out = arithzk->mul_const_gate(*left, right);
	}

	void xorGate(__uint128_t* const out,
			__uint128_t const* const left, __uint128_t const* const right) final {
		error("not supported!");
	}

	void andGate(__uint128_t* const out,
			__uint128_t const* const left, __uint128_t const* const right) final {
		error("not supported!");
	}

	void notGate(__uint128_t* const out, __uint128_t const* const left) final {
		error("not supported!");
	}

	void copy(__uint128_t* const out, __uint128_t const* const left) final {
		*out = *left;
	}

	void assign(__uint128_t* const out, uint64_t const left) final {
		instance(out, left);
	}

	void instance(__uint128_t* const out, uint64_t const left) final {
		*out = arithzk->pub_label(left);
	}

	void witness(__uint128_t* const out, uint64_t const left) final {
		__uint128_t res = 0;
		arithzk->feed(res, left);
		*out = res;
	}

	void assertZero(__uint128_t const* const wire) final {
		__uint128_t tmp = * wire;
		arithzk->reveal_check_zero(&tmp, 1);
	}

	bool check() final {
		return !CheatRecord::cheated();
	}

	void finish() final {
		finalize_zk_arith<BoolIO<NetIO>>();
		finalize_zk_bool<BoolIO<NetIO>>();
	}

	void caseSelect(__uint128_t* const selected_bit,
			uint64_t const case_number, __uint128_t const* const select_wire) final {
		__uint128_t cn = arithzk->pub_label(case_number);//case_number
		__uint128_t diff = cn;
		mulcGate(&diff, &diff, PRIME-1);
		//		__uint128_t local_select_wire = select_wire
		addGate(&diff, &diff, select_wire); // diff = select_wire  - case_number

		uint64_t clear_res = 0, clear_diff_inv = 0;
		if(party == ALICE) {
			uint64_t clear_diff = HIGH64(diff); 
			clear_res = (clear_diff!=0)?1:0;
			clear_diff_inv = compute_inverse(clear_diff);
		}
		__uint128_t diff_inv = 0; arithzk->feed(diff_inv, clear_diff_inv); // diff * p-1
		__uint128_t res = 0; arithzk->feed(res, clear_res); 

		__uint128_t assert_left = arithzk->pub_label(1);
		addGate(&assert_left, &assert_left, &diff_inv);
		mulGate(&assert_left, &assert_left, &diff);
		mulGate(&assert_left, &assert_left, &res); // assert_left = (1 + diff_inv) * diff * res
		__uint128_t assert_right = 0;
		addGate(&assert_right, &res, &diff);      // assert_right = res + diff
		mulcGate(&assert_right, &assert_right, PRIME-1); 
		addGate(&assert_left, &assert_left, &assert_right);
		assertZero(&assert_left); // assert_left - assert_right =?= 0

		__uint128_t second_assert = arithzk->pub_label(PRIME-1);
		addGate(&second_assert, &res, &second_assert);
		mulGate(&second_assert, &res, &second_assert);
		assertZero(&second_assert);// (res - 1) * res =?=0 

		*selected_bit = arithzk->pub_label(1);
		mulcGate(&res, &res, PRIME-1);
		addGate(selected_bit, selected_bit, &res); // selected_bit = 1 - res
	}

	void multiplexHelper(__uint128_t* const out,
			std::vector<wtk::bolt::LocalWireRange<__uint128_t, uint64_t>*>* const dummies,
			std::vector<__uint128_t> const* const selector_bits,
			wtk::index_t const dummy_place) final {
		size_t length = selector_bits->size();
		vector<__uint128_t> selector_vector(*selector_bits);
		selector_vector.push_back(arithzk->pub_label(PRIME-1));// 0 1 0 ... 0 -1
		vector<__uint128_t> dummy_vector; // XX XX XX ... XX res
		for(size_t i = 0; i < length; ++i)
			dummy_vector.push_back(*((*dummies)[i]->deref(dummy_place)));
		uint64_t clear_out = 0, check_val = 0;
		for(size_t i = 0; i < selector_bits->size(); ++i) {
			uint64_t clear_selector_bit_i = HIGH64(selector_vector[i]);
			if(clear_selector_bit_i > 1) error("non-binary selector bit!!");
			if(clear_selector_bit_i == 1) {
				clear_out = HIGH64(dummy_vector[i]);
			}
			check_val +=clear_selector_bit_i;
		}
		assert(check_val == 1);//check_val is only for debug.
		witness(out, clear_out);
		dummy_vector.push_back(*out);
		//compute inner
		polyzk->zkp_inner_prdt(selector_vector.data(), dummy_vector.data(), 0, selector_vector.size());
	}


private:
	const static uint64_t PRIME = (1ULL<<61)-1;
	const static uint64_t PRIME_SIZE = 61;
	const static constexpr bool PRIME_MINUS_1_BITS[] = 
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
};
