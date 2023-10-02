//
// Created by sebastian on 25/09/23.
//

#ifndef PROYECTO_1_SERVER_H
#define PROYECTO_1_SERVER_H

#include "cJSON.h"

cJSON* createExecution();

cJSON* createTransaction();


void add_event_move(cJSON* root, char* event_type, int start_id, int end_id);

void add_event_edit(cJSON* root, int element_id, int tag, int data, char* target_state);

#endif //PROYECTO_1_SERVER_H