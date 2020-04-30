#include <stdlib.h>
/*
 * The namespace of interpreter.
 * Made by binary tree search algorithm
 */

#ifndef VIPER_V4_NAMESPACE_H
#define VIPER_V4_NAMESPACE_H

typedef struct Node {
    int key;
    void * value;
    struct Node * left;
    struct Node * right;
    struct Node * parent;
} Node;

/* Create new namespace that extending the another namespace */
Node * extending(Node * root, Node * extended_namespace);

/* The function will create the new node of tree */
Node * new_node(int key, void * value);

/* The function will insert the new value into tree */
Node * insert_node(Node * root, Node * new_node);

/* The function will find the value associated by key and return it */
void * find_node(Node * root, int key);

/* The function will remove node from tree by key */
void * remove_node(Node * removed_node);

/* Return the node with minimal key */
Node * get_minimal_by_key(Node * root);

/* Set the parrent for node */
void set_parent(Node * node, Node * parent);

/* Return true if node belongs to namespace */
int is_belonged(Node * root, Node * node);

/* The hash-function FAQ6 */
int faq6(const char * str);

/* Display */
void display_node(Node * node);
void display_namespace(Node * node);

/* Destructor */
int namespace_destructor(Node * root);

/* USAGE
    int * hello = malloc(sizeof(int)); *hello = 6996;
    Node * root = new_node(faq6("hello1"), hello);

    hello = malloc(sizeof(int)); *hello = 6997;
    Node * node2 = new_node(faq6("hell2"), hello);

    hello = malloc(sizeof(int)); *hello = 6998;
    Node * node3 = new_node(faq6("hell3"), hello);

    hello = malloc(sizeof(int)); *hello = 6999;
    Node * node4 = new_node(faq6("hell4"), hello);

    hello = malloc(sizeof(int)); *hello = 7000;
    Node * node5 = new_node(faq6("hell5"), hello);

    insert_node(root, node2);
    insert_node(root, node3);
    insert_node(root, node4);
    insert_node(root, node5);

    printf("%s -- %d\n", "hello1", faq6("hello1"));
    printf("%s -- %d\n", "hell2",faq6("hell2"));
    printf("%s -- %d\n", "hell3",faq6("hell3"));
    printf("%s -- %d\n", "hell4",faq6("hell4"));
    printf("%s -- %d\n", "hell5",faq6("hell5"));

    int * n = find_node(root, faq6("hello1"));
    printf("%d", *(int *)n);
*/
#endif //VIPER_V4_NAMESPACE_H
