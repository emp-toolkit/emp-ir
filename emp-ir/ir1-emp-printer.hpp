template<typename Number_T>
bool EMPIR1Printer<Number_T>::containIterName (const char * name, IterExpr * expr) {
	if (expr->type() == IterExpr::LITERAL) {
		return false;
	} else if (expr->type() == IterExpr::ITERATOR) {
		return strcmp(name, expr->name())==0;
	} else {
		bool tmp1 = containIterName(name, expr->lhs());
		bool tmp2 = containIterName(name, expr->rhs());
		return tmp1 or tmp2;
	}
}	
template<typename Number_T>
bool EMPIR1Printer<Number_T>::containIterName (const char * name, IterExprWireList * list) {
   for(size_t i = 0; i < list->size(); i++) {
		if(list->type(i) == IterExprWireList::SINGLE) {
			bool tmp = containIterName(name, list->single(i));
			if (tmp) return true;
		} else if (list->type(i) == IterExprWireList::RANGE) {
			bool tmp1 = containIterName(name, list->range(i)->first());
			bool tmp2 = containIterName(name, list->range(i)->last());
			if (tmp1 or tmp2) return true;
		}
	}
	return false;
}	
template<typename Number_T>
string EMPIR1Printer<Number_T>::getIterNames() {
	return "&";//We are catching all variables and rely on shaowing to resovle conflict.
//If there is a case that it cannot handle, we can mannual pass loop iter names.
/*	if(iterNames.size() == 0) return "";
	else {
		string res = iterNames[0];
		for(size_t i = 1; i < iterNames.size(); ++i)
			res = res + string(",") + iterNames[i];
		return res;
	}*/

}

template<typename Number_T>
void EMPIR1Printer<Number_T>::countWires(set<index_t>& wires, WireList * list) {
	for(size_t i = 0; i < list->size(); i++) {
		if(list->type(i) == WireList::SINGLE)
			wires.insert(list->single(i));
		else if(list->type(i) == WireList::RANGE) {
			for(index_t j = list->range(i)->first(); j <=list->range(i)->last(); ++j)
				wires.insert(j);
		}
	}
}

template<typename Number_T>
uint64_t EMPIR1Printer<Number_T>::getScopeSize(DirectiveList<Number_T>* list, WireList * list1, WireList * list2) {
	set<index_t> wires;
	if(list1 != nullptr) countWires(wires, list1);
	if(list2 != nullptr) countWires(wires, list2);
	for(size_t i = 0; i < list->size(); i++) {
		if (list->type(i) == DirectiveList<Number_T>::BINARY_GATE) {
			wires.insert(list->binaryGate(i)->outputWire());
		}
		else if (list->type(i) == DirectiveList<Number_T>::BINARY_CONST_GATE) {
			wires.insert(list->binaryConstGate(i)->outputWire());
		} else if (list->type(i) == DirectiveList<Number_T>::UNARY_GATE) {
			wires.insert(list->unaryGate(i)->outputWire());
		} else if (list->type(i) == DirectiveList<Number_T>::INPUT) {
			wires.insert(list->input(i)->outputWire());
		} else if (list->type(i) == DirectiveList<Number_T>::ASSIGN) {
			wires.insert(list->assign(i)->outputWire());
		} else if (list->type(i) == DirectiveList<Number_T>::ASSERT_ZERO) {
			wires.insert(list->assertZero(i)->wire());
		} else if (list->type(i) == DirectiveList<Number_T>::FUNCTION_INVOKE) {
			countWires(wires, list->functionInvoke(i)->outputList());
		} else if (list->type(i) == DirectiveList<Number_T>::ANON_FUNCTION) {
			countWires(wires, list->anonFunction(i)->outputList());
		} else if (list->type(i) == DirectiveList<Number_T>::FOR_LOOP) {
			countWires(wires, list->forLoop(i)->outputList());
		} else if (list->type(i) == DirectiveList<Number_T>::SWITCH_STATEMENT) {
			countWires(wires, list->switchStatement(i)->outputList());
		}
		//TODO: We don't do anything with them but what if there is a function that just do delete..
		//case DirectiveList<Number_T>::DELETE_SINGLE:
		//case DirectiveList<Number_T>::DELETE_RANGE:
	}
	//	*capacity = (uint64_t)wires.size();
	uint64_t range;
	if(!wires.empty())
		range = (uint64_t)*wires.rbegin();
	else
		range = 0;
	return range+1;
}

