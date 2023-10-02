#include <stdio.h>
#include <stdlib.h>
//
#include "coherence_protocol_constants.h"
#include "structs.h"

///construct_PEs methods
void initialize_PEs(PE** PEs)
{
    //PEs = malloc(MAX_CORES * sizeof(PE));
    for (int i = 0; i < MAX_CORES; i++)
    {
        PEs[i] = malloc(sizeof(PE));
    }
}
void construct_PE(PE* pe, int id)
{
    pe->id = id;
    pe->instructions = generate_random_instructions();
    pe->reg=100*(id+1);
    pe->MT_done=true;
    pe->process_ended=false;
}
void construct_PEs(PE** PEs)
{
    for (int i = 0; i < MAX_CORES; i++)
    {
        construct_PE(PEs[i], i);
    }
}
void print_PE(PE* pe)
{
    printf("PE: %d\n", pe->id);
    printf("Instructions:\n");
    Queue_instruction_println(pe->instructions);
    printf("Reg: %d\n", pe->reg);
    printf("MT_done: %d\n", pe->MT_done);
}
void print_PEs(PE** PEs)
{
    //printf_green_on();
    for (int i = 0; i < MAX_CORES; i++)
    {
        print_PE(PEs[i]);
        printf("\n");
    }
    //printf_color_reset();
}
