#include "assembler.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

// opcodes, not finished!
// TODO: different write instructions for r and i
std::map<std::string, int> opcodeTable = {
	{"BR",		0x00},
	{"BRPOS",	0x01},
	{"ADD",		0x02},
	{"LOAD",	0x03},
	{"BRZERO",	0x04},
	{"BRNEG",	0x05},
	{"SUB",		0x06},
	{"STORE",	0x07},
	{"WRITE",	0x08},
	{"",	0x09},
	{"DIVIDE",	0x0A},
	{"STOP",	0x0B},
	{"READ",	0x0C},
	{"COPY",	0x0D},
	{"MULT",	0x0E},
	{"CALL",	0x0F},
	{"RET",		0x10},
};

bool Assembler::isComment(const std::string& line) const{
	return !line.empty() && line[0] == '*';
}

bool Assembler::lineTooLong(const std::string& line) const{
	return line.length() > 80;
}

uint16_t Assembler::encodeInstruction(uint8_t opcode, uint8_t addrMode1, uint8_t addrMode2) const{
	return (opcode << 8) | (addrMode2 << 4) | addrMode1;
}

std::vector<std::string> Assembler::tokenize(const std::string& line) const{
	std::stringstream ss(line);
	std::string token;
	std::vector<std::string> tokens;
	while(ss >> token)
		tokens.push_back(token);
	
	return tokens;
}

void Assembler::firstPass(std::ifstream& src){
	std::string line;
	int numLine = 0;

	bool foundStart = false;
	bool foundEnd = false;

	while(getline(src, line)){
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
		if(line[0] != ' ' && line[0] != '\t'){
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
			if(idx < tokens.size()){
				std::string startLabel = tokens[idx];
				symTable[startLabel] = locationCounter;
				foundStart = true;
			}
		}

		// all of those need 1word
		else if(opcode == "SPACE" || opcode == "CONST" || opcode == "STACK") {
            locationCounter += 1;
		}
		else if(opcode == "END"){
			if(foundStart){
				foundEnd = true;
			}else{
				errors.push_back("linha " + std::to_string(numLine) + ": END sem START associado");
			}
			break; //get off
		}
		
		// if its not a directive, its a normal instruction -> 1 word
		else{
			locationCounter += 1;
		}
	}
}

void Assembler::secondPass(std::ifstream& src, const std::string& filename){
	std::string line;
	int numLine = 0;
	int pc = 0x0000;	//position counter for listing

	std::ofstream obj(filename + ".OBJ");
	std::ofstream lst(filename + ".LST");

	src.clear();		//clear EOF flags
	src.seekg(0);		//move cursor to the start

	while(getline(src, line)){
		numLine++;

		std::stringstream listLine;
		std::string output = "----";	//placeholder

		if(isComment(line)){
			listLine << "      " << "      " << "   " << line;
			lst << listLine.str() << std::endl;
			continue;
		}

		if(lineTooLong(line)){
			listLine << "XXXX  ERRO  linha muito longa: " << line;
			lst << listLine.str() << std::endl;
			continue;
		}

		std::vector<std::string> tokens = tokenize(line);
		if(tokens.empty()) continue;

		int idx = 0;
		std::string label, opcode, operand;

		if(line[0] != ' ' && line[0] != '\t'){
			label = tokens[idx++];
		}

		// extract opcode and operand (if it exists in current line)
		if(idx < tokens.size()) opcode = tokens[idx++];
		if(idx < tokens.size()) operand = tokens[idx++];

		int instruction = 0;

		//handle assembler instructions
		if(opcode == "CONST"){
			try{
				instruction = stoi(operand);
			}catch(...){
				errors.push_back("linha " + std::to_string(numLine) + ": valor CONST inválido.");
			}
		}else if(opcode == "SPACE"){
			instruction = 0;
		}else if(opcode == "START" || opcode == "END" || opcode == "STACK" || opcode == "INTDEF" || opcode == "INTUSE"){
			listLine << "      " << "      " << "   " << line;
			lst << listLine.str() << std::endl;
			continue;
		}else if(opcodeTable.count(opcode)){
			//opcodes the fato, se existir pega da table
			int opcodeBin = opcodeTable[opcode];

			// no operand: just shift opcodeBin to its actual place
			// example: STOP = 0x000B << 8 = 0x0B00
			if(operand.empty()){
				instruction = (opcodeBin << 8);
			}else{
				//immediate and indirect checking
				uint8_t addrMode1 = 0x0;
				uint8_t addrMode2 = 0x0;
				
				// #operand
				if(operand.front() == '#'){
					addrMode1 = 0x2;
					operand = operand.substr(1); //skip # and interpret the rest as operand
				}

				// operand,I
				if(operand.size() > 2 && operand.substr(operand.size() - 2) == ",I"){
					addrMode1 = 0x1;
					operand = operand.substr(0, operand.size() - 2); //interpret the operand as normal but skip last 2 chars
				}

				//resolve operand address (direct or previously stored in symTable)
				int address = 0;
				if(isdigit(operand[0])){
					address = stoi(operand);
				}else if(symTable.count(operand)){
					address = symTable[operand];
				}else{
					errors.push_back("linha " + std::to_string(numLine) + ": simbolo nao definido (" + operand + ")");
				}

				instruction = encodeInstruction(opcodeBin, addrMode1, addrMode2);
			}
		}

		//TODO: write this damn thing to a file already!
		std::stringstream codhex;
		codhex << std::hex << std::setw(4) << std::setfill('0') << instruction;
		output = codhex.str();

		obj << output << std::endl;

		listLine << std::hex << std::setw(4) << std::setfill('0') << pc << "  ";
		listLine << output << "   " << line;
		lst << listLine.str() << std::endl;

		pc += 1;
	}

	// error report
	lst << std::endl;
	if(errors.empty()){
		lst << "Nenhum erro detectado." << std::endl;
	}else{
		lst << "Foram encontrados os seguintes erros:" << std::endl;
		for(const std::string& erro : errors)
			lst << erro << std::endl;
	}
    
	obj.close();
	lst.close();
}