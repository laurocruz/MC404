#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rotulos"

#define N_WORDS 0x400
#define WORD_SIZE 5
#define ROTULO_SIZE 64

#define unsigned char BYTE

#define MAKE_LINE(line) \
    line = calloc(WORD_SIZE, sizeof(BYTE));

#define IGNORE_LINE(file) \
    while (fgetc(file) != '\n');

static Base rotulos = HashInit();
static Base sets = HashInit();

//////////////////// Auxiliares para tratar strings/caracteres /////////////////////////
char next_char(FILE src) {
    char c;

    do { c = fgetc(src); } while (c != ' ');

    return c;
}

Boolean valid_char(char c) {
    if ((c >= 0x30 && c <= 0x39) ||
        (c >= 0x41 && c <= 0x5A) ||
        (c >= 0x61 && c <= 0x7A) || (c == '_'))
        return true;

    return false;
}


void inline get_word(FILE src, char * str, char c) {
    src[0] = c;

    c = fgetc(src);

    short int i = 0;
    while (c != ' ') {
        src[++i] = c;    
        c = fgetc(src);
    }
    src[++i] = '\0';
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

void manage_directive(Directive type, unsigned short int pos[], Boolean rot) {
    switch (type) {
        case set:
            break;
        
        case org:
            break;

        case align:
            break;

        case wfill:
            break;

        case word:
            break;
    }


}
////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////// Rotulos ///////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////// Instrucoes /////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////

int main (int argc, char **argv) {
    FILE src = NULL;
    char str[ROTULO_SIZE+1];
    Boolean rotulos_feitos = false;

    BYTE * map[N_WORDS];

    src = fopen(argv[1], "r");

    if (src == NULL) {} // Deal with it

    char c = next_char(src);

    //pos[0] = line
    //pos[1] = side (0 == left | 1 == right)
    unsigned short int pos[2] = {0,0};

    while (c != EOF) { // file
        if (c == '#') IGNORE_LINE(src); // Comment
        else{
            get_word(src, str, c);
            
            if (str[0] == '.') { // Directive
                manage_directive (directive_type(str), pos, false);
            }
            else if (str[strlen(str)] == ':') { // Rotulo
            
            }
            else { // Instruction

            }
        }
        
    }

    return 0;
}
