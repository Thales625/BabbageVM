#pragma once

#include <string>
#include <fstream>
#include <iostream>

#include "utils.hpp"
#include "memory.hpp"
#include "cpu.hpp"

class Loader {
public:
    static word_t loadAndExecute(const std::string& hpxFileName, Memory<word_t>* memory);
};