
#include "cJSON.h"
#include "stdio.h"
#include <stdlib.h>
#include "structs.h"
cJSON* root; 
cJSON* current_events;
cJSON* current_transaction;
cJSON* transactions;

void start_events_recolection(){
    root = cJSON_CreateObject();
    transactions = cJSON_CreateArray();
}

char* toString(int value){
    char* string_value = malloc(20 * sizeof(char));
    sprintf(string_value, "%d", value);
    return string_value;
}

char* get_instruction_type(int type){

    switch (type) {

        case 87:
            return "WRITE";
        case 82: 
            return "READ";
        case 73:
            return "INC";
        default:
            return "Invalid instruction";
    }

}


void start_transaction(int count, Instruction instruction, int core_id){
    current_transaction = cJSON_CreateObject();
    current_events = cJSON_CreateArray();
    char* instruction_type = get_instruction_type(instruction.type);
    cJSON_AddNumberToObject(current_transaction, "count", count);
    cJSON_AddNumberToObject(current_transaction, "core", core_id);
    cJSON_AddNumberToObject(current_transaction, "mem_address", instruction.mem_addr);
    cJSON_AddStringToObject(current_transaction, "type", instruction_type);
}

void add_event_move(int start_id, int end_id){

    cJSON* event = cJSON_CreateObject();
    cJSON_AddStringToObject(event, "event_type", "move");
    cJSON_AddNumberToObject(event, "start_id", start_id);
    cJSON_AddNumberToObject(event, "end_id", end_id);
    cJSON_AddItemToArray(current_events, event);
    
}

void add_event_edit(int element_id, int tag, int data, char* target_state){
    cJSON* event = cJSON_CreateObject();
    cJSON_AddStringToObject(event, "event_type", "edit");
    cJSON_AddNumberToObject(event, "element_id", element_id);
    cJSON_AddNumberToObject(event, "tag", tag);
    cJSON_AddNumberToObject(event, "data", data);
    cJSON_AddStringToObject(event, "target_state", target_state);   
    cJSON_AddItemToArray(current_events, event);
}

void add_report(int cores,
                int instructions_per_core, 
                int blocks_per_cache, 
                int blocks_per_ram,
                int increments_counter,
                int read_requests_count,
                int write_requests_count, 
                int ram_reads_count, 
                int ram_writes_count, 
                int read_response, 
                int invalidates_count
    ){

    cJSON* report = cJSON_CreateObject();
    cJSON_AddNumberToObject(report, "cores", cores);
    cJSON_AddNumberToObject(report, "instructions_per_core", instructions_per_core);
    cJSON_AddNumberToObject(report, "blocks_per_cache", blocks_per_cache);
    cJSON_AddNumberToObject(report, "blocks_per_ram", blocks_per_ram);
    cJSON_AddNumberToObject(report, "increments_counter",increments_counter);
    cJSON_AddNumberToObject(report, "read_requests_count", read_requests_count);
    cJSON_AddNumberToObject(report, "write_requests_count", write_requests_count);
    cJSON_AddNumberToObject(report, "ram_reads_counter", ram_reads_count);
    cJSON_AddNumberToObject(report, "read_responses", ram_writes_count);
    cJSON_AddNumberToObject(report, "read_responses", read_response);
    cJSON_AddNumberToObject(report, "invalidates_counter", invalidates_count);
    cJSON_AddItemToObject(root, "report", report);

}


void add_initial_cores_state(PE** PEs){

    cJSON* cores = cJSON_CreateArray();
    for (int i = 0; i < 3; i++)
    {
        cJSON* core = cJSON_CreateObject();
        PE* current_pe = PEs[i];
        cJSON_AddNumberToObject(core, "id", current_pe->id);
        cJSON* instructions = cJSON_CreateArray();
        Queue* current_instructions = current_pe->instructions;
        if(Queue_is_empty(current_instructions))
        {   
            cJSON_AddItemToObject(core,"instructions", instructions);
            cJSON_AddItemToArray(cores, cores);
            continue;
        }
        else
        {
            SLL_node* piv;
            piv=current_instructions->first;
            while(piv != NULL)
            {
                Instruction* data = (Instruction*)piv->data;
                cJSON* instruction = cJSON_CreateObject();
                cJSON_AddNumberToObject(instruction, "mem_addr", data->mem_addr);
                char* type = get_instruction_type(data->type);
                cJSON_AddStringToObject(instruction, "type", type);
                piv = piv->next;
                cJSON_AddItemToArray(instructions, instruction);
            }
            cJSON_AddItemToObject(core,"instructions", instructions);
            cJSON_AddItemToArray(cores, core);
    }
    }
    cJSON_AddItemToObject(root, "cores", cores);
}

void finish_transaction(){
    cJSON_AddItemToObject(current_transaction, "events", current_events);
    cJSON_AddItemToArray(transactions, current_transaction);
}

cJSON* finish_events_recolection(){
    cJSON_AddItemToObject(root, "transactions", transactions);
    return root;
}

void add_log(cJSON* root){
    
}
