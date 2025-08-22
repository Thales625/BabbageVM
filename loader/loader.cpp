#include "loader.hpp"

#include <vector> 
#include <iostream>

void Loader::loadAndExecute(const std::string& hpxFileName, CPU& cpu, Memory<word_t>& memory) {
    std::ifstream inputFile(hpxFileName, std::ios::binary);
    if (!inputFile.is_open()) {
        std::cerr << "Erro no Carregador: Nao foi possivel abrir o arquivo executavel " << hpxFileName << std::endl;
        return;
    }

    HPXHeader header;
    inputFile.read(reinterpret_cast<char*>(&header), sizeof(HPXHeader));

    //verificacoes
    if (header.totalProgramSize <= 0 || header.entryPoint == -1) {
        std::cerr << "Erro no Carregador: Cabecalho do arquivo HPX invalido." << std::endl;
        inputFile.close();
        return;
    }

    if (header.totalProgramSize + header.totalStackSize + 10 > memory.get_size()) {
        std::cerr << "Erro no Carregador: Memoria da VM insuficiente para o programa e pilha." << std::endl;
        inputFile.close();
        return;
    }

    std::vector<word_t> programCode(header.totalProgramSize);
    inputFile.read(reinterpret_cast<char*>(programCode.data()), header.totalProgramSize * sizeof(word_t));

    // Condicional para relocação
    if (header.relocationRequiredByLoader) {
        // Carregador Relocador
        size_t mapSize;
        inputFile.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize));
        std::vector<RelocationTableEntry> relocationMap(mapSize);
        inputFile.read(reinterpret_cast<char*>(relocationMap.data()), mapSize * sizeof(RelocationTableEntry));

        // Relocação
        std::cout << "Carregador: Relocacao necessaria. Aplicando...\n";
        for (const auto& relEntry : relocationMap) {
            if (relEntry.relativeAddress < programCode.size()) {
                programCode[relEntry.relativeAddress] += 0; // O endereço base de carga é 0 no seu caso (início da memória)
            } else {
                std::cerr << "Aviso no Carregador: Entrada de relocacao fora dos limites do codigo: " << relEntry.relativeAddress << std::endl;
            }
        }
    }

    return;

    inputFile.close();

    memory.load(programCode, 0);
    
    cpu.reset();
    cpu.set_entry(header.entryPoint);
    
    std::cout << "Programa carregado. Ponto de entrada: " << header.entryPoint
    << ", Tamanho total: " << header.totalProgramSize
    << ", Tamanho da pilha: " << header.totalStackSize
    << (header.relocationRequiredByLoader ? " (Relocacao feita pelo carregador)" : " (Relocacao feita pelo ligador)")
    << std::endl;
    
    cpu.run();
}