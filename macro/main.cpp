#include "macro.hpp"

#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    std::string filename = argv[1];

    std::vector<std::string> processed_lines  = Macro::ProcessFromFile(filename);

    for (size_t i = 0; i < processed_lines.size(); ++i) {
        std::cout << (i + 1) << ": " << processed_lines[i] << std::endl;
    }

    return 0;
}