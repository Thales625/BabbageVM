#pragma once
#include <string>
#include <vector>
#include <map>
#include <stdint.h>

class Assembler {
public:
    void assemble(const std::string& filename);

private:
    std::map<std::string, int> symTable;
    std::vector<std::string> objOutput;
    std::vector<std::string> lstOutput;
    std::vector<std::string> errors;
    int locationCounter = 0x0000;
    int startAddress = 0;

    // Checks if a line is a comment by searching for the '*' character at the start of the line.
    bool isComment(const std::string& line) const;

    // Checks if a line is too long (>80 characters).
    bool lineTooLong(const std::string& line) const;

    // Splits a line into tokens to be later used in parsing shenanigans.
    std::vector<std::string> tokenize(const std::string& line) const;

    // Helper function to format addresses in 4-digit hex
    std::string toHex4(int val) const;

    /**
     * @brief 			Encodes a 16-bit instruction word using opcode and operand addressing modes
     * @param opcode	8-bit opcode of the instruction
     * @param addrMode1	Addressing mode of the first operand (00 = direct, 01 = indirect, 10 = immediate)
     * @param addrMode2 Addressing mode of the second operand
     * @return 			16-bit encoded instruction word
     */
    uint16_t encodeInstruction(uint8_t opcode, uint8_t addrMode1, uint8_t addrMode2) const;


    /**
     * @brief       Makes a symbol table by identifyind all labels and their respective addresses.
     *              No machine code is generated on this pass.
     * @param src   Source file to be assembled (MASMAPRG.ASM)
    */ 
    void firstPass(std::ifstream& src);

    /**
     * @brief           Translates assembly instructions into machine code and solves forward references.
     * @param src       Source file to be assembled (MASMAPRG.ASM)
     * @param filename  Filename to write output to (MASMAPRG.OBJ and MASMAPRG.LST)
    */ 
    void secondPass(std::ifstream& src, const std::string& filename);

    //void writeOutput(const std::string& filename) const;
};
