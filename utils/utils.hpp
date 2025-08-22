#pragma once

#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <map>

#include "vm_types.hpp"

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
    int relativeAddress;
};

struct HPXHeader {
    int totalProgramSize;
    int entryPoint;
    int totalStackSize;
    int relocationRequiredByLoader;
};

struct SymbolTableEntry {
    std::string name;
    int relativeAddress;
};

class ObjectModule{
public:
    std::string name;
    std::vector<word_t> code;// instrucoes e dados
    std::vector<SymbolTableEntry> definitionTable;
    std::vector<SymbolTableEntry> useTable;
    std::vector<RelocationTableEntry> relocationMap;
    int size = 0;// Tamanho total do modulo(palavras)
    int startAddress = -1;//endereco do start(-1 se nao tiver ainda)
    int stackSizeReq = 0;// Tamanho da pilha a separar

    void readFromFile(const std::string& filename);
};