template<typename Number_T>
void EMPIR1Printer<Number_T>::printCountIterExprWireList(IterExprWireList * list, const char * counter_name) {
	fprintf(this->outFile, "%sint %s = 0", indent.get(), counter_name);
   for(size_t i = 0; i < list->size(); i++) {
		if(list->type(i) == IterExprWireList::SINGLE) {
			fprintf(this->outFile, " + 1");
		} else if (list->type(i) == IterExprWireList::RANGE) {
			fprintf(this->outFile, " + (");
			this->printIterExpr(list->range(i)->last());
			fprintf(this->outFile, " - ");
			this->printIterExpr(list->range(i)->first());
			fprintf(this->outFile, " + 1)");
		}
	}
	fprintf(this->outFile, ";\n");
}


template<typename Number_T>
void EMPIR1Printer<Number_T>::printIterExpr(IterExpr* expr) {
	if (expr->type() == IterExpr::LITERAL) {
		fprintf(this->outFile, "%" PRIu64, expr->literal());
	} else if (expr->type() == IterExpr::ITERATOR) {
		fputs(expr->name(), this->outFile);
	} else {
		fputs("(", this->outFile);
		this->printIterExpr(expr->lhs());

		if (expr->type() == IterExpr::ADD) {
			fputs(" + ", this->outFile);
		} else if (expr->type() == IterExpr::SUB) {
			fputs(" - ", this->outFile);
		} else if (expr->type() == IterExpr::MUL) {
			fputs(" * ", this->outFile);
		}

		this->printIterExpr(expr->rhs());
		fputs(")", this->outFile);
	}
}

template<typename Number_T>
index_t EMPIR1Printer<Number_T>::countWires(WireList * list) {
	index_t res = 0;
	for(size_t i = 0; i < list->size(); i++) {
		if(list->type(i) == WireList::SINGLE)
			++res;
		else if(list->type(i) == WireList::RANGE)
			res += (1 + list->range(i)->last() - list->range(i)->first());
	}
	return res;
}

template<typename Number_T>
void EMPIR1Printer<Number_T>::printInput(Input* input) {
	if(input->stream() == Input::INSTANCE) {
		fprintf(this->outFile, "%swires.put(%" PRIu64 ", get_next_instance());\n",
				indent.get(), input->outputWire());
	} else if (input->stream() == Input::SHORT_WITNESS ) {
		fprintf(this->outFile, "%swires.put(%" PRIu64 ", get_next_witness());\n",
				indent.get(), input->outputWire());
	} else {
		fprintf(stderr, "unexpected!\n");
		exit(1);
	}
}

template<typename Number_T>
void EMPIR1Printer<Number_T>::printAssertZero(Terminal* assertZero) {
	fprintf(this->outFile, "%sconditional_assert_zero(wires[%" PRIu64 "]);\n", indent.get(), assertZero->wire());
}

template<typename Number_T>
void EMPIR1Printer<Number_T>::printDeleteSingle(Terminal* del) {
	//	fprintf(this->outFile, "%s//@delete($%" PRIu64 ");\n", indent.get(), del->wire());
}

template<typename Number_T>
void EMPIR1Printer<Number_T>::printDeleteRange(WireRange* del) {
	//	fprintf(this->outFile, "%s//@delete($%" PRIu64 ",$%" PRIu64 ");\n", indent.get(), del->first(), del->last());
}

template<typename Number_T>
void EMPIR1Printer<Number_T>::printInitializeArray(WireList * list, std::string dst, size_t output_size) {
	if(list->size() == 0)
		return;
	size_t total_size = output_size;
	for(size_t i = 0; i < list->size(); ++i) {
		if (list->type(i) == WireList::SINGLE) {
			fprintf(this->outFile, "%s%s.put(%" PRIu64 ", Field(%s, PUBLIC));\n", indent.get(),
					dst.c_str(), list->single(i), isbinary?"false":"0");
			++total_size;
		} else if (list->type(i) == WireList::RANGE) {
			fprintf(this->outFile, "%sfor(uint64_t __j = %" PRIu64 ", __i = %zu; __j <= %" PRIu64 "; ++__j, ++__i) ",
					indent.get(), list->range(i)->first(), total_size, list->range(i)->last());
			fprintf(this->outFile, "%s.put(__i, Field(%s, PUBLIC));\n", dst.c_str(), isbinary?"false":"0");
			total_size += (list->range(i)->last() - list->range(i)->first() + 1);
		}
	}
}

