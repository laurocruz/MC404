#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "data.h"

typedef struct RegList {
    Data nod;
    struct RegList * next;
} RegList, *List;

typedef struct RegTable {
    int n_nods;
    List list[MAX_HASH];
} RegTable, *ITable;

int sum_string(String name) {
    int sum = 0;

    while (*name != '\0') {
        sum += *name;
        name++;
    }
    return sum;
}

short int Scatter(int k) {
    return (k%MAX_HASH);
} /* Scatter */

Table HashInit() {
    ITable b = malloc (sizeof(RegTable));

    b->n_nods = 0;

    for (short int i = 0; i < MAX_HASH; i++)
        b->list[i] = NULL;

    return (Table)b;
}

bool insert_instance(Table p, String name, int value) {
    ITable base = (ITable) p;
    Data * rot = malloc (sizeof(Data));

    (*rot).name = malloc((strlen(name) + 1) * sizeof(char));
    strcpy((*rot).name, name);

    (*rot).value = value;

    short int hash = Scatter(sum_string((*rot).name));

    List aux = base->list[hash];

    if (aux == NULL || strcmp((aux->nod).name, name) > 0) {
        base->list[hash] = malloc(sizeof(RegList));
        (base->list[hash])->next = aux;
        (base->list[hash])->nod = *rot;
        (base->n_nods)++;
        return true;
    } else {
        while (true) {
            if (strcmp((aux->nod).name, name) == 0) return false;

            if (strcmp((aux->nod).name, name) < 0) {
                if ((aux->next == NULL) || strcmp(((aux->next)->nod).name, name) > 0) {
                    List aux1 = aux->next;
                    aux->next = malloc(sizeof(RegList));
                    aux = aux->next;
                    aux->next = aux1;
                    aux->nod = *rot;
                    (base->n_nods)++;
                    return true;
                } else aux = aux->next;
            }
        }
    }
}

void print_table(Table p) {
    ITable base = (ITable)p;
    List aux;

    if (base->n_nods == 0) return;

    for (int i = 0; i < MAX_HASH; i++) {
        aux = base->list[i];
        while (aux != NULL) {
            printf("(%3d) %06d %s\n", i, (aux->nod).value, (aux->nod).name);
            aux = aux->next;
        }
    }

}

int get_value(Table p, String name) {
// returns -1024 if nod does not exist
    ITable base = (ITable) p;
    int hash = Scatter(sum_string(name));
    List aux = base->list[hash];


    while (aux != NULL) {
        if (strcmp((aux->nod).name, name) == 0) {
            return (aux->nod).value;
        }
        aux = aux->next;
    }
    /* does not find label/SYM */
    return -1024;
}

void free_table(Table p) {
    ITable base = (ITable) p;
    List aux1, aux2;

    if (base->n_nods == 0) return;

    for (int i = 0; i < MAX_HASH; i++) {
        aux1 = base->list[i];
        while (aux1 != NULL) {
            aux2 = aux1->next;
            free((aux1->nod).name);
            free(aux1);
            aux1 = aux2;
        }
    }
    free(base);

}
