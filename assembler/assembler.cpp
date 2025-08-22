#include "assembler.hpp"

#include "vm_types.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

// opcodes, not finished!
// TODO: different write instructions for r and i
std::map<std::string, word_t> opcodeTable = {
    {"RET",     16},
    {"STOP",    11},
    {"ADD",     02},
    {"BR",      00},
    {"BRNEG",   05},
    {"BRPOS",   01},
    {"BRZERO",  04},
    {"CALL",    15},
    {"DIVIDE",  10},
    {"LOAD",    03},
    {"MULT",    14},
    {"PUSH",    17},
    {"STORE",   07},
    {"SUB",     06},
    {"POP",     18},
    {"COPYM",   13},
    {"COPYM",   19},
};

void Assembler::assemble(const std::string& filename) {
    std::ifstream infile(filename);

    if (!infile) {
        std::cerr << "Error: Cannot open file '" << filename << "'" << std::endl;
        exit(EXIT_FAILURE);
    }

    this->firstPass(infile);

    this->secondPass(infile, "bin/assembler_out");

    /*
    std::vector<std::string> lines;
    
	std::string line;
	while (std::getline(infile, line)) {
		lines.push_back(line);
	}
    */
}

bool Assembler::isComment(const std::string& line) const {
    return !line.empty() && line[0] == '*';
}

bool Assembler::lineTooLong(const std::string& line) const {
    return line.length() > 80;
}

uint16_t Assembler::encodeInstruction(uint8_t opcode, uint8_t addrMode1, uint8_t addrMode2) const {
    return (opcode << 8) | (addrMode2 << 4) | addrMode1;
}

