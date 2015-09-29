#include <string.h>

#include "label.h"

void manage_label(char * label, unsigned short int pos[]) {

    if (!label_verify(label)) {
        ERROR("label: invalid label name");
        // ERRO: Nome invalido de rotulo
    }

    label[strlen(label)-1] = '\0';

    int value;

    if(pos[0] == 0 && pos[1] == 1)
        value = -1025;

    if (pos[1] == 0) value = pos[0];
    else {
        if (pos[0] == 0) value = -1025;
        else value = -pos[0];
    }


    if (!insert_instance(rotulos, label, value)) {
        ERROR("label: label already exists");
        // ERRO: rotulo ja existe
    }
}

////////////////////////////////////////////////////////////////////////////////////////

