#include <string>
#include <fstream>
#include <iostream>

#include "memory.hpp"
#include "cpu.hpp"
#include "linkerutils.hpp"

class Loader {
public:
    Loader() = default;
    
    void loadAndExecute(const std::string& hpxFileName, CPU& cpu, Memory<word_t>& memory);
};