std::vector<std::string> Assembler::tokenize(const std::string& line) const {
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> tokens;
    while(ss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string Assembler::toHex4(int val) const {
    std::stringstream ss;
    ss << std::hex << std::setw(4) << std::setfill('0') << val;
    return ss.str();
}

void Assembler::firstPass(std::ifstream& src) {
    std::string line;
    int numLine = 0;

    bool foundStart = false;
    bool foundEnd = false;

    while(getline(src, line)) {
        numLine++;
        if(lineTooLong(line)){
            errors.push_back("linha " + std::to_string(numLine) + ": linha muito longa.");
            continue;
        }

        if(isComment(line)) continue;

        std::vector<std::string> tokens = tokenize(line);

        if(tokens.empty()) continue;

        std::string label, opcode;
        int idx = 0;

        // label check in the first column
        if(line[0] != ' ' && line[0] != '\t') {
            label = tokens[idx++];
            if(symTable.count(label)){
                errors.push_back("linha " + std::to_string(numLine) + ": símbolo redefinido(" + label + ").");
            }else{
                symTable[label] = locationCounter; //maps the label to its respective memory address
            }
        }

        if(idx < tokens.size())
            opcode = tokens[idx++];

        if(opcode == "START"){
            if(idx < tokens.size()) {
                std::string startLabel = tokens[idx];

                if(isdigit(startLabel[0])){
                    // if digit, just get the address and pass it to lc
                    startAddress = std::stoi(startLabel);
                    locationCounter = startAddress;
                }else if(symTable.count(startLabel)){
                    // if previously defined (can cause a redefined symbol, dont care, solve later on the 2nd pass)
                    startAddress = symTable[startLabel];
                    locationCounter = symTable[startLabel];
                }else{
                    // fallback: label not found yet, can be defined later
                    startAddress = locationCounter;
                    symTable[startLabel] = locationCounter;
                }

                foundStart = true;
            }
        }

        // all of those need 1word
        else if(opcode == "SPACE" || opcode == "CONST" || opcode == "STACK") {
            locationCounter += 1;
        }
        else if(opcode == "END") {
            if(foundStart){
                foundEnd = true;
            }else{
                errors.push_back("linha " + std::to_string(numLine) + ": END sem START associado");
            }
            break; //get off
        }
        
        // if its not a directive, its a normal instruction -> 1 word
        else {
            locationCounter += 1;
        }
    }
}

void Assembler::secondPass(std::ifstream& src, const std::string& filename) {
    std::string line;
    int numLine = 0;
    int pc = startAddress;

    std::ofstream obj(filename + ".OBJ");
    std::ofstream lst(filename + ".LST");

    src.clear();		//clear EOF flags
    src.seekg(0);		//move cursor to the start

    while(getline(src, line)) {
        numLine++;

        if(isComment(line)){
            lst << "      " << "      " << "   " << line << std::endl;
            continue;
        }

        if(lineTooLong(line)){
            lst << "XXXX  ERRO  linha muito longa: " << line << std::endl;
            continue;
        }

        std::vector<std::string> tokens = tokenize(line);
        if(tokens.empty()) continue;

        int idx = 0;
        std::string label, opcode, operand1, operand2;

        if(line[0] != ' ' && line[0] != '\t'){
            label = tokens[idx++];
        }

        // extract opcode and operands (if it exists in current line)
        if(idx < tokens.size()) opcode = tokens[idx++];
        if(idx < tokens.size()) operand1 = tokens[idx++];
        if(idx < tokens.size()) operand2 = tokens[idx++];

        int instruction = 0;
        int val1 = 0, val2 = 0;

        //handle assembler instructions
        if(opcode == "CONST"){
            try{
                instruction = std::stoi(operand1);
            }catch(...){
                errors.push_back("linha " + std::to_string(numLine) + ": valor CONST inválido.");
            }
        }else if(opcode == "SPACE") {
            instruction = 0;
        }else if(opcode == "START" || opcode == "END" || opcode == "STACK" || opcode == "INTDEF" || opcode == "INTUSE"){
            lst << "      " << "      " << "   " << line << std::endl;
            continue;
        }else if(opcodeTable.count(opcode)) {
            //opcodes the fato, se existir pega da table
            int opcodeBin = opcodeTable[opcode];

            // std::cout << opcode << " -> " << opcodeBin << "\n";

            // no operand: just shift opcodeBin to its actual place
            // example: STOP = 0x000B << 8 = 0x0B00
            if(operand1.empty()){
                instruction = (opcodeBin << 8);
            }else{
                uint8_t addrMode1 = 0, addrMode2 = 0;
                
                // cute little lambda fun for addrMode checking
                auto parseOperand = [&](std::string& op, uint8_t& mode, int& val){
                    if(op.empty()) return;

                    if(op.front() == '#'){
                        mode = 0x2;		//immediate
                        op = op.substr(1);
                    }else if(op.size() > 2 && op.substr(op.size() - 2) == ",I"){
                        mode = 0x1;		//indirect
                        op = op.substr(0, op.size() - 2);
                    }else{
                        mode = 0x0;		//direct
                    }

                    //resolve operand address (direct or previously stored in symTable)
                    if(isdigit(op[0])) {
                        val = std::stoi(op);
                    }else if(symTable.count(op)) {
                        val = symTable[op];
                    }else{
                        errors.push_back("linha " + std::to_string(numLine) + ": simbolo nao definido (" + op + ")");
                    }
                };

                parseOperand(operand1, addrMode1, val1);
                parseOperand(operand2, addrMode2, val2);

                instruction = encodeInstruction(opcodeBin, addrMode1, addrMode2);
            }
        }

        obj << toHex4(instruction) << std::endl;
        lst << toHex4(pc) << "  " << toHex4(instruction) << "   " << line << std::endl;
        pc++;

        if(!operand1.empty()){
            obj << toHex4(val1) << std::endl;
            lst << toHex4(pc) << "  " << toHex4(val1) << std::endl;
            pc++; 
        }
        if(!operand2.empty()){
            obj << toHex4(val2) << std::endl;
            lst << toHex4(pc) << "  " << toHex4(val2) << std::endl;
            pc++;
        }

    }

    // error report
    lst << std::endl;
    if(errors.empty()) {
        lst << "Nenhum erro detectado." << std::endl;
    }else{
        lst << "Foram encontrados os seguintes erros:" << std::endl;
        for(const std::string& erro : errors)
            lst << erro << std::endl;
    }
    
    obj.close();
    lst.close();
}