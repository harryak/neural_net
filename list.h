#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

typedef struct list_head {
    double value;
    struct list_head *next;
} *list_head;

list_head create_node_pointer (double value);
void free_list (list_head node);

#endif // LIST_H_INCLUDED
