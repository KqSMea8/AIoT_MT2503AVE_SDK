//
// Created by WANGHUI on 17/8/13.
//

#include <stdlib.h>
#include "linked_list.h"

LINKEDLIST *create_linked_list()
{
    LINKEDLIST *linkedlist = (LINKEDLIST*)get_ctrl_buffer(sizeof(LINKEDLIST));
    memset(linkedlist, 0, sizeof(LINKEDLIST));

    return linkedlist;
}

void add_one_node(LINKEDLIST *linkedlist, void *data)
{
    if((linkedlist!=NULL) && (data!=NULL)){
	    NODE *node = (NODE*)get_ctrl_buffer(sizeof(NODE));
           memset(node, 0, sizeof(NODE));
	    node->data = data;

	    if(linkedlist->header == NULL){
	        linkedlist->header = linkedlist->tail = node;
	        node->next = node->pre = NULL;
	    } else {
	        linkedlist->tail->next = node;
	        node->next = NULL;
	        node->pre = linkedlist->tail;
	        linkedlist->tail = node;
	    }
    	}
}

NODE* delete_one_node(LINKEDLIST *linkedlist, NODE *node)
{
    if(node != NULL){
        if(node == linkedlist->tail && node == linkedlist->header){
            linkedlist->header = linkedlist->tail = NULL;
        } else if(node == linkedlist->tail){
            linkedlist->tail =node->pre;
            node->pre->next = NULL;
            node->pre = NULL;
        } else if(node == linkedlist->header) {
            node->next->pre = NULL;
            linkedlist->header = node->next;
            node->next = NULL;
        } else {
            node->pre->next = node->next;
            node->next->pre = node->pre;
            node->pre = node->next = NULL;
        }
    }

    return node;
}

NODE *get_one_node_by_key(LINKEDLIST *linkedlist, find_func func, WORD key)
{
    NODE *node = linkedlist->header;
    if(func != NULL  && key >= 0) {
	    while(node != NULL){
	        if(func(node->data, key) == 0) {
	            break;
	        }
	        node = node->next;
	    }
    }

    return node;
}

NODE *delete_one_node_by_key(LINKEDLIST *linkedlist, find_func func, WORD key)
{
    NODE *node = get_one_node_by_key(linkedlist, func, key);

    if(node != NULL){
        if(node == linkedlist->tail && node == linkedlist->header){
            linkedlist->header = linkedlist->tail = NULL;
        } else if(node == linkedlist->tail){
            linkedlist->tail =node->pre;
            node->pre->next = NULL;
            node->pre = NULL;
        } else if(node == linkedlist->header) {
            node->next->pre = NULL;
            linkedlist->header = node->next;
            node->next = NULL;
        } else {
            node->pre->next = node->next;
            node->next->pre = node->pre;
            node->pre = node->next = NULL;
        }
    }

    return node;
}

void node_free(NODE* node)
{
	if(node != NULL) {

		if(node->data != NULL) {
			order_info *p = (order_info*)node->data;
			free_ctrl_buffer(node->data);
		}
		node->data = NULL;
		free_ctrl_buffer(node);
	}
}