#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "rotulos.h"

typedef struct RegLista {
    Rotulo rotulo;
    struct RegLista * prox;
} RegLista, *Lista;

typedef struct RegBase {
    int n_rotulos;
    Lista tabela[MAX_HASH];
} RegBase, *ImplBase;

int somaString(String name) {
    int sum = 0;

    while (*name != '\0') {
        sum += *name;
        name++;
    }
    return sum;
}

short int Espalha(int k) {
    return (k%MAX_HASH);
} /* Espalha */

Base HashInit() {
    ImplBase b = malloc (sizeof(RegBase));

    b->n_rotulos = 0;

    for (short int i = 0; i < MAX_HASH; i++)
        b->tabela[i] = NULL;

    return (Base)b;
}

Boolean InsereBase (Base p, String name, unsigned short int mem_line) {
    ImplBase base = (ImplBase) p;
    Rotulo * rot = malloc (sizeof(Rotulo));

    (*rot).name = malloc((strlen(name) + 1) * sizeof(char));
    strcpy((*rot).name, name);

    (*rot).mem_line = mem_line;

    short int hash = Espalha(somaString((*rot).name));

    Lista aux = base->tabela[hash];

    if (aux == NULL || strcmp((aux->rotulo).name, name) > 0) {
        base->tabela[hash] = malloc(sizeof(RegLista));
        (base->tabela[hash])->prox = aux;
        (base->tabela[hash])->rotulo = *rot;
        (base->n_rotulos)++;
        return true;
    } else {
        while (true) {
            /* Caso encontre rotulo com o mesmo nome, nao sera possivel inserir o rotulo na tabela */
            if (strcmp((aux->rotulo).name, name) == 0) return false;

            if (strcmp((aux->rotulo).name, name) < 0) {
            /* Caso o nome do rotulo apontado por aux seja menos que o nome do rotulo a ser inserido, este
             * sera inserido apos o rotulo apontado por aux */
                if ((aux->prox == NULL) || strcmp(((aux->prox)->rotulo).name, name) > 0) {
                /* Caso nao tenha mais nenhum rotulo na lista ou o proximo rotulo tiver o nome maior, o
                 * rotulo sera inserido em frente ao rotulo apontado por aux */
                    Lista aux1 = aux->prox;
                    aux->prox = malloc(sizeof(RegLista));
                    aux = aux->prox;
                    aux->prox = aux1;
                    aux->rotulo = *rot;
                    (base->n_rotulos)++;
                    return true;
                } else aux = aux->prox;
                /* Caso contrario, segue-se na lista */
            }
        }
    }
}

void ImprimeBase(Base p) {
/* Imprime, os registros contidos na base 'p', um por linha.  A ordem
   de impressao segue a ordem das entradas da tabela, e para cada
   entrada, a ordem da lista ligada.  Cada linha come�a com o �ndice
   correspondente na tabela de espalhamento. Deve ser usado o formato
   "(%3d) %06d %s\n". */
    ImplBase base = (ImplBase)p;
    Lista aux;

    if (base->n_rotulos == 0) return;

    for (int i = 0; i < MAX_HASH; i++) {
    /* Percorre todas as listas da tabela */
        aux = base->tabela[i];
        while (aux != NULL) {
        /* Percorre os nos das listas */
            printf("(%3d) %06d %s\n", i, (aux->rotulo).mem_line, (aux->rotulo).name);
            aux = aux->prox;
        }
    }

} /* ImprimeBase */

unsigned short int ConsultaBase(Base p, String name) {
// Retorna o numero de memoria do rotulo.
// Se o rótulo não existir retorna 0x400
    ImplBase p = (ImplBase) p;
    int hash = Espalha(somaString(name));
    Lista aux = base->tabela[hash];


    while (aux != NULL) {
        if (strcmp((aux->rotulo).name, name) == 0) {
            /* Encontra o rotulo com o ra procurado */
            return (aux->rotulo).mem_line;
        }
        aux = aux->prox;
    }
    /* Nao encontra o rotulo */
    return 0x400;
}/* ConsultaBase */

