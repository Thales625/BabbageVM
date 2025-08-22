#include "linker.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <string>
#include <stdexcept>
#include <algorithm>
#include <iterator>

#define DEBUG

#define TAG "LINKER"

Linker::Linker(RelocationMode mode) : currentRelocationMode(mode) {}

//funcao principal
void Linker::link(const std::vector<std::string>& objFileNames, const std::string& outputFileName) {
    firstPass(objFileNames);

    #ifdef DEBUG
    PRINT(totalProgramSize << " | " << entryPoint << "\n";)
    #endif

    if(totalProgramSize > 0 && entryPoint != -1) {
        secondPass(outputFileName);
    } else {
        reportError("Linking failed.");
    }
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
        PRINT("Module Symbol table:\n")
        for (auto& entry : module.symbolTable) {
            PRINT("name: " << entry.name << " | addrs: " << entry.relativeAddress << "\n")
        }

        std::cout << "\n";

        PRINT("Module code:\n")
        for (auto& line : module.code) {
            PRINT("Line: " << line << "\n")
        }

        std::cout << "\n";
        PRINT("Module INFO:\n")
        PRINT("name: " << module.name << "\n")
        PRINT("size: " << module.size << "\n")
        PRINT("start address: " << module.startAddress << "\n")
        PRINT("stack size: " << module.stackSizeReq << "\n")

        std::cout << "\n";
        #endif

        loadedModules.push_back(module);

        //processar a tabela de definicoes para construir a tabela global de simbolos
        for (const auto& symbol : module.symbolTable){
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
    PRINT("Ligacao concluida com sucesso. Executavel gerado: " << outputFileName << std::endl)
}

// Novo método: Ligador que faz a relocação completa
void Linker::writeFullExecutable(std::ofstream& outputFile) {
    int currentLoadAddress = 0; // Endereço de carga atual para o módulo no executável final

    for (ObjectModule& module : loadedModules) {
        std::vector<word_t> moduleCode = module.code;

        // Relocação Interna
        for (const auto& relEntry : module.relocationTable) {
            if (relEntry.relativeAddress < moduleCode.size()) {
                moduleCode[relEntry.relativeAddress] += currentLoadAddress;
            } else {
                reportWarning("Relocation entry out of bounds for module " + module.name + " at address " + std::to_string(relEntry.relativeAddress));
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
        for (const auto& relEntry : module.relocationTable) {
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

void Linker::reportError(const std::string& message) {
    PRINT_ERR(message << std::endl);
}

void Linker::reportWarning(const std::string& message) {
    PRINT_WAR(message << std::endl);
}



void ObjectModule::readFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        PRINT_ERR("Error opening file: " << filename << std::endl)
        throw std::runtime_error("File opening error");
    }

    std::string line;
    enum Section {NONE, HEADER, TEXT, DATA, SYMBOL, RELOCS};
    Section currentSection = NONE;

    while (std::getline(file, line)){
        PRINT("LINE: "<< line << "\n")

        //tratamento de comentario e linhas vazias
        if (line.empty() || line[0] == ';') continue;

        if(line == "#HEADER") {
            currentSection = HEADER;
            continue;
        } else if (line == "#TEXT_SECTION") {
            currentSection = TEXT;
            continue;
        } else if (line == "#DATA_SECTION") {
            PRINT("COCOCOCOCO\n")
            currentSection = DATA;
            continue;
        } else if (line == "#SYMBOL_TABLE") {
            currentSection = SYMBOL;
            continue;
        } else if (line == "#RELOCATION_TABLE") {
            currentSection = RELOCS;
            continue;
        }

        std::istringstream iss(line);
        std::string key;

        switch (currentSection) {
            case HEADER: {
                std::getline(iss, key, ' ');
                if (key == "NAME") iss >> this->name;
                else if (key == "SIZE") iss >> this->size;
                else if (key == "START_ADDRESS") iss >> this->startAddress;
                else if (key == "STACK_SIZE_REQ") iss >> this->stackSizeReq;
                break;
            }
            case TEXT:{
                std::string hexValStr;
                iss >> hexValStr;
                size_t commentPos = hexValStr.find('*'); //tratamento de comentario
                if (commentPos != std::string::npos){
                    hexValStr = hexValStr.substr(0, commentPos);
                }
                if (!hexValStr.empty()){
                    this->code.push_back(static_cast<word_t>(hexStringtoint(hexValStr)));
                }
                break;
            }
            case DATA:{
            }
            case SYMBOL:{
                SymbolTableEntry entry;
                iss >> entry.name >> entry.relativeAddress;
                this->symbolTable.push_back(entry);
                break;
            }
            case RELOCS:{
                RelocationTableEntry entry;
                iss >> entry.relativeAddress;
                this->relocationTable.push_back(entry);
                break;
            }
            case NONE:
            default:
                break;
        }

    }
    file.close();

    //verificacao de precaucao
    if (this->size == 0 && !this->code.empty()) {
        this->size = this->code.size();
    } else if (this->size != this->code.size() && this->size > 0) {
        PRINT_WAR("Module size does not match code size.\n")
    }
}