#include "stack.h"
#include <stdlib.h>
#include <string.h>

int isEmpty(node_t **stack) {
    return (*stack == NULL);
}

node_t *create_node(char *fileName, int depth) {
    node_t *tmp = malloc(sizeof(node_t));
    if (!tmp) return NULL;
    strcpy(tmp->fileName, fileName);
    tmp->depth = depth;
    tmp->next = NULL;
    return tmp;
}

int push(node_t **stack, node_t *elem) {
    node_t *tmp = create_node(elem->fileName, elem->depth);
    if (!tmp) return -1;

    tmp->next = *stack;
    *stack = tmp;
    return 0;
}

node_t *pop(node_t **stack) {
    if (!stack || !(*stack)) return NULL;

    node_t *tmp = *stack;
    *stack = (*stack)->next;
    return tmp;
}
