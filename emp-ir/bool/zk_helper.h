#ifndef EMP_ZK_HELPER__
#define EMP_ZK_HELPER__
#include <wtk/irregular/Parser.h>
vector<uint8_t> instance;
vector<uint8_t> witness;
size_t wit_cnt = 0, ins_cnt = 0;
void load_input (vector<uint8_t> & vec, const char * file) {
	std::string filestr(file);
	wtk::irregular::Parser<uint8_t> p(filestr);
	p.parseHeader();
	p.parseResource();
	uint8_t data = 0;
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

/*
void load_input (vector<unsigned char> & vec, const char * file) {
	FILE * f = fopen(file, "r");
	char * str = new char[100000];

	while(fscanf(f, "%s", str)==1) {
		if (str[0] == '@') 
			break;
	}

	while(true) {
		char c = getc_unlocked(f);
		if(c == EOF) break;
		if(c == '0') {
			vec.push_back(0);
		} else if (c == '1') { 
			vec.push_back(1);
		} 
	}

	delete[] str;
	fclose(f);	
}*/

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
