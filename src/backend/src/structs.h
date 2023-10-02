#ifndef PROPUESTA_2_STRUCTS_H
#define PROPUESTA_2_STRUCTS_H


#include <stdint.h>
#include <stdbool.h>
#include "cJSON.h"

///Enums
typedef enum protocol_state_t
{
    Modified = 'M',
    Owned = 'O',
    Exclusive = 'E',
    Shared = 'S',
    Invalid = 'I'
} protocol_state;

typedef enum instruction_type_t
{
    Read = 'R',
    Write = 'W',
    Increment = 'I',
    Nop = 'N'
} instruction_type;
unsigned char generate_random_type_instruction();


///Data structures
typedef struct SLL_node_t
{
    void* data;
    struct SLL_node* next;
} SLL_node;
typedef struct Queue_t
{
    SLL_node* first;
} Queue;
typedef struct LL_t
{
    SLL_node* first;
} LL;

// Queue methods
Queue* construct_queue(Queue* queue);
int Queue_is_empty(Queue* queue);
void Queue_insert_first(Queue* queue, void* data);
void enqueue(Queue* queue, void* new_data);
void* dequeue(Queue* queue);
int Queue_get_length(Queue* queue);
void Queue_destroy(Queue* queue);
void Queue_print_node_as_int(SLL_node* node);
void Queue_print_node_as_instruction(SLL_node* node);
void Queue_print_node_as_MT_msg(SLL_node* node);
void Queue_int_println(Queue* queue);
void Queue_instruction_println(Queue* queue);
void Queue_msg_println(Queue* queue);


/// --- General Structs ---


/// Instruction
typedef struct Instruction_t
{
    instruction_type type;
    int mem_addr;
} Instruction;

// Instruction methods
void print_instruction(Instruction instruction);
Instruction* construct_instruction(Instruction* instruction, unsigned char type, int mem_addr);
Queue* generate_random_instructions();


/// PE
typedef struct PE_t
{
    int id;
    Queue* instructions;
    int reg;
    bool MT_done;
    bool process_ended;
} PE;

// PE methods
void initialize_PEs(PE** PEs);
void construct_PE(PE* pe, int id);
void construct_PEs(PE** PEs);
void print_PE(PE* pe);
void print_PEs(PE** PEs);


/// Cache line
typedef struct cache_line_t
{
    int tag;
    bool is_valid;
    bool is_dirty;
    protocol_state state;
    int data;

} Cache_line;

// Cache line methods
void construct_cache_line(Cache_line* line, int tag, bool is_valid, bool is_dirty, protocol_state state, int data);
Cache_line** generate_cache_lines();
void print_cache_line(Cache_line* line);
void print_cache_lines(Cache_line** lines);


/// Cache
typedef struct cache_t
{
    int id;
    Cache_line** lines;
} Cache;

// Cache methods
void initialize_Caches(Cache** caches);
void construct_Cache(Cache* $, int id);
void construct_Caches(Cache** caches);
Cache_line* get_cache_line(Cache** caches, int cache_id, int line_addr);
void set_cache_line(Cache** caches, int cache_id, int line_addr, int tag, bool is_valid, bool is_dirty, protocol_state state, int data);
void print_Cache(Cache* $);
void print_Caches(Cache** caches);


///Messages structs
typedef struct MT_msg_t
{
    int cache_id;
    bool MT_request_type;
    int mem_addr;
    int data;
} MT_msg;

// Messages structs methods
MT_msg* generate_MT_msg(MT_msg* msg, int cache_id, bool request_type, int mem_addr, int data);


///RAM methods
void fill_RAMdom(int* RAM);
void fill_RAM(int* RAM);
int get_RAM_block(int* RAM, int mem_addr, int* RAM_reads_counter);
void set_RAM_block(int* RAM, int tag, int data, int* RAM_writes_counter);
void print_RAM(int* RAM);


typedef struct interconnect_t
{
    Queue* messages;
} Interconnect;


/// Printer methods
void printf_red(const char* str);
void printf_red_on();
void printf_yellow(const char* str);
void printf_yellow_on();
void printf_green(const char* str);
void printf_green_on();
void printf_blue(const char* str);
void printf_blue_on();
void printf_cyan(const char* str);
void printf_cyan_on();
void printf_purple(const char* str);
void printf_purple_on();
void printf_white(const char* str);
void printf_white_on();
void printf_black(const char* str);
void printf_black_on();
void printf_color_reset();


// Execution
void start_execution(bool is_MESI_, cJSON* execution);



//  typedef struct Log_frame_t
//  {
//      int id;
//      Request             = Core tal quiere hacer tal cosa a tal linea incluidos incrementos
//      Transiciones[]      = $ tal, linea tal, pasa de tal a tal tanto locale scomo remotos sin distincion
//      Escrituras_a_RAM[]  = Si tal escribe a RAM todo pueden ser mas de 1!
//      Queue* eventos      = invalidate
//      PEs[] y dentro su propia cache
//      RAM
//  } Log_frame;


/*
 * todo id !
 * int id
 */

/*
 * todo Msg !
 * int $, bool is_write, int tag, int data
 */

/*
 * todo Transiciones !
 * int $, int tag, char source_state, char target_state, int (-1 es nada, 0 es leer, 1 es escribir), int data
 * int $, int tag, char source_state, char target_state, int (-1 es nada, 0 es leer, 1 es escribir), int data
 * .
 * .
 * .
 * int $, int tag, char source_state, char target_state, int (-1 es nada, 0 es leer, 1 es escribir), int data
 */

/*
 * todo Eventos de RAM
 * Lectura int $, int tag
 *
 * Escrituras
 * int $, int data
 * .
 * .
 * .
 * int $, int data
 */

/*
 * todo Broadcast Invalidate ?
 * bool done
 */

/*
 * todo Counters
 * Increments counter: 1
 * Interconnect read requests counter: 0
 * Interconnect write requests counter: 1
 * RAM reads counter: 0
 * RAM writes counter: 0
 * Cache data shares counter: 0
 * Invalidates counter: 0
 */


//typedef struct anim_event_move
//{
//    // int event_type {0 mem, 1 sharing_data }
//    // int start_id (puede ser cache (de 0 a mas) o RAM = -1)
//    // int end_id (puede ser cache (de 0 a mas) o RAM = -1)
//};
//typedef struct anim_event_edit
//{
//    // int editing_element_id { -1 si es RAM, 0 o mas si es cache}
//    // int tag
//    // int data
//    // char target_state (F si es RAM = 'F')
//};
//typedef struct transition
//{
//    // anim_event_move
//    // LL anim_event_edit
//};
//typedef struct transaction
//{
//    // LL transition
//};
//typedef struct log
//{
//    // LL transactions
//};



// Dato se pasa del PE a la cache (anim event move)
// State-> Modificar (anim event edit)


// Broadcast invalidate (2 transiciones)
    // PE x manda invalidate al bus (anim event move)
    // State-> Invalidate (anim event edit)

    // PE y manda invalidate al bus (anim event move)
    // State-> Invalidate (anim event edit)



/// Escribir Log

#endif //PROPUESTA_2_STRUCTS_H