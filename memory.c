#include "memory.h"

MemoryElement * new_memory_element(char type_id, void * address, char * owner) {
    MemoryElement * memel = malloc(sizeof(MemoryElement));
    memel->type_id = type_id;
    memel->address = address;
    memel->owner = owner;
    return  memel;
}

void gargbage_destructor(Array ** memory_table) {
    while (*memory_table) {
        Array * element = *memory_table;
        if (element->type_id == FUNCTION) {
            Function * function = element->element;
            function_destructor(function);
            element->element = NULL;
        } else if (element->type_id == VIARRAY) {
            Node * viarray = element->element;
            namespace_destructor(viarray);
            element->element = NULL;
        }
        memory_table++;
    }
}

void display_memory_table(Array ** memory_table) {
    printf("*------------------------------------- MEM. ---------------------------------------* \n");
    printf("|       TYPE       |   ADDRESS    |                META               |    OWNER   |\n");
    printf("*----------------------------------------------------------------------------------* \n");
    int total = 0; int total_freed = 0;
    while (*memory_table) {
        MemoryElement * memel = (*memory_table)->element;
        char meta[255] = "\0"; char spaces[255] = "\0";
        char owner[255] = "\0"; char owner_spaces[255] = "\0";
        if ( memel->type_id == VIARRAY ) {
            Node *viarray = memel->address;
            if (viarray != NULL) sprintf(meta, "%d", viarray->key);
            else {
                strcat(meta, "#FREED#");
                total_freed++;
            }
            strcpy(owner, memel->owner);
            int length = strlen(meta); int owner_length = strlen(owner);
            while (owner_length++ < 15) strcat(owner_spaces, " ");
            while (length++ < 33) strcat(spaces, " ");
            printf("| <VIARRAY>        |");
            total++;
        } else if ( memel->type_id == FUNCTION ) {
            Function * function = memel->address;
            if (function != NULL) strcpy(meta, function->name);
            else {
                strcat(meta, "#FREED#");
                total_freed++;
            }
            strcpy(owner, memel->owner);
            int length = strlen(meta); int owner_length = strlen(owner);
            while (owner_length++ < 15) strcat(owner_spaces, " ");
            while (length++ < 33) strcat(spaces, " ");
            printf("| <FUNCTION>       |");
            total++;
        } else {
            strcat(meta, "SUBDATA");
            strcpy(owner, memel->owner);
            int length = strlen(meta); int owner_length = strlen(owner);
            while (owner_length++ < 15) strcat(owner_spaces, " ");
            while (length++ < 33) strcat(spaces, " ");
            printf("| <SUBDATA>        |");
        }
        printf(" [0x%p] | %s%s|%s%s|\n", memel->address, meta, spaces, owner, owner_spaces);
        memory_table++;
    }
    printf("*---------------------------------------------------------------------------------------* \n");
    printf("TOTAL: %d, FREED: %d", total, total_freed);
}