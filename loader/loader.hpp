#pragma once

#include <string>
#include <fstream>
#include <iostream>

#include "utils.hpp"
#include "memory.hpp"
#include "cpu.hpp"

class Loader {
public:
    Loader() = default;
    
    void loadAndExecute(const std::string& hpxFileName, CPU& cpu, Memory<word_t>& memory);
};