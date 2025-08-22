#include "linker.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

#include <stdexcept>
#include <algorithm>

#define DEBUG

Linker::Linker(RelocationMode mode) : currentRelocationMode(mode) {}

void Linker::reportError(const std::string& message) const {
    std::cerr << "Error: " << message << std::endl;
}

void Linker::reportWarning(const std::string& message) const {
    std::cout << "Warning: " << message << std::endl;
}

void Linker::firstPass(const std::vector<std::string>& objFileNames){
    int currentLoadAddress = 0;
   
    //fresh start
    this->globalSymbolTable.clear();
    this->loadedModules.clear();
    this->totalProgramSize = 0;
    this->entryPoint = -1;
    this->totalStackReq = 0;

    for (const std::string& filename : objFileNames){
        ObjectModule module;
        try{
            module.readFromFile(filename);
        }catch (const std::runtime_error& e) {//tratamento
            reportError(e.what());
            return;
        }

        #ifdef DEBUG
        std::cout << "Module code:\n";
        for (auto& line : module.code) {
            std::cout << "Line: " << line << "\n";
        }
        #endif

        loadedModules.push_back(module);

        //processar a tabela de definicoes para construir a tabela global de simbolos
        for (const auto& symbol : module.definitionTable){
            int absoluteAddress = currentLoadAddress + symbol.relativeAddress;

            if (globalSymbolTable.count(symbol.name)) {
                reportError("Duplicado: " + symbol.name);
            } else {
                globalSymbolTable[symbol.name] = absoluteAddress;
            }
        }

        //Verificar entrada
        if (module.startAddress != -1) {
            if (this->entryPoint != -1) {
                reportError("Multiple entry points found.");
            } else {
                this->entryPoint = currentLoadAddress + module.startAddress;
            }
        }

        //Somar pilha ao total e atualizar o endereco de carga
        this->totalStackReq += module.stackSizeReq;
        currentLoadAddress += module.size;
    }

    this->totalProgramSize = currentLoadAddress;
}

void Linker::secondPass(const std::string& outputFileName) {
    std::ofstream outputFile(outputFileName, std::ios::binary);
    if (!outputFile.is_open()) {
        reportError("Nao foi possivel criar o arquivo de saida: " + outputFileName);
        return;
    }

    HPXHeader header;
    header.totalProgramSize = totalProgramSize;
    header.entryPoint = entryPoint;
    header.totalStackSize = this->totalStackReq;
    header.relocationRequiredByLoader = (currentRelocationMode == RelocationMode::PartialLinkerRelocation);

    outputFile.write(reinterpret_cast<const char*>(&header), sizeof(HPXHeader));

    if (currentRelocationMode == RelocationMode::FullLinkerRelocation) {
        this->writeFullExecutable(outputFile);
    } else {
        this->writeRelocatableExecutable(outputFile);
    }

    outputFile.close();
    std::cout << "Ligacao concluida com sucesso. Executavel gerado: " << outputFileName << std::endl;
}

// Novo método: Ligador que faz a relocação completa
void Linker::writeFullExecutable(std::ofstream& outputFile) {
    int currentLoadAddress = 0; // Endereço de carga atual para o módulo no executável final

    for (ObjectModule& module : loadedModules) {
        std::vector<word_t> moduleCode = module.code;

        // Relocação Interna
        for (const auto& relEntry : module.relocationMap) {
            if (relEntry.relativeAddress < moduleCode.size()) {
                 moduleCode[relEntry.relativeAddress] += currentLoadAddress;
            } else {
                 reportWarning("Relocation entry out of bounds for module " + module.name +
                               " at address " + std::to_string(relEntry.relativeAddress));
            }
        }

        // Resolver Referências de Uso
        for (const auto& useEntry : module.useTable) {
            auto it = globalSymbolTable.find(useEntry.name);
            if (it != globalSymbolTable.end()) {
                word_t absoluteTargetAddress = it->second;
                if (useEntry.relativeAddress < moduleCode.size()) {
                    moduleCode[useEntry.relativeAddress] = absoluteTargetAddress;
                } else {
                    reportWarning("Use entry for symbol " + useEntry.name + " out of bounds for module " + module.name + " at address " + std::to_string(useEntry.relativeAddress));
                }
            } else {
                reportError("Símbolo global nao definido: " + useEntry.name +
                            " [módulo: " + module.name + "]");
                outputFile.close();
                return;
            }
        }

        // Código Modificado para o Arquivo Executável
        outputFile.write(reinterpret_cast<const char*>(moduleCode.data()), moduleCode.size() * sizeof(word_t));

        currentLoadAddress += module.size;
    }
}

// Ligador que deixa a relocação para o Carregador
void Linker::writeRelocatableExecutable(std::ofstream& outputFile) {
    int currentLoadAddress = 0;
    globalRelocationMap.clear(); // Limpa o mapa global

    for (ObjectModule& module : loadedModules) {
        // Resolver Referências de Uso
        for (const auto& useEntry : module.useTable) {
            auto it = globalSymbolTable.find(useEntry.name);
            if (it != globalSymbolTable.end()) {
                word_t absoluteTargetAddress = it->second;
                if (useEntry.relativeAddress < module.code.size()) {
                    module.code[useEntry.relativeAddress] = absoluteTargetAddress;
                } else {
                     reportWarning("Use entry for symbol " + useEntry.name + " out of bounds for module " + module.name +
                                  " at address " + std::to_string(useEntry.relativeAddress));
                }
            } else {
                reportError("Símbolo global nao definido: " + useEntry.name +
                            " [módulo: " + module.name + "]");
                outputFile.close();
                return;
            }
        }

        for (const auto& relEntry : module.relocationMap) {
            RelocationTableEntry globalEntry = relEntry;
            globalEntry.relativeAddress += currentLoadAddress; // Ajustar o offset para ser global
            globalRelocationMap.push_back(globalEntry);
        }

        outputFile.write(reinterpret_cast<const char*>(module.code.data()), module.code.size() * sizeof(word_t));

        currentLoadAddress += module.size;
    }

    // Mapa de Relocação Global
    size_t mapSize = globalRelocationMap.size();
    outputFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize)); // Número de entradas no mapa
    outputFile.write(reinterpret_cast<const char*>(globalRelocationMap.data()), mapSize * sizeof(RelocationTableEntry));
}

//funcao principal
void Linker::link(const std::vector<std::string>& objFileNames, const std::string& outputFileName) {
    firstPass(objFileNames);

    #ifdef DEBUG
    std::cout << totalProgramSize << " | " << entryPoint << "\n";
    #endif

    if(totalProgramSize > 0 && entryPoint != -1) {
        secondPass(outputFileName);
    } else {
        reportError("Linking failed.");
    }
}