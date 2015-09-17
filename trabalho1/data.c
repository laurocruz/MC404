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

Boolean insert_instance(Table p, String name, int value) {
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
            /* Caso encontre rotulo com o mesmo nome, nao sera possivel inserir o rotulo na tabela */
            if (strcmp((aux->nod).name, name) == 0) return false;

            if (strcmp((aux->nod).name, name) < 0) {
            /* Caso o nome do rotulo apontado por aux seja menos que o nome do rotulo a ser inserido, este
             * sera inserido apos o rotulo apontado por aux */
                if ((aux->next == NULL) || strcmp(((aux->next)->nod).name, name) > 0) {
                /* Caso nao tenha mais nenhum rotulo na lista ou o proximo rotulo tiver o nome maior, o
                 * rotulo sera inserido em frente ao rotulo apontado por aux */
                    List aux1 = aux->next;
                    aux->next = malloc(sizeof(RegList));
                    aux = aux->next;
                    aux->next = aux1;
                    aux->nod = *rot;
                    (base->n_nods)++;
                    return true;
                } else aux = aux->next;
                /* Caso contrario, segue-se na lista */
            }
        }
    }
}

void print_table(Table p) {
/* Imprime, os registros contidos na base 'p', um por linha.  A ordem
   de impressao segue a ordem das entradas da tabela, e para cada
   entrada, a ordem da lista ligada.  Cada linha come�a com o �ndice
   correspondente na tabela de espalhamento. Deve ser usado o formato
   "(%3d) %06d %s\n". */
    ITable base = (ITable)p;
    List aux;

    if (base->n_nods == 0) return;

    for (int i = 0; i < MAX_HASH; i++) {
    /* Percorre todas as listas da tabela */
        aux = base->list[i];
        while (aux != NULL) {
        /* Percorre os nos das listas */
            printf("(%3d) %06d %s\n", i, (aux->nod).value, (aux->nod).name);
            aux = aux->next;
        }
    }

} /* ImprimeBase */

int get_value(Table p, String name) {
// Retorna o numero de memoria do rotulo.
// Se o rótulo não existir retorna 0x400
    ITable base = (ITable) p;
    int hash = Scatter(sum_string(name));
    List aux = base->list[hash];


    while (aux != NULL) {
        if (strcmp((aux->nod).name, name) == 0) {
            /* Encontra o rotulo com o ra procurado */
            return (aux->nod).value;
        }
        aux = aux->next;
    }
    /* Nao encontra o rotulo */
    return -1024;
}/* ConsultaBase */

/*
int main() {
    Table test = HashInit();

    int n;

    scanf("%d", &n);

    char str[100];
    int value;
    for (int i = 0; i < n; i++) {
        scanf("%s %d", str, &value);
        insert_instance(test, str, value);
    }
    print_table(test);

    for (int i = 0; i < n; i++) {
        scanf("%s", str);
        value = get_value(test, str);
        printf("%d\n", value);
    }   

    return 0;
}
*/

