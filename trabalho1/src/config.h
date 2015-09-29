#include <stdio.h>

#ifndef DATA
    #include "data.h"

    #define MAP_SIZE 0x400
    #define WORD_SIZE 5 
    #define ROTULO_SIZE 64

    #define BYTE unsigned char

    #define NEW_TOKEN(place) \
        strtok(place, ignore)

    #define ALLOC_LINE(line) \
        line = calloc(WORD_SIZE, sizeof(BYTE));

    #define DATA
#endif

#ifndef GLOBAL
    #define GLOBAL
    extern char ignore[5];
    extern int file_line;
    extern Table rotulos;
    extern Table sets;
#endif

/////////////////////////////////////// ERROS //////////////////////////////////////////
void ERROR(char * msg); 
////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////// Auxiliar functions ///////////////////////////////////////
void test_eol(char * word);
bool make_line(BYTE ** line);
char * get_line(FILE * src, char * line, int * size);
bool label_verify(char * word);
bool SYM_verify(char * word);
bool hex_verify(char * hex);
bool dec_verify(char * dec, int min, int max);

///////////////////////////////////////////////////////////////////////////////////////
