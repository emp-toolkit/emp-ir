#ifndef FLEX_VECTOR_H__
#define FLEX_VECTOR_H__
#include<vector>
using std::vector;
template<typename T>
class FlexVector { public:
	vector<T> vec;
	FlexVector(int init_size = 100) {
		vec.resize(init_size);
	}
	void put(uint64_t idx, const T & v) {
		if(idx < vec.size())
			vec[idx] = v;
		else {
			vec.resize(2*idx);
			vec[idx] = v;
		}
	}
	
	const T& operator[](uint64_t idx) const {
		return vec[idx];
	}
};

template<typename T>
class StubVector { public:
	vector<T> vec;
	StubVector(int init_size) {
		vec.resize(init_size);
	}
	void resize(size_t new_size) {
		if(new_size > vec.size())vec.resize(new_size);
	}
	void put(uint64_t idx, const T & v) {
		vec[idx] = v;
	}
	
	const T& operator[](uint64_t idx) const {
		return vec[idx];
	}
	T* data() {
		return vec.data();
	}
};

#endif // FLEX_VECTOR_H__