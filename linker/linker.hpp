#pragma once

#include "vm_types.hpp"
#include "utils.hpp"

#include <string>
#include <vector>
#include <map>

class ObjectModule{
public:
    std::string name;
    int size = 0;// Tamanho total do modulo(palavras)
    int startAddress = -1;//endereco do start(-1 se nao tiver ainda)
    int stackSizeReq = 0;// Tamanho da pilha a separar

    std::vector<word_t> code;// instrucoes

    std::vector<word_t> data;// dados

    std::vector<SymbolTableEntry> symbolTable;

    std::vector<RelocationTableEntry> relocationTable;

    void readFromFile(const std::string& filename);
};

class Linker{
public:
    Linker(RelocationMode mode = RelocationMode::FullLinkerRelocation); // Construtor com modo padrão

    //funcao para as duas passagens
    void link(const std::vector<std::string>& objFileNames, const std::string& outputFileName);

private:
    GlobalSymbolTable globalSymbolTable;
    std::vector<ObjectModule> loadedModules;
    int totalProgramSize = 0;
    int entryPoint = -1;
    int totalStackReq = 0;
    RelocationMode currentRelocationMode;

    std::vector<RelocationTableEntry> globalRelocationMap;// Construir mapa se parcial

    //Passagens
    void firstPass(const std::vector<std::string>& objFileNames);
    void secondPass(const std::string& outputFileName);

    void writeFullExecutable(std::ofstream& outputFile);
    void writeRelocatableExecutable(std::ofstream& outputFile);

    //Tratamento de erros
    static void reportError( const std::string& message);
    static void reportWarning(const std::string& message);
};
