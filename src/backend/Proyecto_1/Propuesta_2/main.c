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


///Globals
int global_thread_PE_id_counter=0;
bool is_MESI = true;

///Perfilacion
int increments_counter=0;
int interconnect_read_requests_counter=0;
int interconnect_write_requests_counter=0;
int RAM_reads_counter=0;
int RAM_writes_counter=0;
//int cache_hit_counter=0;
//int cache_miss_counter=0;
int cache_data_shares_counter=0;
int invalidates_counter=0;

///Semaphores
sem_t sem_debug_prints;
sem_t sem_thread_PE_id;
sem_t sem_edit_interconnect_queue;
sem_t sem_read_exclusive_request;

sem_t sem_increments_counter;
///PEs
PE* PEs[MAX_CORES];
pthread_t PE_threads[MAX_CORES];

///Caches
Cache* caches[MAX_CORES];

///Interconnect
Interconnect* interconnect;
pthread_t interconnect_thread;

///RAM
int RAM[RAM_ENTRIES];

///Log
Queue* MT_history;

///Debug shit
void printf_red(const char* str)
{
    printf("%s%s%s", RED, str, DEFAULT_COLOR);
}
void printf_red_on()
{
    printf("%s", RED);
}
void printf_yellow(const char* str)
{
    printf("%s%s%s", YELLOW, str, DEFAULT_COLOR);
}
void printf_yellow_on()
{
    printf("%s", YELLOW);
}
void printf_green(const char* str)
{
    printf("%s%s%s", GREEN, str, DEFAULT_COLOR);
}
void printf_green_on()
{
    printf("%s", GREEN);
}
void printf_blue(const char* str)
{
    printf("%s%s%s", BLUE, str, DEFAULT_COLOR);
}
void printf_blue_on()
{
    printf("%s", BLUE);
}
void printf_cyan(const char* str)
{
    printf("%s%s%s", CYAN, str, DEFAULT_COLOR);
}
void printf_cyan_on()
{
    printf("%s", CYAN);
}
void printf_purple(const char* str)
{
    printf("%s%s%s", PURPLE, str, DEFAULT_COLOR);
}
void printf_purple_on()
{
    printf("%s", PURPLE);
}
void printf_white(const char* str)
{
    printf("%s%s%s", WHITE, str, DEFAULT_COLOR);
}
void printf_white_on()
{
    printf("%s", WHITE);
}
void printf_black(const char* str)
{
    printf("%s%s%s", BLACK, str, DEFAULT_COLOR);
}
void printf_black_on()
{
    printf("%s", BLACK);
}
void printf_color_reset()
{
    printf("%s", DEFAULT_COLOR);
}

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
///PE methods
void initialize_PEs()
{
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
void construct_PEs()
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
void print_PEs()
{
    //printf_green_on();
    for (int i = 0; i < MAX_CORES; i++)
    {
        print_PE(PEs[i]);
        printf("\n");
    }
    //printf_color_reset();
}

///Cache line methods
void construct_cache_line(Cache_line* line, int tag, bool is_valid, bool is_dirty, protocol_state state, int data)
{
    line->tag=tag;
    line->is_valid=is_valid;
    line->is_dirty=is_dirty;
    line->state = state;
    line->data = data;
}
Cache_line** generate_cache_lines()
{
    Cache_line** cache_lines = malloc(CACHE_ENTRIES*sizeof(Cache_line*));
    if(cache_lines == NULL)
    {
        fprintf(stderr, "construct_cache_line/cache_lines/Memory allocation failed\n");
        exit(1);
    }
    for (int i = 0; i < CACHE_ENTRIES; i++)
    {
        cache_lines[i] = malloc(sizeof(Cache_line));
        if(cache_lines[i] == NULL)
        {
            fprintf(stderr, "construct_cache_line/cache_lines[i]/Memory allocation failed\n");
            exit(1);
        }
        construct_cache_line(cache_lines[i], 0, false, false, Invalid, 0);
    }
    return cache_lines;
}
Cache_line* get_cache_line(int cache_id, int line_addr)
{
    return caches[cache_id]->lines[line_addr];
}
void set_cache_line(int cache_id, int line_addr, int tag, bool is_valid, bool is_dirty, protocol_state state, int data)
{
    construct_cache_line(caches[cache_id]->lines[line_addr], tag, is_valid, is_dirty, state, data);
}
void print_cache_line(Cache_line* line)
{
    printf("Tag: %d\tValid: %d\tDirty: %d\tState: %c\tData: %d\n", line->tag, line->is_valid, line->is_dirty, line->state, line->data);
}
void print_cache_lines(Cache_line** lines)
{
    for (int i = 0; i < CACHE_ENTRIES; i++)
    {
        print_cache_line(lines[i]);
    }
}

///Cache methods
void initialize_Caches()
{
    for (int i = 0; i < MAX_CORES; i++)
    {
        caches[i] = malloc(sizeof(Cache));
    }
}
void construct_Cache(Cache* $, int id)
{
    $->id = id;
    $->lines = generate_cache_lines();
}
void construct_Caches()
{
    for (int i = 0; i < MAX_CORES; +i++)
    {
        construct_Cache(caches[i], i);
    }
}
void print_Cache(Cache* $)
{
    printf("Cache: %d\n", $->id);
    printf("Lines:\n");
    print_cache_lines($->lines);
}
void print_Caches()
{
    printf_blue_on();
    for (int i = 0; i < MAX_CORES; ++i)
    {
        print_Cache(caches[i]);
        printf("\n");
    }
    printf_color_reset();
}

///MT_message methods
MT_msg* generate_MT_msg(MT_msg* msg, int cache_id, bool request_type, int mem_addr, int data)
{
    msg->cache_id=cache_id;
    msg->MT_request_type=request_type;
    msg->mem_addr=mem_addr;
    msg->data=data;
    return msg;
}

///RAM methods
void fill_RAMdom()
{
    for (int i = 0; i < RAM_ENTRIES; ++i)
    {
        RAM[i] = rand()%MAX_RAM_RAND_VALUE;
    }
}
void fill_RAM()
{
    for (int i = 0; i < RAM_ENTRIES; ++i)
    {
        RAM[i] = (i+1)*1000;
    }
}
int get_RAM_block(int mem_addr)
{
    RAM_reads_counter++;
    return RAM[mem_addr];
}
void set_RAM_block(int tag, int data)
{
    RAM_writes_counter++;
    RAM[tag] = data;
}
void print_RAM()
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

///Interconnect methods
void initialize_Interconnect()
{
    interconnect = malloc(sizeof(Interconnect));
}
void construct_interconnect()
{
    interconnect->messages = malloc(MAX_CORES*sizeof(Queue));
    if(interconnect->messages == NULL)
    {
        fprintf(stderr, "construct_interconnect/interconnect->messages/Memory allocation failed\n");
        exit(1);
    }
}
void print_MT_msg(MT_msg* msg)
{
    if(msg==NULL)
    {
        printf("NULL\n");
    }
    else
    {
        printf("Id: %d, ", msg->cache_id);
        if(msg->MT_request_type){printf("Request: WRITE_REQUEST, ");}
        else{printf("Request: READ_REQUEST, ");}
        printf("Mem addr: %d, ", msg->mem_addr);
        printf("Data: %d\n", msg->data);
    }
}
void print_interconnect()
{
    printf("Interconnect messages:\n");
    Queue_msg_println(interconnect->messages);
    printf("\n");
}
bool is_data_in_cache(int cache_id, int tag)
{
    if(tag == caches[cache_id]->lines[tag%CACHE_ENTRIES]->tag)
    {
        if (caches[cache_id]->lines[tag%CACHE_ENTRIES]->state != 'I')
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}
Queue* get_remote_data_owners(int cache_id, int tag)
{
    Queue* remote_data_owners = malloc(sizeof(Queue));
    construct_queue(remote_data_owners);

    for (int i = 0; i < MAX_CORES; i++)
    {
        int* current_cache_id = malloc(sizeof(int));
        *current_cache_id = NOBODY;
        if(i != cache_id)
        {
            if(is_data_in_cache(i, tag))
            {
                *current_cache_id = i;
                enqueue(remote_data_owners, current_cache_id);
            }
        }
    }
    return remote_data_owners;
}
char* get_state_str(protocol_state state)
{
    if(state==Modified)
    {
        return "Modified";
    }
    else if(state==Owned)
    {
        return "Owned";
    }
    else if(state==Exclusive)
    {
        return "Exclusive";
    }
    else if(state==Shared)
    {
        return "Shared";
    }
    if(state==Modified)
    {
        return "Modified";
    }
    else if(state==Invalid)
    {
        return "Invalid";
    }
    return "None";
}
void read_exclusive_request()
{

}
void MESI_check_flush_to_memory(int cache_id, Cache_line* line, bool is_remote, protocol_state target_state)
{
    if(line->state!=Invalid)
    {
        const char* str_start_state;
        const char* str_end_state;
        const char* str_locality;

        str_start_state = get_state_str(line->state);
        str_end_state = get_state_str(target_state);
        if(target_state==Invalid)
        {
            invalidates_counter++;
        }
        if(is_remote)
        {
            str_locality = "remote";
        }
        else
        {
            str_locality = "local";
        }

        if(line->is_dirty)
        {
            //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
            sem_wait(&sem_debug_prints);
            printf_cyan_on();
            printf("Interconnect/MESI_check_flush_to_memory/$:%d T:%d/Implicit %s transition: %s->%s/Flush (%d) to RAM[%d]\n", cache_id, line->tag, str_locality, str_start_state, str_end_state, line->data, line->tag);
            printf_color_reset();
            sem_post(&sem_debug_prints);

            set_RAM_block(line->tag, line->data);
        }
        else
        {
            //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
            sem_wait(&sem_debug_prints);
            printf_yellow_on();
            printf("Interconnect/MESI_check_flush_to_memory/$:%d T:%d/Implicit %s transition: %s->%s\n", cache_id, line->tag, str_locality, str_start_state, str_end_state);
            printf_color_reset();
            sem_post(&sem_debug_prints);
        }
    }
}
void MOESI_check_flush_to_memory(int cache_id, Cache_line* line, bool is_remote, protocol_state target_state)
{
    if(line->state!=Invalid)
    {
        const char* str_start_state;
        const char* str_end_state;
        const char* str_locality;

        str_start_state = get_state_str(line->state);
        str_end_state = get_state_str(target_state);
        if(target_state==Invalid)
        {
            invalidates_counter++;
        }
        if(is_remote)
        {
            str_locality = "remote";
        }
        else
        {
            str_locality = "local";
        }

        if(line->is_dirty && target_state==Invalid && line->state!=Shared)
        {
            //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
            sem_wait(&sem_debug_prints);
            printf_cyan_on();
            printf("Interconnect/MOESI_check_flush_to_memory/$:%d T:%d/Implicit %s transition: %s->%s/Flush (%d) to RAM[%d]\n", cache_id, line->tag, str_locality, str_start_state, str_end_state, line->data, line->tag);
            printf_color_reset();
            sem_post(&sem_debug_prints);

            set_RAM_block(line->tag, line->data);
        }
        else
        {
            //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
            sem_wait(&sem_debug_prints);
            printf_yellow_on();
            printf("Interconnect/MOESI_check_flush_to_memory/$:%d T:%d/Implicit %s transition: %s->%s\n", cache_id, line->tag, str_locality, str_start_state, str_end_state);
            printf_color_reset();
            sem_post(&sem_debug_prints);
        }
    }
}
void MESI_invalidate(int cache_id, int tag)
{
    Cache_line* remote_line = get_cache_line(cache_id, tag%CACHE_ENTRIES);
    MESI_check_flush_to_memory(cache_id, remote_line, true, Invalid);
    remote_line->is_valid = false;
    remote_line->is_dirty = false;
    remote_line->state=Invalid;
}
void MOESI_invalidate(int cache_id, int tag)
{
    Cache_line* remote_line = get_cache_line(cache_id, tag%CACHE_ENTRIES);
    MOESI_check_flush_to_memory(cache_id, remote_line, true, Invalid);
    remote_line->is_valid = false;
    remote_line->is_dirty = false;
    remote_line->state=Invalid;
}
void MESI_broadcast_invalidate(Queue* remote_data_owners, MT_msg* msg)
{
    if(Queue_is_empty(remote_data_owners))
    {
        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_purple_on();
        printf("Interconnect/MESI_broadcast_invalidate/triggered by $:%d T:%d/No one else has the data\n", msg->cache_id, msg->mem_addr);
        printf_color_reset();
        sem_post(&sem_debug_prints);
    }
    else
    {
        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_purple_on();
        printf("Interconnect/MESI_broadcast_invalidate/triggered by $:%d T:%d\n", msg->cache_id, msg->mem_addr);
        printf_color_reset();
        sem_post(&sem_debug_prints);

        while(!Queue_is_empty(remote_data_owners))
        {
            int* cache_id = (int*)dequeue(remote_data_owners);
            if(*cache_id==NOBODY)
            {
                fprintf(stderr, "Interconnect/MESI_broadcast_invalidate/cache_id/Cast int from void* failed");
            }
            else
            {
                MESI_invalidate(*cache_id, msg->mem_addr);
            }
        }
    }
}
void MOESI_broadcast_invalidate(Queue* remote_data_owners, MT_msg* msg)
{
    if(Queue_is_empty(remote_data_owners))
    {
        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_purple_on();
        printf("Interconnect/MOESI_broadcast_invalidate/triggered by $:%d T:%d/No one else has the data\n", msg->cache_id, msg->mem_addr);
        printf_color_reset();
        sem_post(&sem_debug_prints);
    }
    else
    {
        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_purple_on();
        printf("Interconnect/MOESI_broadcast_invalidate/triggered by $:%d T:%d\n", msg->cache_id, msg->mem_addr);
        printf_color_reset();
        sem_post(&sem_debug_prints);

        while(!Queue_is_empty(remote_data_owners))
        {
            int* cache_id = (int*)dequeue(remote_data_owners);
            if(*cache_id==NOBODY)
            {
                fprintf(stderr, "Interconnect/MOESI_broadcast_invalidate/cache_id/Cast int from void* failed");
            }
            else
            {
                MOESI_invalidate(*cache_id, msg->mem_addr);
            }
        }
    }
}
void MESI_set_remote_share_states(Queue* remote_data_owners, MT_msg* msg)
{
    if(Queue_is_empty(remote_data_owners))
    {
        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_purple_on();
        printf("Interconnect/MESI_set_remote_share_states/triggered by $:%d T:%d/No one else has the data\n", msg->cache_id, msg->mem_addr);
        printf_color_reset();
        sem_post(&sem_debug_prints);
    }
    else
    {
        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_purple_on();
        printf("Interconnect/MESI_set_remote_share_states/triggered by $:%d T:%d\n", msg->cache_id, msg->mem_addr);
        printf_color_reset();
        sem_post(&sem_debug_prints);

        while(!Queue_is_empty(remote_data_owners))
        {
            int* cache_id = (int*)dequeue(remote_data_owners);
            if(*cache_id==NOBODY)
            {
                fprintf(stderr, "Interconnect/MESI_set_remote_share_states/cache_id/Cast int from void* failed");
            }
            else
            {
                Cache_line* remote_line = get_cache_line(*cache_id, msg->mem_addr%CACHE_ENTRIES);
                if(remote_line->state != Shared)
                {
                    MESI_check_flush_to_memory(*cache_id, remote_line, true, Shared);
                    remote_line->is_dirty=0;
                    remote_line->state = Shared;
                }
            }
        }
    }
}
void MOESI_set_remote_share_states(Queue* remote_data_owners, MT_msg* msg)
{
    if(Queue_is_empty(remote_data_owners))
    {
        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_purple_on();
        printf("Interconnect/MOESI_set_remote_share_states/triggered by $:%d T:%d/No one else has the data\n", msg->cache_id, msg->mem_addr);
        printf_color_reset();
        sem_post(&sem_debug_prints);
    }
    else
    {
        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_purple_on();
        printf("Interconnect/MOESI_set_remote_share_states/triggered by $:%d T:%d\n", msg->cache_id, msg->mem_addr);
        printf_color_reset();
        sem_post(&sem_debug_prints);

        while(!Queue_is_empty(remote_data_owners))
        {
            int* cache_id = (int*)dequeue(remote_data_owners);
            if(*cache_id==NOBODY)
            {
                fprintf(stderr, "Interconnect/MOESI_set_remote_share_states/cache_id/Cast int from void* failed");
            }
            else
            {
                Cache_line* remote_line = get_cache_line(*cache_id, msg->mem_addr%CACHE_ENTRIES);
                if(remote_line->state != Shared)
                {
                    if(remote_line->state==Exclusive)
                    {
                        MOESI_check_flush_to_memory(*cache_id, remote_line, true, Shared);
                        //remote_line->is_dirty=0;
                        remote_line->state = Shared;
                    }
                    else
                    {
                        MOESI_check_flush_to_memory(*cache_id, remote_line, true, Owned);
                        //remote_line->is_dirty=1;
                        remote_line->state = Owned;
                    }
                }
            }
        }
    }
}

///General functions
bool end_total_execution()
{
    bool end_total_execution = true;
    for (int i = 0; i < MAX_CORES; i++)
    {
        if(!PEs[i]->process_ended)
        {
            end_total_execution = false;
            return end_total_execution;
        }
    }
    return end_total_execution;
}

///Log
void initialize_MT_history_queue()
{
    MT_history = malloc(sizeof(Queue));
}
void add_increments_counter()
{
    sem_wait(&sem_increments_counter);
    increments_counter++;
    sem_post(&sem_increments_counter);
}
void print_log()
{
    //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
    printf("PEs: %d\n", MAX_CORES);
    printf("Instructions per core: %d\n", MAX_INSTRUCTIONS);
    printf("Blocks per cache: %d\n", CACHE_ENTRIES);
    printf("Blocks per RAM: %d\n", RAM_ENTRIES);
    printf("Increments counter: %d\n", increments_counter);
    printf("Interconnect read requests counter: %d\n", interconnect_read_requests_counter);
    printf("Interconnect write requests counter: %d\n", interconnect_write_requests_counter);
    printf("RAM reads counter: %d\n", RAM_reads_counter);
    printf("RAM writes counter: %d\n", RAM_writes_counter);
    //printf("Cache hits: %d\n", cache_hit_counter);
    //printf("Cache misses: %d\n", cache_miss_counter);
    printf("Cache data shares counter: %d\n", cache_data_shares_counter);
    printf("Invalidates counter: %d\n", invalidates_counter);
    printf("\n");
    printf("MT History:\n");
    Queue_msg_println(MT_history);
}

/// Protocols
void MESI(MT_msg* msg, bool local_PE_has_the_data, Queue* remote_data_owners, Cache_line* current_editing_line)
{
    if(msg->MT_request_type==READ_REQUEST && !local_PE_has_the_data && Queue_is_empty(remote_data_owners))///000 I-E
    {
        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_yellow_on();
        printf("Interconnect/MESI/$:%d T:%d/local transition: Invalid->Exclusive\n", msg->cache_id, msg->mem_addr);
        printf_color_reset();
        sem_post(&sem_debug_prints);

        MESI_check_flush_to_memory(msg->cache_id, current_editing_line, false, Invalid);
        set_cache_line(msg->cache_id, msg->mem_addr%CACHE_ENTRIES, msg->mem_addr, true, false, Exclusive, get_RAM_block(msg->mem_addr));
    }
    else if((msg->MT_request_type == READ_REQUEST) && !local_PE_has_the_data && !Queue_is_empty(remote_data_owners))///001 I->S
    {
        // Pass data from caches
        int sharing_cache_id = *((int*)remote_data_owners->first->data);
        int data_from_remote_cache = caches[sharing_cache_id]->lines[msg->mem_addr%CACHE_ENTRIES]->data;

        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        cache_data_shares_counter++;

        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_purple_on();
        printf("Interconnect/MESI/$:%d T:%d/Data (%d) move between caches: %d -> %d\n", msg->cache_id, msg->mem_addr, data_from_remote_cache, sharing_cache_id, msg->cache_id);
        printf_color_reset();
        sem_post(&sem_debug_prints);

        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_yellow_on();
        printf("Interconnect/MESI/$:%d T:%d/local transition: Invalid->Shared\n", msg->cache_id, msg->mem_addr);
        printf_color_reset();
        sem_post(&sem_debug_prints);

        MESI_set_remote_share_states(remote_data_owners, msg);
        MESI_check_flush_to_memory(msg->cache_id, current_editing_line, false, Invalid);
        set_cache_line(msg->cache_id, msg->mem_addr%CACHE_ENTRIES, msg->mem_addr, true, false, Shared, data_from_remote_cache);
    }
    else if(msg->MT_request_type==READ_REQUEST && local_PE_has_the_data && Queue_is_empty(remote_data_owners))///010 M->M o E->E o S->S
    {
        //Is possible to get an exclusive Shared by remote eviction

        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_yellow_on();
        printf("Interconnect/MESI/$:%d T:%d/local transition: %s->Modified\n", msg->cache_id, msg->mem_addr, get_state_str(current_editing_line->state));
        printf_color_reset();
        sem_post(&sem_debug_prints);
    }
    else if(msg->MT_request_type==READ_REQUEST && local_PE_has_the_data && !Queue_is_empty(remote_data_owners))///011 S->S
    {
        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_yellow_on();
        printf("Interconnect/MESI/$:%d T:%d/local transition: %s->%s\n", msg->cache_id, msg->mem_addr, get_state_str(current_editing_line->state), get_state_str(current_editing_line->state));
        printf_color_reset();
        sem_post(&sem_debug_prints);
    }
    else if(msg->MT_request_type==WRITE_REQUEST && !local_PE_has_the_data)///100 y 101 I->M
    {
        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_yellow_on();
        printf("Interconnect/MESI/$:%d T:%d/local transition: Invalid->Modified\n", msg->cache_id, msg->mem_addr);
        printf_color_reset();
        sem_post(&sem_debug_prints);

        read_exclusive_request();
        MESI_broadcast_invalidate(remote_data_owners, msg);
        MESI_check_flush_to_memory(msg->cache_id, current_editing_line, false, Invalid);
        set_cache_line(msg->cache_id, msg->mem_addr%CACHE_ENTRIES, msg->mem_addr, true, true, Modified, msg->data);
    }
    else if(msg->MT_request_type==WRITE_REQUEST && local_PE_has_the_data && Queue_is_empty(remote_data_owners))///110 E->M o M->M o S->M
    {
        // Is possible to get an exclusive Shared by remote eviction

        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_yellow_on();
        printf("Interconnect/MESI/$:%d T:%d/local transition: %s->Modified\n", msg->cache_id, msg->mem_addr, get_state_str(current_editing_line->state));
        printf_color_reset();
        sem_post(&sem_debug_prints);

        read_exclusive_request();
        set_cache_line(msg->cache_id, msg->mem_addr%CACHE_ENTRIES, msg->mem_addr, true, true, Modified, msg->data);
    }
    else if(msg->MT_request_type==WRITE_REQUEST && local_PE_has_the_data && !Queue_is_empty(remote_data_owners))///111 S->M
    {
        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_yellow_on();
        printf("Interconnect/MESI/$:%d T:%d/local transition: Shared->Modified\n", msg->cache_id, msg->mem_addr);
        printf_color_reset();
        sem_post(&sem_debug_prints);

        read_exclusive_request();
        MESI_broadcast_invalidate(remote_data_owners, msg);
        set_cache_line(msg->cache_id, msg->mem_addr%CACHE_ENTRIES, msg->mem_addr, true, true, Modified, msg->data);
    }
}
void MESI_viejo(MT_msg* msg, bool local_PE_has_the_data, Queue* remote_data_owners, Cache_line* current_editing_line)
{
//    if((msg->MT_request_type == WRITE_REQUEST) && !local_PE_has_the_data)/// I->M
//    {
//        sem_wait(&sem_debug_prints);
//        printf_yellow_on();
//        printf("Interconnect/$:%d T:%d/local transition: Invalid->Modified\n", msg->cache_id, msg->mem_addr);
//        printf_color_reset();
//        sem_post(&sem_debug_prints);
//
//        read_exclusive_request();
//        broadcast_invalidate(remote_data_owners, msg);
//        check_flush_to_memory(msg->cache_id, current_editing_line, false, Invalid);
//        set_cache_line(msg->cache_id, msg->mem_addr%CACHE_ENTRIES, msg->mem_addr, true, true, Modified, msg->data);
//    }
//    else if((msg->MT_request_type == READ_REQUEST) && !local_PE_has_the_data && Queue_is_empty(remote_data_owners))/// I-E
//    {
//        sem_wait(&sem_debug_prints);
//        printf_yellow_on();
//        printf("Interconnect/$:%d T:%d/local transition: Invalid->Exclusive\n", msg->cache_id, msg->mem_addr);
//        printf_color_reset();
//        sem_post(&sem_debug_prints);
//
//        check_flush_to_memory(msg->cache_id, current_editing_line, false, Invalid);
//        set_cache_line(msg->cache_id, msg->mem_addr%CACHE_ENTRIES, msg->mem_addr, true, false, Exclusive, get_RAM_block(msg->mem_addr));
//    }
//    else if((msg->MT_request_type == READ_REQUEST) && !local_PE_has_the_data && !Queue_is_empty(remote_data_owners))/// I->S
//    {
//        sem_wait(&sem_debug_prints);
//        printf_yellow_on();
//        printf("Interconnect/$:%d T:%d/local transition: Invalid->Shared\n", msg->cache_id, msg->mem_addr);
//        printf_color_reset();
//        sem_post(&sem_debug_prints);
//
//        // Pass data from caches
//        int sharing_cache_id = *((int*)remote_data_owners->first->data);
//        int data_from_remote_cache = caches[sharing_cache_id]->lines[msg->mem_addr%CACHE_ENTRIES]->data;
//        cache_data_shares_counter++;
//
//        sem_wait(&sem_debug_prints);
//        printf_yellow_on();
//        printf("Interconnect/$:%d T:%d/Data move between caches: self <- %d\n", msg->cache_id, msg->mem_addr, sharing_cache_id);
//        printf_color_reset();
//        sem_post(&sem_debug_prints);
//
//        set_remote_share_states(remote_data_owners, msg);
//        set_cache_line(msg->cache_id, msg->mem_addr%CACHE_ENTRIES, msg->mem_addr, true, false, Shared, data_from_remote_cache);
//    }
//    else if((msg->MT_request_type == WRITE_REQUEST) && local_PE_has_the_data && !Queue_is_empty(remote_data_owners))/// S->M
//    {
//        sem_wait(&sem_debug_prints);
//        printf_yellow_on();
//        printf("Interconnect/$:%d T:%d/local transition: Shared->Modified\n", msg->cache_id, msg->mem_addr);
//        printf_color_reset();
//        sem_post(&sem_debug_prints);
//
//        read_exclusive_request();
//        broadcast_invalidate(remote_data_owners, msg);
//        set_cache_line(msg->cache_id, msg->mem_addr%CACHE_ENTRIES, msg->mem_addr, true, true, Modified, msg->data);
//    }
//    else if((msg->MT_request_type == WRITE_REQUEST) && local_PE_has_the_data && Queue_is_empty(remote_data_owners))/// E->M
//    {
//        sem_wait(&sem_debug_prints);
//        printf_yellow_on();
//        printf("Interconnect/$:%d T:%d/local transition: Exclusive->Modified\n", msg->cache_id, msg->mem_addr);
//        printf_color_reset();
//        sem_post(&sem_debug_prints);
//
//        read_exclusive_request();
//        set_cache_line(msg->cache_id, msg->mem_addr%CACHE_ENTRIES, msg->mem_addr, true, true, Modified, msg->data);
//    }
}
void MOESI(MT_msg* msg, bool local_PE_has_the_data, Queue* remote_data_owners, Cache_line* current_editing_line)
{
    if(msg->MT_request_type==READ_REQUEST && !local_PE_has_the_data && Queue_is_empty(remote_data_owners))///000 I-E !
    {
        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_yellow_on();
        printf("Interconnect/MOESI/$:%d T:%d/local transition: Invalid->Exclusive\n", msg->cache_id, msg->mem_addr);
        printf_color_reset();
        sem_post(&sem_debug_prints);

        MOESI_check_flush_to_memory(msg->cache_id, current_editing_line, false, Invalid);
        set_cache_line(msg->cache_id, msg->mem_addr%CACHE_ENTRIES, msg->mem_addr, true, false, Exclusive, get_RAM_block(msg->mem_addr));
    }
    else if((msg->MT_request_type == READ_REQUEST) && !local_PE_has_the_data && !Queue_is_empty(remote_data_owners))///001 I->S !
    {
        // Pass data from caches
        int sharing_cache_id = *((int*)remote_data_owners->first->data);
        int data_from_remote_cache = caches[sharing_cache_id]->lines[msg->mem_addr%CACHE_ENTRIES]->data;
        bool inherit_dirty_bit = caches[sharing_cache_id]->lines[msg->mem_addr%CACHE_ENTRIES]->is_dirty;

        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        cache_data_shares_counter++;

        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_purple_on();
        printf("Interconnect/MOESI/$:%d T:%d/Data (%d) move between caches: %d -> %d\n", msg->cache_id, msg->mem_addr, data_from_remote_cache, sharing_cache_id, msg->cache_id);
        printf_color_reset();
        sem_post(&sem_debug_prints);

        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_yellow_on();
        printf("Interconnect/MOESI/$:%d T:%d/local transition: Invalid->Shared\n", msg->cache_id, msg->mem_addr);
        printf_color_reset();
        sem_post(&sem_debug_prints);

        MOESI_set_remote_share_states(remote_data_owners, msg);
        MOESI_check_flush_to_memory(msg->cache_id, current_editing_line, false, Invalid);
        set_cache_line(msg->cache_id, msg->mem_addr%CACHE_ENTRIES, msg->mem_addr, true, inherit_dirty_bit, Shared, data_from_remote_cache);
    }
    else if(msg->MT_request_type==READ_REQUEST && local_PE_has_the_data && Queue_is_empty(remote_data_owners))///010 M->M o E->E o S->S !
    {
        //Is possible to get an exclusive Shared by remote eviction

        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_yellow_on();
        printf("Interconnect/MOESI/$:%d T:%d/local transition: %s->Modified\n", msg->cache_id, msg->mem_addr, get_state_str(current_editing_line->state));
        printf_color_reset();
        sem_post(&sem_debug_prints);
    }
    else if(msg->MT_request_type==READ_REQUEST && local_PE_has_the_data && !Queue_is_empty(remote_data_owners))///011 S->S o O->O!
    {
        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_yellow_on();
        printf("Interconnect/MOESI/$:%d T:%d/local transition: %s->%s\n", msg->cache_id, msg->mem_addr, get_state_str(current_editing_line->state), get_state_str(current_editing_line->state));
        printf_color_reset();
        sem_post(&sem_debug_prints);
    }
    else if(msg->MT_request_type==WRITE_REQUEST && !local_PE_has_the_data)///100 y 101 I->M
    {
        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_yellow_on();
        printf("Interconnect/MOESI/$:%d T:%d/local transition: Invalid->Modified\n", msg->cache_id, msg->mem_addr);
        printf_color_reset();
        sem_post(&sem_debug_prints);

        read_exclusive_request();
        MOESI_broadcast_invalidate(remote_data_owners, msg);
        MOESI_check_flush_to_memory(msg->cache_id, current_editing_line, false, Invalid);
        set_cache_line(msg->cache_id, msg->mem_addr%CACHE_ENTRIES, msg->mem_addr, true, true, Modified, msg->data);
    }
    else if(msg->MT_request_type==WRITE_REQUEST && local_PE_has_the_data && Queue_is_empty(remote_data_owners))///110 E->M o M->M o S->M o O->M !
    {
        // Is possible to get an exclusive Shared by remote eviction

        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_yellow_on();
        printf("Interconnect/MOESI/$:%d T:%d/local transition: %s->Modified\n", msg->cache_id, msg->mem_addr, get_state_str(current_editing_line->state));
        printf_color_reset();
        sem_post(&sem_debug_prints);

        read_exclusive_request();
        set_cache_line(msg->cache_id, msg->mem_addr%CACHE_ENTRIES, msg->mem_addr, true, true, Modified, msg->data);
    }
    else if(msg->MT_request_type==WRITE_REQUEST && local_PE_has_the_data && !Queue_is_empty(remote_data_owners))///111 S->M o O->M !
    {
        //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
        sem_wait(&sem_debug_prints);
        printf_yellow_on();
        printf("Interconnect/MOESI/$:%d T:%d/local transition: %s->Modified\n", msg->cache_id, msg->mem_addr, get_state_str(current_editing_line->state));
        printf_color_reset();
        sem_post(&sem_debug_prints);

        read_exclusive_request();
        MOESI_broadcast_invalidate(remote_data_owners, msg);
        set_cache_line(msg->cache_id, msg->mem_addr%CACHE_ENTRIES, msg->mem_addr, true, true, Modified, msg->data);
    }
}


///Threading
void PE_MT(int id, bool request_type, int mem_addr, int data)
{
    PE* pe = PEs[id];
    pe->MT_done=false;

    MT_msg* msg = malloc(sizeof(MT_msg));
    generate_MT_msg(msg, id, request_type, mem_addr, data);

    //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
    sem_wait(&sem_debug_prints);
    printf_green_on();
    printf("PE: %d send\n{\n\t", id);
    print_MT_msg(msg);
    printf("}\n");
    printf_color_reset();
    sem_post(&sem_debug_prints);

    sem_wait(&sem_edit_interconnect_queue);
    enqueue(interconnect->messages, msg);
    sem_post(&sem_edit_interconnect_queue);

    while(!pe->MT_done)
    {
        ///Wait
    }

    //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
    sem_wait(&sem_debug_prints);
    printf("PE: %d receive response\n", id);/////////////////////////////////////////////////////////////////////
    sem_post(&sem_debug_prints);
}
void PE_execute(int id)
{
    PE *pe = PEs[id];
    Instruction* instruction = dequeue(pe->instructions);

    //Borrar esta vrga todo ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    pe->reg = pe->reg+1;
    //

    switch (instruction->type)
    {
        case Read:
            PE_MT(id, READ_REQUEST, instruction->mem_addr,  pe->reg);
            break;

        case Write:
            PE_MT(id, WRITE_REQUEST, instruction->mem_addr,  pe->reg);
            break;

        case Increment:
            //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
            sem_wait(&sem_debug_prints);
            printf_green_on();
            printf("PE: %d. Increment executed: Reg %d->%d\n", id, pe->reg, pe->reg+1);
            printf_color_reset();
            sem_post(&sem_debug_prints);

            pe->reg = pe->reg+1;

            //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
            add_increments_counter();
            break;

        default:
            break;
    }
}
void* PE_thread_f()
{
    ///Critical write
    sem_wait(&sem_thread_PE_id);
    int id = global_thread_PE_id_counter++;
    sem_post(&sem_thread_PE_id);

    PE* pe = PEs[id];

    while (!Queue_is_empty(pe->instructions))
    {
        PE_execute(id);
    }

    //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
    sem_wait(&sem_debug_prints);
    printf_green_on();
    printf("PE:%d END\n\n", id);
    printf_color_reset();
    sem_post(&sem_debug_prints);

    PEs[id]->process_ended=true;
}
void* interconnect_thread_f()
{
    while(!end_total_execution())
    {
        if(!Queue_is_empty(interconnect->messages))
        {
            sem_wait(&sem_edit_interconnect_queue);
            MT_msg* msg = (MT_msg*)dequeue(interconnect->messages);
            sem_post(&sem_edit_interconnect_queue);

            MT_msg* msg_to_history = malloc(sizeof(MT_msg));
            *msg_to_history = *msg;
            enqueue( MT_history,msg_to_history);

            //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
            sem_wait(&sem_debug_prints);
            printf("Interconnect receive (%d)\n{\n\t", interconnect_read_requests_counter+interconnect_write_requests_counter);
            print_MT_msg(msg);
            printf("}\n");
            sem_post(&sem_debug_prints);

            bool local_PE_has_the_data = is_data_in_cache(msg->cache_id, msg->mem_addr);
            Queue* remote_data_owners = get_remote_data_owners(msg->cache_id, msg->mem_addr);
            Cache_line* current_editing_line = get_cache_line(msg->cache_id, msg->mem_addr%CACHE_ENTRIES);

            if(msg->MT_request_type == READ_REQUEST)
            {
                //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
                interconnect_read_requests_counter++;
            }
            else{interconnect_write_requests_counter++;}

            if(is_MESI)
            {
                MESI(msg, local_PE_has_the_data, remote_data_owners, current_editing_line);
            }
            else
            {
                MOESI(msg, local_PE_has_the_data, remote_data_owners, current_editing_line);
            }

            //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
            sem_wait(&sem_debug_prints);
            print_Caches();
            print_RAM();
            print_log();
            sem_post(&sem_debug_prints);

            /// Release PE to continue execution
            PEs[msg->cache_id]->MT_done=true;
        }
    }
    //todo perfilacion /////////////////////////////////////////////////////////////////////////////////////////////
    sem_wait(&sem_debug_prints);
    printf_green("Interconnect ENDED\n\n");
    sem_post(&sem_debug_prints);
}
void initialize_semaphores()
{
    sem_init(&sem_debug_prints, 0, 1);
    sem_init(&sem_thread_PE_id, 0, 1);
    sem_init(&sem_edit_interconnect_queue, 0, 1);
    sem_init(&sem_read_exclusive_request, 0, 1);
    sem_init(&sem_increments_counter,0,1);
}
void start_threads()
{
    pthread_create(&interconnect_thread, NULL, interconnect_thread_f, NULL);
    for (int i = 0; i < MAX_CORES; i++)
    {
        pthread_create(&PE_threads[i], NULL, PE_thread_f, NULL);
    }
}
void join_threads()
{
    for (int i = 0; i < MAX_CORES; i++)
    {
        pthread_join(PE_threads[i], NULL);
    }
    pthread_join(interconnect_thread, NULL);
}


///Main
int main()
{
    is_MESI = true;//MESIRVE
    //is_MESI = false;//MESIENTO

    /// --- Build structs ---
    /// Build PEs
    initialize_PEs();
    //construct_specific_PEs();
    construct_PEs();
    print_PEs();

    /// Build caches
    initialize_Caches();
    construct_Caches();
    print_Caches();

    /// Build interconnect
    initialize_Interconnect();
    construct_interconnect();
    print_interconnect();

    /// Build RAM
    fill_RAM();
    print_RAM();

    /// Build Log
    initialize_MT_history_queue();

    printf("\n\n////////////////////////////////////////////////////////\n\n");
    printf("\n\n////////////////////////////////////////////////////////\n\n");
    printf("\n\n////////////////////////////////////////////////////////\n\n");

    initialize_semaphores();
    start_threads();
    join_threads();

    print_PEs();
    print_Caches();
    print_interconnect();
    print_RAM();

    printf("\n");
    printf("End!\n");

    printf("\n");
    print_log();
}