template<typename Number_T>
void EMPIR1Printer<Number_T>::printInputAssignment(WireList * list, std::string dst, std::string src, size_t output_size) {
	// for input assignment, dst should start from output_size and increase continously
	if(list->size() == 0)
		return;
	size_t total_size = output_size;
	for(size_t i = 0; i < list->size(); ++i) {
		if (list->type(i) == WireList::SINGLE) {
			fprintf(this->outFile, "%s%s.put(%zu, %s[%" PRIu64 "]);\n", indent.get(),
					dst.c_str(), total_size, src.c_str(), list->single(i));
			++total_size;
		} else if (list->type(i) == WireList::RANGE) {
			fprintf(this->outFile, "%sfor(uint64_t __j = %" PRIu64 ", __i = %zu; __j <= %" PRIu64 "; ++__j, ++__i) ",
					indent.get(), list->range(i)->first(), total_size, list->range(i)->last());
			fprintf(this->outFile, "%s.put(__i, %s[__j]);\n", dst.c_str(), src.c_str());
			total_size += (list->range(i)->last() - list->range(i)->first() + 1);
		}
	}
}


template<typename Number_T>
void EMPIR1Printer<Number_T>::printOutputAssignment(WireList * list, std::string dst, std::string src, const char * condition) {
	if(list->size() == 0)
		return;
	size_t total_size = 0;
	for(size_t i = 0; i < list->size(); ++i) {
		fprintf(this->outFile, "%s", indent.get());
		if(list->type(i) == WireList::SINGLE) {
			if(condition == nullptr)
				fprintf(this->outFile, "%s.put(%" PRIu64 ",  %s[%zu]);\n",
						dst.c_str(), list->single(i), src.c_str(), total_size);
			else
				fprintf(this->outFile, "%s.put(%" PRIu64 ", %s[%" PRIu64 "].select(%s, %s[%zu]));\n",
						dst.c_str(), list->single(i), dst.c_str(), list->single(i), 
						condition, src.c_str(), total_size);

			++ total_size;
		} else if( list->type(i) == WireList::RANGE) {
			fprintf(this->outFile, "for(uint64_t __j = %" PRIu64 ", __i = %zu; __j <= %" PRIu64 "; ++__j, ++__i) ",
					list->range(i)->first(), total_size, list->range(i)->last());
			if(condition == nullptr)
				fprintf(this->outFile, "%s.put(__j,  %s[__i]);\n", dst.c_str(), src.c_str());
			else 
				fprintf(this->outFile, "%s.put(__j, %s[__j].select(%s, %s[__i]));\n", 
						dst.c_str(), dst.c_str(), condition, src.c_str());
			total_size += (list->range(i)->last() - list->range(i)->first() + 1);
		}
	}
}


template<typename Number_T>
void EMPIR1Printer<Number_T>::printInputAssignment(IterExprWireList * list, std::string dst, std::string src, const char * counter_name, size_t output_size) {
	if(list->size() == 0) return;
	fprintf(this->outFile, "%s", indent.get());
	if(counter_name == nullptr)
		fprintf(this->outFile, "size_t input_cnt = %zu; //Function output size\n", output_size);
	else 
		fprintf(this->outFile, "size_t input_cnt = %s; //Function output size\n", counter_name);

	for(size_t i = 0; i < list->size(); ++i) {
		fprintf(this->outFile, "%s", indent.get());
		if (list->type(i) == IterExprWireList::SINGLE) {
			//Should print dst[input_cnt++] = src[list_index];
			fprintf(this->outFile, "%s.put(input_cnt++, %s[", dst.c_str(), src.c_str());
			this->printIterExpr(list->single(i));
			fprintf(this->outFile, "]);\n");
		} else if (list->type(i) == IterExprWireList::RANGE) {
			// for(int i = ??; i <= ??; ++i)
			fprintf(this->outFile, "for(uint64_t __j = ");
			this->printIterExpr(list->range(i)->first());
			fprintf(this->outFile, "; __j <= ");
			this->printIterExpr(list->range(i)->last());
			fprintf(this->outFile, "; ++__j) ");
			fprintf(this->outFile, "%s.put(input_cnt++, %s[__j]);\n", dst.c_str(), src.c_str());
		}
	}
}

