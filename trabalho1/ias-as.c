#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"

#define N_WORDS 0x400
#define WORD_SIZE 5
#define ROTULO_SIZE 64

#define unsigned char BYTE

#define NEW_TOKEN(place) \
    strtok(place, " \n")

#define MAKE_LINE(line) \
    line = calloc(WORD_SIZE, sizeof(BYTE));

#define IGNORE_LINE(file) \
    while (fgetc(file) != '\n');

typedef enum {false, true} Boolean;

static Table rotulos = HashInit(); // Armazena os rótulos
static Table sets = HashInit(); // Armazena os valores gerados por .set

char * buffer; // String auxiliar
char * token;

Boolean EOL;

Boolean comment;

//////////////////// Auxiliares para tratar strings/caracteres /////////////////////////

char * get_line(FILE * src, char * line, int * size) {
    char * indicator;
    indicator = fgets(line, *size, src);

    while (strlen(line) == (*size)-1 && line[strlen(line)-1] != '\n') {
        line = realloc(line, (*size) + 20);
        (*size) += 20;
        fgets(line + strlen(line), 21, src)
    }
    return indicator;
}

void inline get_word(FILE * src, char * buffer, char c) {
    buffer[0] = c;

    c = fgetc(src);

    short int i = 0;
    while (c != ' ' && c != '\n') {
        buffer[++i] = c;    
        c = fgetc(src);
    }
    if (c == '\n') EOL = true;
    else EOL = false;
    buffer[++i] = '\0';
}

char next_char(FILE * src) {
    char c;

    do { c = fgetc(src); } while (c == ' ');

    if (c == '\n') EOL = true;
    else EOL == false;


    return c;
}

Boolean valid_char(char c) {
    if ((c >= 0x30 && c <= 0x39) ||
            (c >= 0x41 && c <= 0x5A) ||
            (c >= 0x61 && c <= 0x7A) || (c == '_'))
        return true;

    return false;
}

Boolean comment_verify(char * word) {
    if (word[0] == '#') {
        comment = true;
        return true;
    }

    return false;
}

Boolean hex_verify(char * hex) {
    if (str(len) != 12) return false;

    if (buffer[0] != '0' || buffer[1] != 'x') return false;

    for (short int i = 2; i < 12; i++) {
        if (buffer[i] <= 0x2F || (buffer[i] >= 0x3A && buffer[i] <= 0x40) ||
                (buffer[i] >= 0x47 && buffer[i] <= 0x60) || buffer[i] >= 0x67)
            return false;
    }

    return true;
}

Boolean dec_verify(char * dec, int min, int max) {
    short int i = 1;

    while (buffer[i] != '\0') {
        if (buffer[i] < '0' || buffer[i] > '9') return false;
        i++;
    }

    if ((buffer[0] != '-') && (buffer[i] < '0' || buffer[i] > '9')) return false;

    long int val = atoi(dec);

    if (val < min || val > max) return false;

    return true;
}


////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////// Diretivas ///////////////////////////////////////
typedef enum {set, org, align, wfill, word} Directive;

// .set = 0
// .org = 1
// .align = 2
// .wfill = 3
// .word = 4

static char directives[5][7] =
{ 
    {'.', 's', 'e', 't', '\0', 0, 0},
    {'.', 'o', 'r', 'g', '\0', 0, 0},
    {'.', 'a', 'l', 'i', 'g', 'n', '\0'},
    {'.', 'w', 'f', 'i', 'l', 'l', '\0'}
    {'.', 'w', 'o', 'r', 'd', '\0', 0}
};

short int inline directive_type(char * directive) {

    for (short int i = 0; i <= word; i++) {
        if (strcmp(directive, directives[i]) == 0)
            return i;
    }
    return -1; // diretiva não existe
}

void set_dir(FILE * src, Boolean rot) {

    if (!rot) {

    } else {

    }
}

void org_dir(FILE * src, unsigned short int pos[], Boolean rot) {

    if (!rot) {
        token = NEW_TOKEN(NULL);

        if (token == NULL) {
            //ERRO: Nada apos .org
        } else if (comment_verify(token)) {
            //ERRO: Nada apos .org
        }

        if (hex_verify(token)) {
            long int parameter = strtol(token, NULL, 16);
            if (parameter < 0 || parameter > 0x3FF) {
                // ERRO: Posicao Inexistente
                return;
            } 
            pos[0] = (unsigned short int)parameter;
            pos[1] = 0;
        } else if (dec_verify(token, 0, 0x3FF)) {
            unsigned short int parameter = (unsigned short int)atoi(token);
            pos[0] = (unsigned short int)parameter;
            pos[1] = 0;
        } else {
            // ERRO: Parametro invalido
        }
    } else {
        pos[1] = 0;

        token = NEW_TOKEN(NULL);

        if (token[1] == 'x') 
            pos[0] = (unsigned short int)strtol(buffer, NULL, 16); 
        else pos[0] = (unsigned short int)atoi(buffer);
    }
}

void manage_directive(Directive type, FILE * src, unsigned short int pos[], Boolean rot) {
    switch (type) {
        case set:

            break;

        case org:
            org_dir(src, pos, rot);
            break;

        case align:
            break;

        case wfill:
            break;

        case word:
            break;

        default: 
            // Diretiva não existe
    }


}
////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////// Rotulos ////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////// Instrucoes /////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////

int main (int argc, char **argv) {
    FILE src = NULL;
    Boolean rotulos_feitos = false;
    BYTE * map[N_WORDS];

    src = fopen(argv[1], "r");

    if (src == NULL) {} // Deal with it

    int buf_size = 128;
    buffer = malloc(buf_size * sizeof(char));


    //pos[0] = line
    //pos[1] = side (0 == left | 1 == right)
    unsigned short int pos[2] = {0,0};


    while (get_line(src, buffer, &buf_size) != EOF) {
        token = strtok(buffer, " \n");

        if (token != NULL || token[0] == '#') {
            if (token[0] == '.') { // Directive
                manage_directive(directive_type(token), src, pos, false);
            }
            else if (token[strlen(token)-1] == ':') { // Rotulo

            }
            else { // Instruction
                // Erros de coisas aleatórias serão tradatas dentro de Instruction
            }
        }
    }
}

return 0;
}

