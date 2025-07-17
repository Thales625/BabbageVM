#include "macro.hpp"

#include <iostream>

int main() {
    std::vector<std::string> lines = {
        ".macro subtrair %x %y",
        "sub %x %y",
        ".end_macro",

        ".macro adicionar %x %y",
        "sum %x %y",
        "subtrair(%y %x)",
        ".end_macro",

        "adicionar(1 2)"
    };

    std::vector<std::string> processed_lines = Macro::process(lines);

    for (size_t i = 0; i < processed_lines.size(); ++i) {
        std::cout << (i + 1) << ": " << processed_lines[i] << std::endl;
    }

    return 0;
}