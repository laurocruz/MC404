#include <string.h>
#include <stdlib.h>

#include "config.h"

///////////// Global declarations ///////////////////////////////
char ignore[] = {'\n', '\r', ' ', 9, '\0'}; 
int file_line;
Table rotulos;
Table sets;

//////////////////////////////////////////

void ERROR(char * msg) {
    fprintf(stderr, "ERROR on line %d\n", file_line);
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

//////////////////// Auxiliares para tratar strings/caracteres /////////////////////////

void test_eol(char * word) {
    if (word == NULL) {
        //ERRO: Falta parâmetro
        ERROR("missing parameter");
    } else if (word[0] == '#') {
        //ERRO: Falta parâmetro
        ERROR("missing parameter");
    }  
}

bool make_line(BYTE ** line) {
    if (*line != NULL) return false;

    ALLOC_LINE(*line);

    return true;
}

char * get_line(FILE * src, char * line, int * size) {
    char * indicator;

    indicator = fgets(line, *size, src);

    while (strlen(line) == (*size)-1 && line[strlen(line)-1] != '\n') {
        line = realloc(line, (*size) + 20);
        (*size) += 20;
        fgets(line + strlen(line), 21, src);
    }
    return indicator;
}

bool label_verify(char * word) {

    if (word[0] >= '0' && word[0] <= '9') return false;

    if (word[strlen(word)-1] != ':') return false;

    for (int i = 0; i < strlen(word)-1; i++) {
        if (word[i] < '0' || (word[i] > '9' && word[i] < 'A') ||
            (word[i] > 'Z' && word[i] < '_') || word[i] == '`' ||
            word[i] > 'z')
            return false;
    }

    return true;
}

bool SYM_verify(char * word) {

    if (word[0] >= '0' && word[0] <= '9') return false;

    for (int i = 0; i < strlen(word)-1; i++) {
        if (word[i] < '0' || (word[i] > '9' && word[i] < 'A') ||
            (word[i] > 'Z' && word[i] < '_') || word[i] == '`' ||
            word[i] > 'z')
            return false;
    }

    return true;
}

bool hex_verify(char * hex) {
    if (strlen(hex) != 12) return false;

    if (hex[0] != '0' || hex[1] != 'x') return false;

    for (short int i = 2; i < strlen(hex); i++) {
        if (hex[i] <= 0x2F || (hex[i] >= 0x3A && hex[i] <= 0x40) ||
                (hex[i] >= 0x47 && hex[i] <= 0x60) || hex[i] >= 0x67)
            return false;
    }

    return true;
}

bool dec_verify(char * dec, int min, int max) {
    short int i = 1;

    while (dec[i] != '\0') {
        if (dec[i] < '0' || dec[i] > '9') return false;
        i++;
    }

    if ((dec[0] != '-') && (dec[0] < '0' || dec[0] > '9')) return false;

    long int val = atoi(dec);

    if (val < min || val > max) return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
