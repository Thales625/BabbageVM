#pragma once

#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <map>

#include "vm_types.hpp"

#define PRINT(msg) { std::cout << TAG << "> " << msg; }
#define PRINT_ERR(msg) { std::cerr << TAG << "> ERROR: " << msg; }
#define PRINT_WAR(msg) { std::cout << TAG << "> WARNING: " << msg; }

void save_file(std::vector<std::string> lines, std::string filename);
int hexStringtoint(const std::string& hexStr);
std::string toHex4(int val);

// LINKER & LOADER utils
using GlobalSymbolTable = std::map<std::string, int>;

enum class RelocationMode {
    FullLinkerRelocation,    // Carregador Absoluto
    PartialLinkerRelocation  // Carregador Relocador
};


struct RelocationTableEntry{
    word_t relativeAddress;
};

struct HPXHeader {
    word_t totalProgramSize;
    word_t entryPoint;
    word_t totalStackSize;
    word_t relocationRequiredByLoader;
};

struct SymbolTableEntry {
    std::string name;
    word_t relativeAddress;
};