#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
//Hs
#include "constants.h"
#include "structs.h"
#include "Queue.c"
#include "LL.c"
#include "main.c"


//todo Unit tests /////////////////////////////////////////////////////////////////////////////////////////////
//todo Unit tests /////////////////////////////////////////////////////////////////////////////////////////////
//todo Unit tests /////////////////////////////////////////////////////////////////////////////////////////////


Queue* generate_specific_instructions(int id)
{
    Queue* specific_instructions = malloc(sizeof(Queue));
    construct_queue(specific_instructions);

    if (id == 0)
    {
        Instruction* instruction_0 = malloc(sizeof(Instruction));
        construct_instruction(instruction_0, Nop, 0);
        enqueue(specific_instructions, instruction_0);

        Instruction* instruction_1 = malloc(sizeof(Instruction));
        construct_instruction(instruction_1, Nop, 0);
        enqueue(specific_instructions, instruction_1);

        Instruction* instruction_2 = malloc(sizeof(Instruction));
        construct_instruction(instruction_2, Nop, 0);
        enqueue(specific_instructions, instruction_2);

        Instruction* instruction_3 = malloc(sizeof(Instruction));
        construct_instruction(instruction_3, Nop, 0);
        enqueue(specific_instructions, instruction_3);
    }
    if (id == 1)
    {
        Instruction* instruction_0 = malloc(sizeof(Instruction));
        construct_instruction(instruction_0, Nop, 0);
        enqueue(specific_instructions, instruction_0);

        Instruction* instruction_1 = malloc(sizeof(Instruction));
        construct_instruction(instruction_1, Nop, 4);
        enqueue(specific_instructions, instruction_1);

        Instruction* instruction_2 = malloc(sizeof(Instruction));
        construct_instruction(instruction_2, Nop, 0);
        enqueue(specific_instructions, instruction_2);

        Instruction* instruction_3 = malloc(sizeof(Instruction));
        construct_instruction(instruction_3, Nop, 0);
        enqueue(specific_instructions, instruction_3);
    }
    if (id == 2)
    {
        Instruction* instruction_0 = malloc(sizeof(Instruction));
        construct_instruction(instruction_0, Nop, 0);
        enqueue(specific_instructions, instruction_0);

        Instruction* instruction_1 = malloc(sizeof(Instruction));
        construct_instruction(instruction_1, Nop, 0);
        enqueue(specific_instructions, instruction_1);

        Instruction* instruction_2 = malloc(sizeof(Instruction));
        construct_instruction(instruction_2, Nop, 0);
        enqueue(specific_instructions, instruction_2);

        Instruction* instruction_3 = malloc(sizeof(Instruction));
        construct_instruction(instruction_3, Nop, 0);
        enqueue(specific_instructions, instruction_3);
    }
    return specific_instructions;
}
void construct_specific_PE(PE* pe, int id, int reg)
{
    pe->id = id;
    pe->instructions = generate_specific_instructions(id);
    pe->reg = 100*(id+1);
}
void construct_specific_PEs()
{
    for (int i = 0; i < MAX_CORES; i++)
    {
        construct_specific_PE(PEs[i], i, i);
    }
}

void _start()
{
    int ret = test();
    exit(ret);
}
int test()
{
//    initialize_PEs();
//    construct_specific_PEs();
//    print_PEs();
    printf("F\n");
}