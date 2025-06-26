#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <iomanip>

// symtable(label -> addr)
std::map<std::string, int> symTable;

// source object code and listing
std::vector<std::string> srcObj;
std::vector<std::string> listing;

// error listing(line + message)
std::vector<std::string> errors;

// pos
int locCounter = 0x0000;

// aux functions
bool isComment(const std::string& line){
	return !line.empty() && line[0] == '*';
}

bool lineTooLong(const std::string& line){
	return line.length() > 80;
}

// split line into individual tokens
std::vector<std::string> tokenize(const std::string& line){
	std::stringstream ss(line);
	std::string token;
	std::vector<std::string> tokens;
	while(ss >> token)
		tokens.push_back(token);
	
	return tokens;
}

// first pass: make symtable
void firstPass(std::ifstream& src){
	std::string line;
	int numLine = 0;

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
				symTable[label] = locCounter;
			}
		}

		if(idx < tokens.size())
			opcode = tokens[idx++];

		// TODO: sub with real opcodes
		if(opcode == "START"){
			if(idx < tokens.size()){
				std::string startLabel = tokens[idx];
				symTable[startLabel] = locCounter;
			}
		}
		else if(opcode == "SPACE"){
			locCounter += 1;
		}
		else if(opcode == "CONST"){
			locCounter += 1;
		}
		else if(opcode == "STACK"){
			locCounter += 1;
		}
		else if(opcode == "END"){
			// do nothing :3
		}
		else{
			// TODO: think hard about ts(this segment)
			locCounter += 1;
		}
	}
}

void secondPass(const std::string& filename, std::ifstream& src){
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

		if(idx < tokens.size()) opcode = tokens[idx++];
		if(idx < tokens.size()) operand = tokens[idx++];

		// generate simple obj code
		// TODO: map<string, int> opcodeTable ={{"LOAD", 0x01}, ... };

		int instruction = 0;

		if(opcode == "CONST"){
			try{
				instruction = stoi(operand);
			}catch(...){
				errors.push_back("linha " + std::to_string(numLine) + ": valor CONST inválido.");
			}
		}else if(opcode == "SPACE"){
			instruction = 0;
		}else if(opcode == "START" || opcode == "END" || opcode == "STACK" || opcode == "INTDEF" || opcode == "INTUSE"){
			// no code here, just the directive
			listLine << "      " << "      " << "   " << line;
			lst << listLine.str() << std::endl;
			continue;
		}else{
			// TODO: some real work
			instruction = 0xAA;

			if(!operand.empty()){
				if(symTable.count(operand)){
					instruction =(instruction << 8) | symTable[operand];
				}else{
					errors.push_back("linha " + std::to_string(numLine) + ": símbolo não definido(" + operand + ").");
				}
			}
		}

		// objcode generated
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