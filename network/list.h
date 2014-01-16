#ifndef _INIT_LIST_H_
#define _INIT_LIST_H_

#ifndef offsetof
/* Offset of member MEMBER in a struct of type TYPE. */
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

struct listnode
{
    struct listnode *next;
    struct listnode *prev;
};

#define node_to_item(node, container, member) \
    (container *) (((char*) (node)) - offsetof(container, member))

#define list_declare(name) \
    struct listnode name = { \
        .next = &name, \
        .prev = &name, \
    }

#define list_for_each(node, list) \
    for (node = (list)->next; node != (list); node = node->next)

#define list_for_each_reverse(node, list) \
    for (node = (list)->prev; node != (list); node = node->prev)

void list_init(struct listnode *list);
void list_add_tail(struct listnode *list, struct listnode *item);
void list_remove(struct listnode *item);

#define list_empty(list) ((list) == (list)->next)
#define list_head(list) ((list)->next)
#define list_tail(list) ((list)->prev)

#endif


void list_init(struct listnode *node)
{
    node->next = node;
    node->prev = node;
}

void list_add_tail(struct listnode *head, struct listnode *item)
{
    item->next = head;
    item->prev = head->prev;
    head->prev->next = item;
    head->prev = item;
}

void list_remove(struct listnode *item)
{
    item->next->prev = item->prev;
    item->prev->next = item->next;
}


