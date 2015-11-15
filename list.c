#include <stdlib.h>
#include <stdio.h>

#include "list.h"

/**
 *	Init a node with a given value.
 */
list_head create_node_pointer (double value) {
	list_head node = (list_head) malloc (sizeof(struct list_head));
	if (NULL == node) {
        perror ("Not enough memory to create node.");
        exit (EXIT_FAILURE);
	}

	node->value = value;
	node->next  = NULL;

	return node;
}

/**
 *	Free a single-linked list.
 */
void free_list (list_head node) {
	if (NULL == node) return;

	list_head tmp;

	while (NULL != node && NULL != node->next) {
		tmp = node->next;
		free (node);
		node = tmp;
	}
	free (node);
}
