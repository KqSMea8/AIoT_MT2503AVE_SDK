//
// Created by wanghui on 17/8/13.
//

#ifndef __LINKEDLIST_H
#define __LINKEDLIST_H
#include "lock_data.h"
#include "kal_public_api.h"

typedef struct node{
    void *data;
    struct node *next;
    struct node *pre;
}NODE;

typedef struct linkedlist{
    NODE *header;
    NODE *tail;
}LINKEDLIST;

typedef BYTE (*find_func)(void *data, WORD key);

LINKEDLIST *create_linked_list();
void add_one_node(LINKEDLIST *linkedlist, void *data);
NODE* delete_one_node(LINKEDLIST *linkedlist, NODE *node);
NODE *get_one_node_by_key(LINKEDLIST *linkedlist, find_func func, WORD key);
NODE *delete_one_node_by_key(LINKEDLIST *linkedlist, find_func func, WORD key);
void node_free(NODE* node);
#endif //__LINKEDLIST_H
