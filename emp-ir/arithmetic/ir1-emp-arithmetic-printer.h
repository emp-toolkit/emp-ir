#ifndef IR1_EMP_Arithmetic_PRINTER_H__
#define IR1_EMP_Arithmetic_PRINTER_H__
#include "emp-ir/ir1-emp-printer.h"

class EMPIR1ArithmeticPrinter : public EMPIR1Printer<uint64_t> { public:
	EMPIR1ArithmeticPrinter(FILE* out) : EMPIR1Printer<uint64_t>(out, false) { }
	void printBinaryGate(BinaryGate* gate);
	void printBinaryConstGate(BinaryConstGate<uint64_t>* gate);
	void printUnaryGate(UnaryGate* gate);
};

void EMPIR1ArithmeticPrinter::printBinaryGate(BinaryGate* gate) {
	if(gate->calculation() == BinaryGate::MUL) {
		fprintf(this->outFile, "%s1, %" PRIu64 ", %" PRIu64 ", %" PRIu64 ", //mul\n",
			indent.get(), gate->outputWire(), gate->leftWire(), gate->rightWire());
	} else if(gate->calculation() == BinaryGate::ADD) {
		fprintf(this->outFile, "%s2, %" PRIu64 ", %" PRIu64 ", %" PRIu64 ", //add\n",
			indent.get(), gate->outputWire(), gate->leftWire(), gate->rightWire());
	} else {
		fprintf(stderr, "unexpected!\n");
		exit(1);
	}
}

void EMPIR1ArithmeticPrinter::printBinaryConstGate(BinaryConstGate<uint64_t>* gate) {
	if(gate->calculation() == BinaryConstGate<uint64_t>::MULC) {
		fprintf(this->outFile, "%s3, %" PRIu64 ", %" PRIu64 ", %" PRIu64 ",//mulc\n",
			indent.get(), gate->outputWire(), gate->leftWire(), gate->rightValue());
	} else if(gate->calculation() == BinaryConstGate<uint64_t>::ADDC) {
		fprintf(this->outFile, "%s6, %" PRIu64 ", %" PRIu64 ", %" PRIu64 ",//addc\n",
			indent.get(), gate->outputWire(), gate->leftWire(), gate->rightValue());
	} else {
		fprintf(stderr, "unexpected!\n");
		exit(1);
	}
}

void EMPIR1ArithmeticPrinter::printUnaryGate(UnaryGate* gate) {
	if (gate->calculation() == UnaryGate::COPY) {
		fprintf(this->outFile, "%s4, %" PRIu64 ", %" PRIu64 ", 0,//copy\n",
			indent.get(), gate->outputWire(), gate->inputWire());
	} else {
		fprintf(stderr, "unexpected! unary\n");
		exit(1);
	}
}
#endif 
