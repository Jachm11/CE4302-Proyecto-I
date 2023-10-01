#ifndef PROPUESTA_2_STRUCTS_H
#define PROPUESTA_2_STRUCTS_H


#include <stdint.h>
#include <stdbool.h>



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


///General Structs
typedef struct Instruction_t
{
    instruction_type type;
    int mem_addr;
} Instruction;

typedef struct PE_t
{
    int id;
    Queue* instructions;
    int reg;
    bool MT_done;
    bool process_ended;
} PE;

typedef struct cache_line_t
{
    int tag;
    bool is_valid;
    bool is_dirty;
    protocol_state state;
    int data;

} Cache_line;

typedef struct cache_t
{
    int id;
    Cache_line** lines;
} Cache;


///Messages structs
typedef struct MT_msg_t
{
    int cache_id;
    bool MT_request_type;
    int mem_addr;
    int data;
} MT_msg;


typedef struct interconnect_t
{
    Queue* messages;
} Interconnect;







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