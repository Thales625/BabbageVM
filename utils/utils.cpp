#include "utils.hpp"

void save_file(std::vector<std::string> lines, std::string filename) {
    std::ofstream output_file(filename);
    std::ostream_iterator<std::string> output_iterator(output_file, "\n");
    std::copy(lines.begin(), lines.end(), output_iterator);
}