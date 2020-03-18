#include <stdlib.h>
/*
 * The namespace of interpreter.
 * Made by binary tree search algorithm
 */

#ifndef VIPER_V4_NAMESPACE_H
#define VIPER_V4_NAMESPACE_H

typedef struct Node {
    unsigned long key;
    void * value;
    void * left;
    void * right;
} Node;

/* The function will allocate tree into memory */
Node * new_space(void);

/* The function will create the new node of tree */
Node * new_node(unsigned int key, void * value);

/* The function will insert the new value into tree */
void insert_node(Node * root, Node * new_node);

/* The function will find the value associated by key and return it */
void find_node(Node * root, unsigned int key);

/* The function will remove node from tree by key */
void remove_node(Node * root, unsigned int key);

/* The hash-function FAQ6 */
unsigned int faq6(const char * str);

#endif //VIPER_V4_NAMESPACE_H
