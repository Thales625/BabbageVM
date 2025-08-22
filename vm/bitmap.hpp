#pragma once

#include "memory.hpp"
#include "vm_utils.hpp"

struct BitMap {
public:
	BitMap(Memory<> *n_mem_ptr, size_t n_start_index, size_t n_width, size_t n_height);

	void show();
private:
	Memory<> *mem_ptr;
	size_t start_index;
	size_t width, height;
};