#include "namespace.h"

Node * new_space(void) {
    Node * root = malloc(sizeof(Node));
    root->key = NULL;
    root->value = NULL;
    return root;
}

Node * new_node(unsigned int key, void * value) {
    Node * node = malloc(sizeof(Node));
    node->key = key;
    node->value = value;
}

void insert_node(Node * root, Node * node) {
    if (root == NULL) root = node;
    else {
        if (root->key > node->key) {
            if (root->left == NULL) root->left = node;
            else insert_node(root->left, node);
        } else if (root->key < node->key) {
            if (root->right == NULL) root->right = node;
            else insert_node(root->right, node);
        } else if (root->key == node->key) root = node;
    }
}

unsigned int faq6(const char * str) {
    unsigned int hash = 0;
    while (*str) {
        hash += (unsigned char)(*str);
        hash += (hash << 10);
        hash ^= (hash >> 6);
        str++;
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}