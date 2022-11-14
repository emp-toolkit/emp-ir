#ifndef IR1_EMP_Binary_PRINTER_H__
#define IR1_EMP_Binary_PRINTER_H__
#include "emp-ir/ir1-emp-printer.h"

class EMPIR1BinaryPrinter: public EMPIR1Printer<uint8_t> { public: 
	EMPIR1BinaryPrinter(FILE* out) : EMPIR1Printer(out, true) { }
	void printBinaryGate(BinaryGate* gate);
	void printBinaryConstGate(BinaryConstGate<uint8_t>* gate) {
		std::cerr<<"Boolean circuit does not have binary const gate!\n";
		exit(1);
	}
	void printUnaryGate(UnaryGate* gate);
};

void EMPIR1BinaryPrinter::printBinaryGate(BinaryGate* gate) {
	if(gate->calculation() == BinaryGate::AND) {
		fprintf(this->outFile, "%s1, %" PRIu64 ", %" PRIu64 ", %" PRIu64 ",\n",
			indent.get(), gate->outputWire(), gate->leftWire(), gate->rightWire());
	} else if(gate->calculation() == BinaryGate::XOR) {
		fprintf(this->outFile, "%s2, %" PRIu64 ", %" PRIu64 ", %" PRIu64 ",\n",
			indent.get(), gate->outputWire(), gate->leftWire(), gate->rightWire());
	} else {
		fprintf(stderr, "unexpected!\n");
		exit(1);
	}
}

void EMPIR1BinaryPrinter::printUnaryGate(UnaryGate* gate) {
	if(gate->calculation() == UnaryGate::NOT) {
		fprintf(this->outFile, "%s3, %" PRIu64 ", %" PRIu64 ", 0,\n",
			indent.get(), gate->outputWire(), gate->inputWire());
	}
	else if (gate->calculation() == UnaryGate::COPY) {
		fprintf(this->outFile, "%s4, %" PRIu64 ", %" PRIu64 ", 0,\n",
			indent.get(), gate->outputWire(), gate->inputWire());
	} else {
		fprintf(stderr, "unexpected!\n");
		exit(1);
	}
}
#endif 