template<typename Number_T>
void EMPIR1Printer<Number_T>::printOutputAssignment(IterExprWireList * list, std::string dst, std::string src) {
	if(list->size() == 0)
		return;
	fprintf(this->outFile, "%ssize_t output_cnt = 0;\n", indent.get());
	for(size_t i = 0; i < list->size(); ++i) {
		fprintf(this->outFile, "%s", indent.get());
		if (list->type(i) == IterExprWireList::SINGLE) {
			fprintf(this->outFile, "%s.put(", dst.c_str());
			this->printIterExpr(list->single(i));
			fprintf(this->outFile, ", %s[output_cnt++]);\n", src.c_str());
		} else if (list->type(i) == IterExprWireList::RANGE) {
			fprintf(this->outFile, "for(uint64_t __j = ");
			this->printIterExpr(list->range(i)->first());
			fprintf(this->outFile, "; __j <= ");
			this->printIterExpr(list->range(i)->last());
			fprintf(this->outFile, "; ++__j) ");
			fprintf(this->outFile, "%s.put(__j, %s[output_cnt++]);\n", dst.c_str(), src.c_str());
		}
	}
}

template<typename Number_T>
void EMPIR1Printer<Number_T>::printFunctionInvoke(FunctionInvoke* invoke) {
	FunctionDeclare<Number_T>* func = this->functionsMap[invoke->name()];
	fprintf(this->outFile, "%s{ //Begin func call %s\n", indent.get(), invoke->name());

	indent.inc();
	fprintf(this->outFile, "%sstatic StubArray func_wires(%" PRIu64 ");\n", indent.get(), getScopeSize(func->body(), invoke->inputList(), invoke->outputList()));
	printInputAssignment(invoke->inputList(), "func_wires", "wires", func->outputCount());
	fprintf(this->outFile, "%s%s(func_wires);\n", indent.get(), invoke->name());
	printOutputAssignment(invoke->outputList(), "wires", "func_wires");
	indent.dec();

	fprintf(this->outFile, "%s} //End of function call to %s\n", indent.get(), invoke->name());
}

template<typename Number_T>
void EMPIR1Printer<Number_T>::printAnonFunction(AnonFunction<Number_T>* anon) { 
	fprintf(this->outFile, "%s{ // Begin of Anon function\n", indent.get());
	index_t inputCount = countWires(anon->inputList());
	index_t outputCount = countWires(anon->outputList());

	indent.inc();
	fprintf(this->outFile, "%sstatic StubArray func_wires(%" PRIu64 ");\n", indent.get(), getScopeSize(anon->body(), anon->inputList(), anon->outputList()));
	printInputAssignment(anon->inputList(), "func_wires", "wires", outputCount);
	fprintf(this->outFile, "%s[] (StubArray & wires) { //(in, out, ins, wit) = (%" PRIu64 ", %" PRIu64 ", %" PRIu64 ", %" PRIu64 ")\n",
			indent.get(), inputCount, outputCount, anon->instanceCount(), anon->shortWitnessCount());
	this->printDirectiveList(anon->body());
	fprintf(this->outFile, "%s} (func_wires); \n",indent.get());
	printOutputAssignment(anon->outputList(), "wires", "func_wires");
	indent.dec();

	fprintf(this->outFile, "%s} // End of Anon function\n",indent.get());
}


