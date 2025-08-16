#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "vm_types.hpp"

struct SymbolTableEntry {
    std::string name;
    int relative_address;
    char sign = '+';

    void print() const {//Conferir se ta certinho
        std::cout << "SymbolEntry: " << name 
                  << " Relative Address: " << relative_address 
                  << " with sign " << sign << std::endl;
    }
};

struct RelocationTableEntry{
    int relativeAdress;
    //more realoc methods??????
    void print() const {//Conferir se ta certinho
        std::cout << "Relocation Address: " << relativeAdress << std::endl;
    }
};

class ObjectModule{
    public:
        std::string name;
        std::vector<word_t> code;// instrucoes e dados
        std::vector<SymbolTableEntry> definitionTable;
        std::vector<SymbolTableEntry> useTable;
        std::vector<RelocationTableEntry> relocationMap;
        int size = 0;// Tamanho total do modulo(palavras)
        int startAdress = -1;//endereco do start(-1 se nao tiver ainda)
        int stackSizeReq = 0;// Tamanho da pilha a separar
};

//Print de tudo pra debuggar depois(priguica)

//Agora comeca o arquivo gerado executavel .HPX
struct HPXheader {
    int totalProgramsize;
    int entryPoint;
    int totalStackSize;
    int relocationRequiredByLoader
};

using GlobalSymbolTable = std::map::<std::string, int>;

//converter hex para int dec
int hexStringtoint(const std::string& hexStr);