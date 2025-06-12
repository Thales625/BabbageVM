#pragma once

#include "vm_types.hpp"
#include "register.hpp"

#include <vector>
#include <stdexcept>

class Memory {
public:
	Memory(std::size_t size);

	size_t get_size() const;

	word_t read(address_t address) const;
	void write(address_t address, word_t value);
	void clear();

	void load(const std::vector<word_t>& data_vector, address_t start_address);
	// void append(const std::vector<word_t>& data_vector); // TODO
private:
	std::vector<word_t> data;
	size_t size;
};