#ifndef EMP_ZK_HELPER__
#define EMP_ZK_HELPER__
#include <wtk/irregular/Parser.h>
vector<uint64_t> instance;
vector<uint64_t> witness;
size_t wit_cnt = 0, ins_cnt = 0;
void load_input (vector<uint64_t> & vec, const char * file) {
	std::string file_str(file);
	wtk::irregular::Parser<uint64_t> p(file_str);
	p.parseHeader();
	p.parseResource();
	uint64_t data = 0;
	if(p.resource == wtk::Resource::instance) {
		auto stream = p.arithmetic()->instance();
		while(stream->next(&data)!=wtk::StreamStatus::end) {
			vec.push_back(data);
		}
	}
	else if(p.resource == wtk::Resource::shortWitness) {
		auto stream = p.arithmetic()->shortWitness();
		while(stream->next(&data)!=wtk::StreamStatus::end) {
			vec.push_back(data);
		}
	} else {
		cerr<<"file type wrong!\n";
	}
}

void load_instance (string f) {
	load_input(instance, f.c_str());
	ins_cnt = 0;
}
void load_witness (string f) {
	load_input(witness, f.c_str());
	wit_cnt = 0;
}

Field EMP_next_input(int source) {
	if(source == WITNESS) {
		if(witness.size() != 0) {
			return Field(witness[wit_cnt++], ALICE);
		}
		else {
			return Field(false, ALICE);
		}
	} else {
		return Field(instance[ins_cnt++], PUBLIC);
	}
}
#endif// EMP_ZK_HELPER__