template<typename Number_T>
void EMPIR1Printer<Number_T>::printForLoop( ForLoop<Number_T>* loop) {
	fprintf(this->outFile, "%sfor(uint64_t %s = %" PRIu64 "; %s <= %" PRIu64 "; ++%s) { //Begin of Loop\n", 
			indent.get(), loop->iterName(), loop->first(), loop->iterName(), loop->last(), loop->iterName());
	iterNames.push_back(string(loop->iterName()));

	indent.inc();
	if(loop->bodyType() == ForLoop<Number_T>::INVOKE) {
		IterExprFunctionInvoke* invoke = loop->invokeBody();
		FunctionDeclare<Number_T>* func = this->functionsMap[invoke->name()];
		fprintf(this->outFile, "%sstatic StubArray func_wires(%" PRIu64 ");\n",indent.get(), getScopeSize(func->body())+ func->inputCount() + func->outputCount());
		//This can be done in a more fine grained way, only the range with IterName needs to be reassigned.
		if(!containIterName(loop->iterName(), invoke->inputList())) {
			fprintf(this->outFile, "%sif (%s == %" PRIu64 ") {\n", indent.get(), loop->iterName(), loop->first());
			indent.inc();
		}
		printInputAssignment(invoke->inputList(), "func_wires", "wires", nullptr, func->outputCount());	
		if(!containIterName(loop->iterName(), invoke->inputList())) {
			indent.dec();
			fprintf(this->outFile, "%s}\n", indent.get());
		}
		fprintf(this->outFile, "%s%s(func_wires);\n", indent.get(), invoke->name());
		printOutputAssignment(invoke->outputList(), "wires", "func_wires");
	} else if (loop->bodyType() == ForLoop<Number_T>::ANONYMOUS) {
		IterExprAnonFunction<Number_T>* anon = loop->anonymousBody();
		printCountIterExprWireList(anon->outputList(), "output_size");
		printCountIterExprWireList(anon->inputList(), "input_size");
		fprintf(this->outFile, "%sstatic FlexArray func_wires(0); func_wires.resize(input_size + output_size + %" PRIu64 ");\n",indent.get(), getScopeSize(anon->body()));
		if(!containIterName(loop->iterName(), anon->inputList())) {
			fprintf(this->outFile, "%sif (%s == %" PRIu64 ") {\n", indent.get(), loop->iterName(), loop->first());
			indent.inc();
		}
		printInputAssignment(anon->inputList(), "func_wires", "wires", "output_size");
		if(!containIterName(loop->iterName(), anon->inputList())) {
			indent.dec();
			fprintf(this->outFile, "%s}\n", indent.get());
		}
		fprintf(this->outFile, "%s[%s] (FlexArray & wires) { // lambda function\n", indent.get(), getIterNames().c_str());
		fprintf(this->outFile, "%s//(ins, wit) = (%" PRIu64 ", %" PRIu64 ")\n",
				indent.get(), anon->instanceCount(), anon->shortWitnessCount());
		this->printDirectiveList(anon->body());
		fprintf(this->outFile, "%s} (func_wires); \n", indent.get());
		printOutputAssignment(anon->outputList(), "wires", "func_wires");
	}
	iterNames.pop_back();
	indent.dec();

	fprintf(this->outFile, "%s} //End of Loop\n", indent.get());
}

template<typename Number_T>
void EMPIR1Printer<Number_T>::printAssign(Assign<Number_T>* assign) {
	fprintf(this->outFile, "%s5, %" PRIu64 ", %s, 0,\n",
			indent.get(), assign->outputWire(), wtk::utils::short_str(assign->constValue()).c_str());
}

template<typename Number_T>
void EMPIR1Printer<Number_T>::printTree(IRTree<Number_T>* tree, int mode) {
	string path = "#include \"emp-ir/";
	path = isbinary ? path + "bool/" : path + "arithmetic/";

	if(mode == 1) path += "print_helper.h\"\n";
	else if (mode == 2) path += "prover_helper.h\"\nusing namespace emp;\n";
	else if (mode == 3) path += "verifier_helper.h\"\nusing namespace emp;\n";

	fprintf(this->outFile,
			"%susing namespace std;\n"
			"typedef StubVector<Field> FlexArray;\n"
			"typedef StubVector<Field> StubArray;\n",
			path.c_str());

	for(size_t i = 0; i < tree->size(); i++) {
		FunctionDeclare<Number_T>* func = tree->functionDeclare(i);
		this->functionsMap[func->name()] = func;
		fprintf(this->outFile, "void %s(StubArray& wires) {", func->name());
		fprintf(this->outFile, " //(in, out, ins, wit) = (%" PRIu64 ", %"
				PRIu64 ", %" PRIu64 ", %" PRIu64 ")\n", func->inputCount(),
				func->outputCount(), func->instanceCount(), func->shortWitnessCount());
		this->printDirectiveList(func->body());
		fprintf(this->outFile, "%s}\n", indent.get());
	}

	fprintf(this->outFile,
			"void emp_main() {\n"
			"  static StubArray wires(%" PRIu64 ");\n"
			"  prefixCon.push(Field(%s, PUBLIC));\n",
			getScopeSize(tree->body()), isbinary?"true":"1");
	this->printDirectiveList(tree->body());
	fprintf(this->outFile, "}\n");
}

