#include "namespace.h"
#include "arithmetica.h"
#include "display.h"

NameSpaceObject * new_namespace_object(char * name, Node * value) {
    NameSpaceObject * namespace_object = malloc(sizeof(NameSpaceObject));
    namespace_object->name = name;
    namespace_object->namespace = value;
    return namespace_object;
}

Node * new_node(int key, void * value) {
    Node * node = malloc(sizeof(Node));
    node->key = key;
    node->raw_key = NULL;
    node->value = value;

    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;

    node->extend = NULL;
    node->is_global = 0;
}

Node * extending(Node * root, Node * extended_namespace) {
    if (root != NULL) {
        Constant * node_value = root->value;
        Constant * extended_value = new_constant(node_value->type_id, copy_data(node_value->value, node_value->type_id));
        Node * extended_node = new_node(root->key, extended_value);
        if (root->extend == NULL) extended_node->extend = root;
        (extended_namespace == NULL) ? extended_namespace = insert_node(extended_namespace, extended_node) : insert_node(extended_namespace, extended_node);
        if (root->left != NULL) extending(root->left, extended_namespace);
        if (root->right != NULL) extending(root->right, extended_namespace);
        return extended_node;
    } else return NULL;
}

void * remove_node(Node * removed_node) {
    if (removed_node == NULL) return NULL;
    void * removed_node_val = removed_node->value;
    Node * parent = removed_node->parent;
    if (removed_node->right != NULL) {
        Node * minimal_key_node = get_minimal_by_key(removed_node->right);
        Node * parent_of_minimal = minimal_key_node->parent;
        if (parent != NULL) {
            if (parent_of_minimal != removed_node) {
                /* if parent of minimal by key node is not a removed node */
                parent_of_minimal->left = NULL;
                minimal_key_node->parent = parent;
                minimal_key_node->left = removed_node->left;
                minimal_key_node->right = removed_node->right;
                memcpy(removed_node, minimal_key_node, sizeof(Node));
                free(minimal_key_node);
            } else if (parent_of_minimal == removed_node) {
                parent_of_minimal->parent = parent;
                minimal_key_node->left = removed_node->left;
                minimal_key_node->parent = parent;
                memcpy(removed_node, minimal_key_node, sizeof(Node));

                /* set the new parent for child nodes */
                set_parent(removed_node->left, removed_node); set_parent(removed_node->right, removed_node);
                free(minimal_key_node);
            }
        } else {
            if (parent_of_minimal != removed_node) {
                parent_of_minimal->left = NULL;
                minimal_key_node->parent = NULL;
                minimal_key_node->left = removed_node->left;
                minimal_key_node->right = removed_node->right;
                memcpy(removed_node, minimal_key_node, sizeof(Node));
                free(minimal_key_node);
            } else if (parent_of_minimal == removed_node) {
                minimal_key_node->parent = NULL;
                minimal_key_node->left = removed_node->left;
                memcpy(removed_node, minimal_key_node, sizeof(Node));

                /* set the new parent for child nodes */
                set_parent(removed_node->left, removed_node); set_parent(removed_node->right, removed_node);
                free(minimal_key_node);
            }
        }
    } else if (removed_node->left != NULL) {
        Node * left_node = removed_node->left;
        memcpy(removed_node, left_node, sizeof(Node));

        /* set the new parent for child nodes */
        set_parent(removed_node->left, removed_node); set_parent(removed_node->right, removed_node);
        removed_node->parent = parent;
        free(left_node);
    } else {
        /* the removed node is a last node of tree */
        if (parent->left == removed_node) parent->left = NULL;
        else if (parent->right == removed_node) parent->right = NULL;
        free(removed_node);
    }
    return removed_node_val;
}

void set_parent(Node * node, Node * parent) {
    if (node != NULL) node->parent = parent;
}

Node * get_minimal_by_key(Node * root) {
    if (root->left == NULL) return root;
    else {
        return get_minimal_by_key(root->left);
    }
}

Node * insert_node(Node * root, Node * node) {
    if (root == NULL) return node;
    else {
        if (root->key > node->key) {
            if (root->left == NULL) {
                node->parent = root;
                root->left = node;
            } else insert_node(root->left, node);
        } else if (root->key < node->key) {
            if (root->right == NULL) {
                node->parent = root;
                root->right = node;
            } else insert_node(root->right, node);
        } else if (root->key == node->key) {
            root->value = node->value;
            free(node);
        }
    }
}

void * find_node(Node * root, int key) {
    if (root == NULL) return NULL;
    if (root->key == key) return root;
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
        Constant * value = root->value;
        if (is_simple_data(value->type_id)) {
            free(value->value); free(value);
        } else free(value);

        if (root->left != NULL) namespace_destructor(root->left);
        if (root->right != NULL) namespace_destructor(root->right);
        free(root->raw_key);
        free(root);
    }
}

int namespace_object_destructor(NameSpaceObject * namespace_object) {
    free(namespace_object->name);
    namespace_destructor(namespace_object->namespace);
    free(namespace_object);
}

int is_belonged(Node * root, Node * node) {
    if (node == NULL || root == NULL) return 1;
    Node * potential = find_node(root, node->key);
    if (potential == node) return 1;
    return 0;
}

void display_node(Node * node) {
    printf("%d => ", node->key); display_constant(node->value);
}

void display_namespace(Node * root) {
    if (root != NULL) {
        printf("*----------------------------------------------------------------------------*\n");
        printf("|hash: %d | value: ", root->key); display_constant(root->value); printf("|\n");
        display_namespace(root->left);
        display_namespace(root->right);
    }
}