#include "structs.h"



///MT_message methods
MT_msg* generate_MT_msg(MT_msg* msg, int cache_id, bool request_type, int mem_addr, int data, Instruction instruction)
{
    msg->cache_id=cache_id;
    msg->MT_request_type=request_type;
    msg->mem_addr=mem_addr;
    msg->data=data;
    msg->instruction = instruction;
    return msg;
}
