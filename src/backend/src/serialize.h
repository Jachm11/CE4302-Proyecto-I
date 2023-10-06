//
// Created by sebastian on 25/09/23.
//

#ifndef PROYECTO_1_SERVER_H
#define PROYECTO_1_SERVER_H

#include "cJSON.h"
#include "structs.h"
#include "coherence_protocol_constants.h"

char* toString(int value);

cJSON* start_events_recolection();
cJSON* finish_events_recolection();

void start_transaction(int count, Instruction instruction, int core_id);
void finish_transaction();


void add_event_move(int start_id, int end_id);
void add_event_edit(int element_id, int tag, int data, char* target_state);

void add_initial_cores_state(PE** PEs);

void add_report(int cores, int instructions_per_core, 
                int blocks_per_cache, 
                int blocks_per_ram,
                int increments_counter,
                int read_requests_count,
                int write_requests_count, 
                int ram_reads_count, 
                int ram_writes_count, 
                int read_response, 
                int invalidates_count
);


#endif //PROYECTO_1_SERVER_H