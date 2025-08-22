#include "utils.hpp"

#include "vm_types.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <iterator>
#include <string>
#include <vector>

void save_file(std::vector<std::string> lines, std::string filename) {
    std::ofstream output_file(filename);
    std::ostream_iterator<std::string> output_iterator(output_file, "\n");
    std::copy(lines.begin(), lines.end(), output_iterator);
}

int hexStringtoint(const std::string& hexStr){
    if (hexStr.length() > 2 && hexStr[0] == '0' && (hexStr[1] == 'x' || hexStr[1] == 'X')) {
        return std::stoi(hexStr.substr(2), nullptr, 16);
    }
    return 0; 
}

std::string toHex4(int val) {
    std::stringstream ss;
    ss << std::hex << std::setw(4) << std::setfill('0') << val;
    return ss.str();
}