template<typename Number_T>
void EMPIR1Printer<Number_T>::printSwitchStatement(SwitchStatement<Number_T>* switch_stmt) {
	index_t outputCount = countWires(switch_stmt->outputList());
	fprintf(this->outFile, "%s{ // Begin switch\n", indent.get());

	indent.inc();
	fprintf(this->outFile, "%sstart_branch();\n", indent.get());
	fprintf(this->outFile, "%sstatic StubArray local_con(%zu);\n", indent.get(), switch_stmt->size());
	for(size_t i = 0; i < switch_stmt->size(); i++) {
		CaseBlock<Number_T>* case_blk = switch_stmt->caseBlock(i);
		fprintf(this->outFile, "%slocal_con.put(%zu, wires[%" PRIu64 "] == Field(%s, PUBLIC));\n",
				indent.get(), i, switch_stmt->condition(), wtk::utils::short_str(case_blk->match()).c_str());
	}

//	printInitializeArray(switch_stmt->outputList(), "wires");
	size_t max_wit = 0, max_ins = 0;
	for(size_t i = 0; i < switch_stmt->size(); i++) {
		CaseBlock<Number_T>* case_blk = switch_stmt->caseBlock(i);
		fprintf(this->outFile, "%s{ next_case(); // Case < %zu >\n", indent.get(), i);

		indent.inc();
		fprintf(this->outFile, "%sprefixCon.push(prefixCon.top() %s local_con[%zu]);\n", indent.get(), isbinary?"&":"*", i);
		if(case_blk->bodyType() == CaseBlock<Number_T>::INVOKE) {
			CaseFunctionInvoke* invoke = case_blk->invokeBody();
			FunctionDeclare<Number_T>* func = this->functionsMap[invoke->name()];
			max_wit = std::max(max_wit, (size_t)func->shortWitnessCount());
			max_ins = std::max(max_ins, (size_t)func->instanceCount());

			fprintf(this->outFile, "%sstatic StubArray func_wires(%" PRIu64 ");\n",indent.get(), 
				getScopeSize(func->body(), invoke->inputList(), switch_stmt->outputList()));
			printInputAssignment(invoke->inputList(), "func_wires", "wires", func->outputCount());
			fprintf(this->outFile, "%s%s(func_wires);\n", indent.get(), invoke->name());
		} else if (case_blk->bodyType() == CaseBlock<Number_T>::ANONYMOUS) {
			CaseAnonFunction<Number_T>* anon = case_blk->anonymousBody();
			max_wit = std::max(max_wit, (size_t)anon->shortWitnessCount());
			max_ins = std::max(max_ins, (size_t)anon->instanceCount());

			fprintf(this->outFile, "%sstatic StubArray func_wires(%" PRIu64 ");\n",indent.get(), 
				getScopeSize(case_blk->anonymousBody()->body(), anon->inputList(), switch_stmt->outputList()));
			index_t inputCount = countWires(anon->inputList());
			printInputAssignment(anon->inputList(), "func_wires", "wires", outputCount);
			fprintf(this->outFile, "%s[] (StubArray & wires) { //(in,out,ins,wit)="
					"(%" PRIu64 ",%" PRIu64 ",%" PRIu64 ",%" PRIu64 ")\n",
					indent.get(), inputCount, outputCount, anon->instanceCount(), anon->shortWitnessCount());
			this->printDirectiveList(anon->body());
			fprintf(this->outFile, "%s} (func_wires); \n",indent.get());
		}
		if (i == 0)
			printOutputAssignment(switch_stmt->outputList(), "wires", "func_wires");
		else
			printOutputAssignment(switch_stmt->outputList(), "wires", "func_wires", "prefixCon.top()");
		fprintf(this->outFile, "%sprefixCon.pop();\n", indent.get());
		indent.dec();
		fprintf(this->outFile, "%s}\n", indent.get());
	}
	fprintf(this->outFile, "%send_branch(%zu, %zu);\n", indent.get(), max_wit, max_ins);
	indent.dec();
	fprintf(this->outFile, "%s}\n", indent.get());

}

