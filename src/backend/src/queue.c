#include <stdio.h>
#include <stdlib.h>
#include "structs.h"


Queue* construct_queue(Queue* queue)
{
    queue->first=NULL;
    return queue;
}

int Queue_is_empty(Queue* queue)
{
    return (queue->first==NULL);
}

void Queue_insert_first(Queue* queue, void* data)
{
    SLL_node* new_node = malloc(sizeof(SLL_node));
    new_node->data = data;
    new_node->next = queue->first;
    queue->first = new_node;
}

void enqueue(Queue* queue, void* new_data)
{
    SLL_node* new_node = malloc(sizeof(SLL_node));
    SLL_node* piv = queue->first;
    new_node->data = new_data;
    new_node->next = NULL;
    if(Queue_is_empty(queue))
    {
        queue->first = new_node;
        return;
    }
    while(piv->next != NULL)
    {
        piv = piv->next;
    }
    piv->next = new_node;
}

void* dequeue(Queue* queue)
{
    void* data;
    if(!Queue_is_empty(queue))
    {
        SLL_node* temp = queue->first;
        data = temp->data;
        queue->first = queue->first->next;
        free(temp);
        return data;
    }
}

int Queue_get_length(Queue* queue)
{
    int length=0;
    if(!Queue_is_empty(queue))
    {
        SLL_node* piv;
        piv=queue->first;
        while(piv != NULL)
        {
            piv = piv->next;
            length++;
        }
    }
    return length;
}
void Queue_destroy(Queue* queue)
{
    while (!Queue_is_empty(queue))
    {
        void* data = dequeue(queue);
        free(data); // Free the data once it's dequeued
    }
    free(queue); // Free the memory allocated for the queue structure itself
}

void Queue_print_node_as_int(SLL_node* node)
{
    int data = *((int *)node->data);
    printf("(%d)->", data);
}
void Queue_print_node_as_instruction(SLL_node* node)
{
    Instruction* data = (Instruction*)node->data;
    printf("(Instruction type: %c, Mem Addr: %d)->\n", data->type, data->mem_addr);
}
void Queue_print_node_as_MT_msg(SLL_node* node)
{
    MT_msg* msg = (MT_msg*)node->data;
    if(msg==NULL)
    {
        printf("NULL\n");
    }
    else
    {
        printf("($: %d\t", msg->cache_id);
        if(msg->MT_request_type){printf("Request: WRITE_REQUEST\t");}
        else{printf("Request: READ_REQUEST\t");}
        printf("Mem addr: %d\t\t", msg->mem_addr);
        printf("Data: %d)->\n", msg->data);
    }
}
void Queue_int_println(Queue* queue)
{
    if(Queue_is_empty(queue))
    {
        printf("Empty");
    }
    else
    {
        SLL_node* piv;
        piv=queue->first;
        while(piv != NULL)
        {
            Queue_print_node_as_int(piv);
            piv = piv->next;
        }
    }
    printf("\n");
}
void Queue_instruction_println(Queue* queue)
{
    if(Queue_is_empty(queue))
    {
        printf("Empty\n");
    }
    else
    {
        SLL_node* piv;
        piv=queue->first;
        while(piv != NULL)
        {
            Queue_print_node_as_instruction(piv);
            piv = piv->next;
        }
    }
}
void Queue_msg_println(Queue* queue)
{
    if(Queue_is_empty(queue))
    {
        printf("Empty");
    }
    else
    {
        SLL_node* piv;
        piv=queue->first;
        while(piv != NULL)
        {
            Queue_print_node_as_MT_msg(piv);
            piv = piv->next;
        }
    }
    printf("\n");
}
void Queue_copy(Queue* original_queue, Queue* copy_queue)
{
    if(!Queue_is_empty(copy_queue))
    {
        SLL_node* piv;
        piv=copy_queue->first;
        while(piv != NULL)
        {
            piv = piv->next;
            dequeue(copy_queue);
        }
    }
    if(!Queue_is_empty(original_queue))
    {
        SLL_node* piv;
        piv=original_queue->first;
        while(piv != NULL)
        {
            enqueue(copy_queue, piv->data);
            piv = piv->next;
        }
    }
}