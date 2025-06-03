#include "memory.hpp"

Memory::Memory(std::size_t n_size) : data(n_size, 0), size(n_size) {}

size_t Memory::get_size() const {
	return this->size;
}

word_t Memory::read(address_t address) const {
	if (address >= this->size) {
		throw std::out_of_range("Memory read out of bounds");
	}
	return data[address];
}

void Memory::write(address_t address, word_t value) {
	if (address >= this->size) {
		throw std::out_of_range("Memory write out of bounds");
	}
	data[address] = value;
}

void Memory::clear() {
	std::fill(data.begin(), data.end(), 0);
}

void Memory::load(const std::vector<word_t>& data_vector, address_t start_address) {
	if (start_address + data_vector.size() > this->size) {
		throw std::out_of_range("Data does not fit in memory at the specified address");
	}

	std::copy(data_vector.begin(), data_vector.end(), this->data.begin() + start_address);
}