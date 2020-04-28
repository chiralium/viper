#include "memory.h"
#include "functions.h"

MemoryElement * new_memory_element(char type_id, void * address, char * owner) {
    MemoryElement * memel = malloc(sizeof(MemoryElement));
    if (type_id == VIARRAY) {
        Node * viarray = address;
        char meta[255];
        sprintf(meta, "%d", viarray->key);
        memel->meta = alloc_string(meta);
        memel->type = alloc_string("<viarray>");
    } else if (type_id == FUNCTION_CONTAINER) {
        FunctionContainer * function_container = address;
        char meta[255]; strcpy(meta, function_container->name);
        memel->meta = alloc_string(meta);
        memel->type = alloc_string("<function cont.>");
    } else if (type_id == FUNCTION) {
        Function *function = address;
        char meta[255]; char signature[255]; sprintf(signature, "[%d]", get_function_signature(function->arg_list) - 1);
        strcpy(meta, function->name); strcat(meta, signature);
        memel->meta = alloc_string(meta);
        memel->type = alloc_string("<function>");
    } else if (type_id == KEYPAIR) {
        Node *keypair = address;
        char meta[255]; sprintf(meta, "%d", keypair->key);
        memel->meta = alloc_string(meta);
        memel->type = alloc_string("<keypair>");
    } else {
        memel->meta = alloc_string("<subdata>");
        memel->type = alloc_string("<subdata>");
    }
    memel->type_id = type_id;
    memel->owner = owner;
    memel->address = address;
    memel->is_freed = 0;
    return  memel;
}

void garbage_destructor(Array ** memory_table) {
    while (*memory_table) {
        Array * element = *memory_table; MemoryElement * memel = element->element;
        if (memel->type_id == FUNCTION_CONTAINER) {
            FunctionContainer * function_container = memel->address;
            function_container_destructor(function_container);
            memel->is_freed = 1;
        } else if (memel->type_id == FUNCTION) {
            Function * function = memel->address;
            function_destructor(function);
            memel->is_freed = 1;
        } else if (memel->type_id == VIARRAY) {
            Node * viarray = memel->address;
            namespace_destructor(viarray);
            memel->is_freed = 1;
        }
        memory_table++;
    }
}

void display_memory_table(Array ** memory_table) {
    printf("*---------------------------------------------------- MEM. ----------------------------------------------------* \n");
    printf("|       TYPE       |   ADDRESS    |               META               |                OWNER             | free |\n");
    printf("*--------------------------------------------------------------------------------------------------------------* \n");
    int counter = 0;
    while (memory_table[counter]) {
        MemoryElement * memel = memory_table[counter]->element;
        char spaces[255] = "\0"; int length = strlen(memel->type); while (length++ < 17) strcat(spaces, " ");
        printf("| %s%s|", memel->type, spaces);

        printf(" [0x%p] |", memel->address);

        *spaces = '\0'; length = strlen(memel->meta); while (length++ < 33) strcat(spaces, " ");
        printf(" %s%s|", memel->meta, spaces);

        *spaces = '\0'; length = strlen(memel->owner); while (length++ < 33) strcat(spaces, " ");
        printf(" %s%s|", memel->owner, spaces);

        printf((memel->is_freed) ? " yes  |" : "  no  |");
        printf("\n");
        counter++;
    }
    printf("*--------------------------------------------------------------------------------------------------------------* \n");
}

void memory_table_destructor(Array ** memory_table) {
    int counter = 0;
    while (memory_table[counter]) {
        MemoryElement * memel = memory_table[counter]->element;
        free(memel->type); free(memel->meta); free(memel);
        free(memory_table[counter]);
        counter++;
    }
    free(memory_table);
}