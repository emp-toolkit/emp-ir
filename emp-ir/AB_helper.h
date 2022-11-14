#ifndef EMP_ABEXEC_HELP_H__
#define EMP_ABEXEC_HELP_H__
#include <vector>
#include <stack>
#include <iostream>
using std::stack;
using std::vector;

stack<Field> prefixCon;
vector<Field> witness_buffer;
vector<size_t> witness_branch_sizes;
size_t witness_counter = 0;


vector<Field> instance_buffer;
vector<size_t> instance_branch_sizes;
size_t instance_counter = 0;

Field get_next_witness() {
	if(witness_branch_sizes.size() > 0)
		witness_branch_sizes[witness_branch_sizes.size()-1]++;

	if (witness_counter == witness_buffer.size()) {
		witness_buffer.push_back(EMP_next_input(WITNESS));
		witness_counter++;
		return witness_buffer.back();
	} else return witness_buffer[witness_counter++];
}

Field get_next_instance() {
	if(instance_branch_sizes.size() > 0)
		instance_branch_sizes[instance_branch_sizes.size()-1]++;

	if (instance_counter == instance_buffer.size()) {
		instance_buffer.push_back(EMP_next_input(INSTANCE));
		instance_counter++;
		return instance_buffer.back();
	} else return instance_buffer[instance_counter++];
}

void start_branch() {
	witness_branch_sizes.push_back(0);
	instance_branch_sizes.push_back(0);
}

void end_branch(size_t witness_size, size_t instance_size) {
	witness_counter -= witness_branch_sizes.back();
	instance_counter -= instance_branch_sizes.back();
	witness_counter += witness_size;
	instance_counter += instance_size;
	witness_branch_sizes.pop_back();
	instance_branch_sizes.pop_back();
}
void next_case() {
	witness_counter -= witness_branch_sizes.back();
	instance_counter -= instance_branch_sizes.back();
	witness_branch_sizes.back() = 0;
	instance_branch_sizes.back() = 0;
}
void emp_main();
#endif
