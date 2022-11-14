#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <string.h>
#include <wtk/utils/FileNameUtils.h>
#include <wtk/irregular/Parser.h>
#include "emp-ir/emp-ir.h"

using namespace wtk;

int main(int argc, char const* argv[]) {
	if (argc != 3) {
		std::cerr << argv[0] << " input.rel [printer|prover|verifier]\n";
		exit(1);
	}

	int mode = 0;
	if(strncmp(argv[2], "printer", 7) == 0)
		mode = 1;
	else if (strncmp(argv[2], "prover", 6) == 0)
		mode = 2;
	else if (strncmp(argv[2], "verifier", 8) == 0)
		mode = 3;
	else {
    std::cerr << "Unrecognized mode \n" << argv[2] << "\"\n";
		std::cerr << argv[0] << " input.rel [printer|prover|verifier]\n";
		exit(1);
	}

	std::string fname(argv[1]);
	wtk::irregular::Parser<uint64_t> p(fname);
  if(!p.parseHeader() || !p.parseResource())
  {
    exit(1);
  }

  if(p.resource != wtk::Resource::relation) {
    std::cerr << argv[1] << " is not a relation\n";
    std::cerr << argv[0] << " input.rel [printer|prover|verifier]\n";
    exit(1);
  }

	if(!p.parseParameters()) {
    exit(1);
  }
	
	if(p.gateSet.gateSet == wtk::GateSet::arithmetic) {
		if(p.characteristic != (1ULL<<61)-1) {
			std::cerr<<"Prime not supported!\n";
			exit(1);
		}

    wtk::IRTree<uint64_t>* tree = p.arithmetic()->parseTree();
    if(tree == nullptr)
    {
      exit(1);
    }

	  EMPIR1ArithmeticPrinter prt(stdout);
	  prt.printTree(tree, mode);
	} else {
    wtk::IRTree<uint8_t>* tree = p.boolean()->parseTree();
    if(tree == nullptr)
    {
      exit(1);
    }

		EMPIR1BinaryPrinter prt(stdout);
		prt.printTree(tree, mode);
	}
	return 0;
}
