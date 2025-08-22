#include "utils.hpp"

#include "vm_types.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

#include <iterator>
#include <string>
#include <vector>

#include <stdexcept>
#include <algorithm>

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

// LINKER & LOADER utils

void ObjectModule::readFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        throw std::runtime_error("File opening error");
    }

    std::string line;
    enum Section {NONE, HEADER, CODE, DEFS, USES, RELOCS};
    Section currentSection = NONE;

    while (std::getline(file, line)){
        std::cout << "LINE: "<< line << "\n";

        //tratamento de comentario e linhas vazias
        if (line.empty() || line[0] == ';') continue;

        if(line == "#HEADER") {
            currentSection = HEADER;
            continue;
        } else if (line == "#CODE_SECTION" || line == "CODE:") {
            currentSection = CODE;
            continue;
        } else if (line == "#DEFINITION_TABLE" || line == "DEFINITIONS:") {
            currentSection = DEFS;
            continue;
        } else if (line == "#USE_TABLE" || line == "USES:") {
            currentSection = USES;
            continue;
        } else if (line == "#RELOCATION_TABLE" || line == "RELOCATIONS:") {
            currentSection = RELOCS;
            continue;
        }

        std::istringstream iss(line);
        std::string key;

        switch (currentSection) {
            case HEADER: {
                std::getline(iss, key, ':');
                if (key == "MODULE_NAME") iss >> this->name;
                else if (key == "MODULE_SIZE") iss >> this->size;
                else if (key == "START_ADDRESS") iss >> this->startAddress;
                else if (key == "STACK_SIZE_REQ") iss >> this->stackSizeReq;
                break;
            }
            case CODE:{
                std::string hexValStr;
                iss >> hexValStr;
                size_t commentPos = hexValStr.find(';');//tratamento de comentario
                if (commentPos != std::string::npos){
                    hexValStr = hexValStr.substr(0, commentPos);
                }
                if (!hexValStr.empty()){
                    this->code.push_back(static_cast<word_t>(hexStringtoint(hexValStr)));
                }
                break;
            }
            case DEFS:{
                SymbolTableEntry entry;
                iss >> entry.name >> entry.relativeAddress;
                this->definitionTable.push_back(entry);
                break;
            }
            case USES:{
                SymbolTableEntry entry;
                iss >> entry.name >> entry.relativeAddress;
                this->useTable.push_back(entry);
                break;
            }
            case RELOCS:{
                RelocationTableEntry entry;
                iss >> entry.relativeAddress;
                this->relocationMap.push_back(entry);
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
        std::cerr << "Warning: Module size does not match code size." << std::endl;
    }
}