#pragma once

#include "vm_types.hpp"
#include "utils.hpp"

#include <string>
#include <vector>
#include <map>


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

    //Tratamento de erros
    void reportError( const std::string& message) const;
    void reportWarning(const std::string& message) const;

    void writeFullExecutable(std::ofstream& outputFile);
    void writeRelocatableExecutable(std::ofstream& outputFile);
};