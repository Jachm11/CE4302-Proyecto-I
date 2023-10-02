#include <stdio.h>
#include <stdlib.h>
//
#include "coherence_protocol_constants.h"
#include "structs.h"

///RAM methods
void fill_RAMdom(int* RAM)
{
    for (int i = 0; i < RAM_ENTRIES; ++i)
    {
        RAM[i] = rand()%MAX_RAM_RAND_VALUE;
    }
}
void fill_RAM(int* RAM)
{
    for (int i = 0; i < RAM_ENTRIES; ++i)
    {
        RAM[i] = (i+1)*1000;
    }
}
int get_RAM_block(int* RAM, int mem_addr, int* RAM_reads_counter)
{
    (*RAM_reads_counter)++;
    return RAM[mem_addr];
}
void set_RAM_block(int* RAM, int tag, int data, int* RAM_writes_counter)
{
    (*RAM_writes_counter)++;
    RAM[tag] = data;
}
void print_RAM(int* RAM)
{
    printf_red_on();
    printf("RAM:\n[");
    for (int i = 0; i < RAM_ENTRIES; i++)
    {
        printf(" %d:%d", i, RAM[i]);
    }
    printf(" ]\n");
    printf_color_reset();
}
