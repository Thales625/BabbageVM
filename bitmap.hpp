#pragma once

#include "memory.hpp"
#include "utils.hpp"

struct BitMap {
public:
	BitMap(Memory *n_mem_ptr, size_t start_index, size_t end_index) : mem_ptr(n_mem_ptr), display_alloc({start_index, end_index}) {}
private:
	Memory *mem_ptr;
	MemAllocation display_alloc;
};