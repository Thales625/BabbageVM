#include "bitmap.hpp"

#include <iostream>

BitMap::BitMap(Memory<> *n_mem_ptr, size_t n_start_index, size_t n_width, size_t n_height) : mem_ptr(n_mem_ptr), start_index(n_start_index), width(n_width), height(n_height) {
	// check if have space
	if (n_start_index + n_width * n_height > n_mem_ptr->get_size())
		throw std::out_of_range("Not enough space");
}

void BitMap::show() {
	std::cout << "BitMap Display\n";

	for (size_t j=0; j<this->height; j++) {
		std::cout << "| ";
		for (size_t i=0; i<this->width; i++) {
			std::cout << this->mem_ptr->read(this->start_index + j*this->height + i) << "\t";
		}
		std::cout << " |\n";
	}
}