
#include "cJSON.h"

cJSON* createExecution(){
    cJSON* root;
    root = cJSON_CreateObject();
    return root;
}

cJSON* createTransaction(){    
    cJSON *root;
    root = cJSON_CreateObject();
    return root;
}

void add_event_move(cJSON* root, char* event_type, int start_id, int end_id){

    cJSON_AddStringToObject(root, "event_type", event_type);
    cJSON_AddNumberToObject(root, "start_id", start_id);
    cJSON_AddNumberToObject(root, "end_id", end_id);
}

void add_event_edit(cJSON* root, int element_id, int tag, int data, char* target_state){

    cJSON_AddNumberToObject(root, "element_id", element_id);
    cJSON_AddNumberToObject(root, "tag", tag);
    cJSON_AddNumberToObject(root, "data", data);
    cJSON_AddStringToObject(root, "target_state", target_state);   
}

void add_log(cJSON* root){
    
}
