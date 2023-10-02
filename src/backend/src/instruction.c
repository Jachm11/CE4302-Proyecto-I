#include <stdio.h>
#include <stdlib.h>

#include "coherence_protocol_constants.h"
#include "structs.h"

///Instruction type methods
unsigned char generate_random_type_instruction()
{
    int random = rand()%3;
    switch (random)
    {
        case 0: return Read;
        case 1: return Write;
        case 2: return Increment;
        default: return Nop;
    }
}

///Instruction methods
void print_instruction(Instruction instruction)
{
    printf("Type: %c\nMem addr: %d\nReg addr: %d\nData: %d\n", instruction.type, instruction.mem_addr);
}
Instruction* construct_instruction(Instruction* instruction, unsigned char type, int mem_addr)
{
    instruction->type = type;
    instruction->mem_addr = mem_addr;
}
Queue* generate_random_instructions()
{
    Queue* random_instructions = malloc(sizeof(Queue));
    construct_queue(random_instructions);
    for (int i = 0; i < MAX_INSTRUCTIONS; i++)
    {
        Instruction* instruction = malloc(sizeof(Instruction));
        construct_instruction(instruction, generate_random_type_instruction(), rand()%RAM_ENTRIES);
        enqueue(random_instructions, instruction);
    }
    return random_instructions;
}