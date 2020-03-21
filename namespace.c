#include "namespace.h"
#include "arithmetica.h"

Node * new_node(int key, void * value) {
    Node * node = malloc(sizeof(Node));
    node->key = key;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
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
        } else if (root->key == node->key) {
            constant_destructor(root->value);
            root->value = node->value;
            free(node);
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

int namespace_destructor(Node * root) {
    if (root == NULL) return 0;
    else {
        constant_destructor(root->value);
        if (root->left != NULL) namespace_destructor(root->left);
        if (root->right != NULL) namespace_destructor(root->right);
        free(root);
    }
}