template<typename Number_T>
void EMPIR1Printer<Number_T>::printDirectiveList(DirectiveList<Number_T>* list) {
	indent.inc();
	uint64_t state = 0; // number of gates
	for(size_t i = 0; i < list->size(); i++) { 
		if(list->type(i) == DirectiveList<Number_T>::BINARY_GATE) {
			if(state == 0) fprintf(this->outFile, "%s{ const static uint64_t gate[] = {\n",indent.get());
			this->printBinaryGate(list->binaryGate(i));
			state++;
		} if(list->type(i) == DirectiveList<Number_T>::BINARY_CONST_GATE) {
			if(state == 0) fprintf(this->outFile, "%s{ const static uint64_t gate[] = {\n",indent.get());
			this->printBinaryConstGate(list->binaryConstGate(i));
			state++;
		} else if (list->type(i) == DirectiveList<Number_T>::UNARY_GATE) {
			if(state == 0) fprintf(this->outFile, "%s{ const static uint64_t gate[] = {\n",indent.get());
			this->printUnaryGate(list->unaryGate(i));
			state++;
		} else if (list->type(i) == DirectiveList<Number_T>::INPUT) {
			if(state > 0) {
				fprintf(this->outFile, "%s}; execute(wires,gate,%" PRIu64 "); }\n",indent.get(), state);
				state = 0;
			}
			this->printInput(list->input(i));
		} else if (list->type(i) == DirectiveList<Number_T>::ASSIGN) {
			if(state == 0) fprintf(this->outFile, "%s{ const static uint64_t gate[] = {\n",indent.get());
			state++;
			this->printAssign(list->assign(i));
		} else if (list->type(i) == DirectiveList<Number_T>::ASSERT_ZERO) {
			if(state > 0) {
				fprintf(this->outFile, "%s}; execute(wires,gate,%" PRIu64 "); }\n",indent.get(), state);
				state = 0;
			}
			this->printAssertZero(list->assertZero(i));
		} else if (list->type(i) == DirectiveList<Number_T>::DELETE_SINGLE) {
			this->printDeleteSingle(list->deleteSingle(i));
		} else if (list->type(i) == DirectiveList<Number_T>::DELETE_RANGE) {
			this->printDeleteRange(list->deleteRange(i));
		} else if (list->type(i) == DirectiveList<Number_T>::FUNCTION_INVOKE) {
			if(state > 0) {
				fprintf(this->outFile, "%s}; execute(wires,gate,%" PRIu64 "); }\n",indent.get(), state);
				state = 0;
			}
			this->printFunctionInvoke(list->functionInvoke(i));
		} else if (list->type(i) == DirectiveList<Number_T>::ANON_FUNCTION) {
			if(state > 0) {
				fprintf(this->outFile, "%s}; execute(wires,gate,%" PRIu64 "); }\n",indent.get(), state);
				state = 0;
			}
			this->printAnonFunction(list->anonFunction(i));
		} else if (list->type(i) == DirectiveList<Number_T>::FOR_LOOP) {
			if(state > 0) {
				fprintf(this->outFile, "%s}; execute(wires,gate,%" PRIu64 "); }\n",indent.get(), state);
				state = 0;
			}
			this->printForLoop(list->forLoop(i));
		} else if (list->type(i) == DirectiveList<Number_T>::SWITCH_STATEMENT) {
			if(state > 0) {
				fprintf(this->outFile, "%s}; execute(wires,gate,%" PRIu64 "); }\n",indent.get(), state);
				state = 0;
			}
			this->printSwitchStatement(list->switchStatement(i));
		}
	}
	if(state > 0) {
		fprintf(this->outFile, "%s}; execute(wires,gate,%" PRIu64 "); }\n",indent.get(), state);
		state = 0;
	}
	indent.dec();
}
