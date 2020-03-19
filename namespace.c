#include "namespace.h"

Node * new_space(void) {
    Node * root = malloc(sizeof(Node));
    root->key = NULL;
    root->value = NULL;
    return root;
}

Node * new_node(int key, void * value) {
    Node * node = malloc(sizeof(Node));
    node->key = key;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
}

Node * insert_node(Node * root, Node * node) {
    if (root == NULL) {
        root = node;
        return root;
    } else {
        if (root->key > node->key) {
            if (root->left == NULL) root->left = node;
            else insert_node(root->left, node);
        } else if (root->key < node->key) {
            if (root->right == NULL) root->right = node;
            else insert_node(root->right, node);
        } else if (root->key == node->key) {
            root = node;
            return root;
        }
    }
}

void * find_node(Node * root, int key) {
    if (root == NULL) return NULL;
    if (root->key == key) return root->value;
    else {
        if (root->key > key) find_node(root->left, key);
        else if (root->key < key) find_node(root->right, key);
     }
}

int faq6(const char * str) {
    unsigned int hash = 0;
    while (*str) {
        hash += (char)(*str);
        hash += (hash << 10);
        hash ^= (hash >> 6);
        str++;
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}