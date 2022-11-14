#ifndef IR1_EMP_PRINTER_H__
#define IR1_EMP_PRINTER_H__

#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <inttypes.h>
#include <set>
#include <map>
#include <unordered_map>
#include <iostream>
#include <wtk/IRTree.h>
#include <wtk/utils/NumUtils.h>
#include <wtk/utils/Indent.h>
using std::set;
using std::cerr;
using std::string;
using std::vector;
using namespace wtk;

template<typename Number_T>
class EMPIR1Printer { public:
	// used during relation/proof checks for finding function bodies.
	std::unordered_map<std::string, FunctionDeclare<Number_T>*> functionsMap;
	wtk::utils::Indent indent;
	FILE* outFile = nullptr;
	bool isbinary;
	vector<string> iterNames;
	string getIterNames();

	EMPIR1Printer(FILE* out, bool isbinary) : outFile(out), isbinary(isbinary) { }

	virtual void printBinaryGate(BinaryGate* gate) = 0;
	virtual void printBinaryConstGate(BinaryConstGate<Number_T>* gate) = 0;
	virtual void printUnaryGate(UnaryGate* gate) = 0;

	void printInput(Input* input);

	void printAssign(Assign<Number_T>* assign);

	void printAssertZero(Terminal* assertZero);

	void printDeleteSingle(Terminal* del);

	void printDeleteRange(WireRange* del);

	void printFunctionInvoke(FunctionInvoke* invoke);

	void printDirectiveList(DirectiveList<Number_T>* list);

	void printInitializeArray(WireList * list, std::string dst, size_t output_size = 0);

	void printInputAssignment(WireList * list, std::string dst, std::string src, size_t output_size);

	void printOutputAssignment(WireList * list, std::string dst, std::string src, const char * condition = nullptr);

	void printInputAssignment(IterExprWireList * list, std::string dst, std::string src, const char * counter_name = nullptr, size_t size = 0);

	void printOutputAssignment(IterExprWireList * list, std::string dst, std::string src);

	void printAnonFunction(AnonFunction<Number_T>* anon);

	void printIterExpr(IterExpr* expr);

	void printForLoop(ForLoop<Number_T>* loop);

	uint64_t getScopeSize(DirectiveList<Number_T>* list, WireList * list1 = nullptr, WireList * list2 = nullptr);

	index_t countWires(WireList * list);

	bool containIterName (const char * name, IterExpr * expr);	

	bool containIterName (const char * name, IterExprWireList * list);	

	void printCountIterExprWireList(IterExprWireList * list, const char * counter_name);

	void countWires(std::set<index_t>& wires, WireList * list);

	void printSwitchStatement(SwitchStatement<Number_T>* switch_stmt);

	void printTree(IRTree<Number_T>* tree, int mode);
};

#include "emp-ir/ir1-emp-printer.hpp"
#endif 
