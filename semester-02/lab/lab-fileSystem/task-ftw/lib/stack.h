#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define STACK_SIZE 1000000
#define MAX_NAME 200


typedef struct node_s {
    char fileName[MAX_NAME];
    long depth;
    struct node_s *next;
} node_t;


int isEmpty(node_t **stack);
node_t *create_node(char *fileName, int depth);
int push(node_t **stack, node_t *elem);
node_t *pop(node_t **stack);

#endif
