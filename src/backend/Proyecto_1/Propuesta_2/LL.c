#include <stdio.h>
#include <stdlib.h>
#include "structs.h"


LL* LL_construct(LL* list)
{
    list = malloc(sizeof(LL));
    list->first=NULL;
    return list;
}

int LL_is_empty(LL* list)
{
    return (list->first==NULL);
}

void LL_insert_first(LL* list, void* data)
{
    SLL_node* new_node = malloc(sizeof(SLL_node));
    new_node->data = data;
    new_node->next = list->first;
    list->first = new_node;
}


int LL_get_length(LL* list)
{
    int length=0;
    if(!LL_is_empty(list))
    {
        SLL_node* piv;
        piv=list->first;
        while(piv != NULL)
        {
            piv = piv->next;
            length++;
        }
    }
    return length;
}

void LL_print_node_as_int(SLL_node* node)
{
    int data = *((int *)node->data);
    printf("(%d)->", data);
}
void LL_print_node_as_instruction(SLL_node* node)
{
    Instruction* data = (Instruction*)node->data;
    printf("(Type:\t%c\tMem Addr: %d)->", data->type, data->mem_addr);
}

void LL_println(LL* list, int type)
{
    if(LL_is_empty(list))
    {
        printf("\nEmpty\n");
    }
    else
    {
        SLL_node* piv;
        piv=list->first;
        while(piv != NULL)
        {
            switch (type)
            {
                case 0: LL_print_node_as_int(piv); break;
                case 1: LL_print_node_as_instruction(piv); break;
                default: printf("None ");
            }
            piv = piv->next;
        }
    }
    printf("\n");
}
///


void LL_append(LL* list, void* new_data)
{
    SLL_node* new_node = malloc(sizeof(SLL_node));
    SLL_node* piv = list->first;
    new_node->data = new_data;
    new_node->next = NULL;
    if(LL_is_empty(list))
    {
        list->first = new_node;
        return;
    }
    while(piv->next != NULL)
    {
        piv = piv->next;
    }
    piv->next = new_node;
}

void LL_delete_first(LL* list)
{
    if(!LL_is_empty(list))
    {
        SLL_node* temp = list->first;
        list->first = list->first->next;
        free(temp);
    }
}
void LL_delete_last(LL* list)
{
    if(!LL_is_empty(list))
    {
        if(list->first->next==NULL)
        {
            list->first=NULL;
        }
        else
        {
            SLL_node* piv = list->first;
            SLL_node* prev_from_piv = list->first;
            while(piv->next!=NULL)
            {
                piv=piv->next;
                if(piv->next!=NULL)
                {
                    prev_from_piv=prev_from_piv->next;
                }
            }
            prev_from_piv->next = NULL;
            free(piv);
        }
    }
}