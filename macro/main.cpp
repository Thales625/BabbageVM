#include "macro.hpp"

#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
    std::string filename = argv[1];
    std::ifstream infile(filename);

    if (!infile) {
        std::cerr << "Error: Cannot open file '" << filename << "'" << std::endl;
        return 1;
    }

    std::vector<std::string> lines;

	std::string line;
	while (std::getline(infile, line)) {
		lines.push_back(line);
	}

    std::vector<std::string> processed_lines = Macro::process(lines);

    for (size_t i = 0; i < processed_lines.size(); ++i) {
        std::cout << (i + 1) << ": " << processed_lines[i] << std::endl;
    }

    return 0;
}