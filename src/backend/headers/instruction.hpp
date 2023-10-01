#ifndef INSTRUCTION_H // Include guards to prevent multiple inclusions
#define INSTRUCTION_H

#include <string>

struct instruction 
{
    std::string opcode;
    int r1;
    int r2;
};

#endif // INSTRUCTION_H
