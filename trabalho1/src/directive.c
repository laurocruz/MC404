#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "directive.h"
#include "instruction.h"


// .set = 0
// .org = 1
// .align = 2
// .wfill = 3
// .word = 4
char directives[5][7] =
{ 
    {'.', 's', 'e', 't', '\0', 0, 0},
    {'.', 'o', 'r', 'g', '\0', 0, 0},
    {'.', 'a', 'l', 'i', 'g', 'n', '\0'},
    {'.', 'w', 'f', 'i', 'l', 'l', '\0'},
    {'.', 'w', 'o', 'r', 'd', '\0', 0}
};

//////////////// Auxiliar functions ////////////////////////////////////

int directive_type(char * directive) {

    for (short int i = 0; i <= word; i++) {
        if (strcmp(directive, directives[i]) == 0)
            return i;
    }
    return -1; // diretiva não existe
}

void dec_into_map(BYTE * line, int dec) {
    int absdec = abs(dec);

    line[4] = absdec & 0xFF; 
    line[3] = (absdec >> 8) & 0xFF;
    line[2] = (absdec >> 16) & 0xFF;
    line[1] = (absdec >> 24) & 0xFF;
    line[0] = 0;

    // Two's complement
    if (dec < 0) {
        BYTE carry_in = 1;
        for (int i = WORD_SIZE-1; i >= 0 ; i--) {
            line[i] = ~line[i];
            if (carry_in == 1) {
                if (line[i] == 0xFF)
                    line[i] = 0;
                else {
                    line[i] += carry_in;
                    carry_in = 0;
                }
            }
        }
    }
}

BYTE test_char(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return 0;
}

void hex_to_map(BYTE * map_word, char * hex) {

    hex += 2;

    for (int i = 0; i < WORD_SIZE; i++, hex += 2) {
        map_word[i] = test_char(hex[0]) << 4 | test_char(hex[1]);
    }

}
///////////////////////////////////////////////////////////////////////////


/////////////////////////////// Directive functions //////////////////////////////
void set_dir(bool rot) {
    char * token;

    if (!rot) {
        token = NEW_TOKEN(NULL);

        test_eol(token);

        if (!SYM_verify(token)) {
            ERROR(".set: invalid SYM");
            // SYM com caracter inválido
        }

        if (instruction_type(token) != -1)
            ERROR(".set: SYM with same name of instruction");

        token = NEW_TOKEN(NULL);

        test_eol(token);

        if (!dec_verify(token, 0, 2147483647)) {
            if (hex_verify(token)) {
                long int parameter = strtol(token, NULL, 16);
                if (parameter < 0 || parameter > 2147483647) {
                    ERROR(".set: invalid parameter value");
                    // ERRO: Parâmetro inválido
                }
            } else {
                ERROR(".set: invalid parameter value");
                // ERRO: Parâmetro inválido
            }
        }

    } else {
        char * token2;
        int value;

        token = NEW_TOKEN(NULL);
        token2 = NEW_TOKEN(NULL);

        if (token2[1] == 'x') value = strtol(token2, NULL, 16);
        else value = atoi(token2);

        if (get_value(rotulos, token) != -1024) {
            ERROR(".set: label with same name already exists");
            // ERRO: Ja existe label com o nome desse SYM
        }
        if (!insert_instance(sets, token, value)) {
            ERROR(".set: SYM already exists");
            // ERRO: SYM ja existe
        }
    }
}

void org_dir(unsigned short int pos[], bool rot) {
    char * token;

    if (!rot) {
        token = NEW_TOKEN(NULL);

        test_eol(token);

        if (hex_verify(token)) {
            long int parameter = strtol(token, NULL, 16);
            if (parameter < 0 || parameter > 0x3FF) {
                ERROR(".org: invalid memory position");
                // ERRO: Posicao Inexistente
            } 
            pos[0] = (unsigned short int)parameter;
            pos[1] = 0;
        } else if (dec_verify(token, 0, 0x3FF)) {
            unsigned short int parameter = (unsigned short int)atoi(token);
            pos[0] = parameter;
            pos[1] = 0;
        } else {
            ERROR(".org: invalid parameter");
            // ERRO: Parametro invalido
        }
    } else {
        pos[1] = 0;

        token = NEW_TOKEN(NULL);

        if (token[1] == 'x') 
            pos[0] = (unsigned short int)strtol(token, NULL, 16); 
        else pos[0] = (unsigned short int)atoi(token);
    }
}

void align_dir(unsigned short int pos[], bool rot) {
    char * token;

    if (!rot) {
        token = NEW_TOKEN(NULL);

        test_eol(token);

        if (pos[0] >= 0x3FF) {
            ERROR(".align: invalid memory position");
            // ERRO: Ultrapassou limite da memoria
        }   

        if (dec_verify(token, 0, 0x3FF)) {
            unsigned short int parameter = (unsigned short int)atoi(token);

            if (pos[0] % parameter == 0) {
                if (pos[1] == 1) {
                    if (pos[0] + parameter > 0x3FF) {
                        ERROR(".align: invalid memory position");
                        // ERRO: Ultrapassou limite da memoria
                    }
                    pos[0] += parameter;
                    pos[1] = 0;
                }
            } else {
                if (pos[0] + parameter - (pos[0] % parameter) > 0x3FF) {
                    ERROR(".align: invalid memory position");
                    // ERRO: Ultrapassou limite de memoria
                }
                pos[0] += parameter - (pos[0] % parameter);
                pos[1] = 0;
            }
        } else {
            ERROR(".align: invalid parameter");
            // ERRO: Parametro invalido
        }

    } else {
        token = NEW_TOKEN(NULL);

        unsigned short int parameter = (unsigned short int)atoi(token);

        if (pos[0] % parameter == 0) {
            if (pos[1] == 1) {
                pos[0] += parameter;
                pos[1] = 0;
            }
        } else {
            pos[0] += parameter - (pos[0] % parameter);
            pos[1] = 0;
        }
    }
}


void word_dir(BYTE ** map, unsigned short int pos[], bool rot) {
    char * token;

    if (!rot) {

        if (pos[1] == 1) {
            ERROR(".word: word partially used");
            // ERRO: Tentando colocar word em palavra parcialmente usada
        } else if (pos[0] > 0x3FF) {
            ERROR(".word: not a valid memory position");
            // ERRO: Tentando utilizar espaco de memoria inexistente 
        }

        token = NEW_TOKEN(NULL);

        test_eol(token);


        if (!dec_verify(token, 0, 2147483647) && !label_verify(token) && 
            !SYM_verify(token) && !hex_verify(token)) {
            ERROR(".word: not a valid parameter");
            // ERRO: Parametro invalido
        }

        pos[0]++;

    } else {
        token = NEW_TOKEN(NULL);

        if (!make_line(&(map[pos[0]]))) {
            ERROR(".word: word already being used");
            // ERRO: Palavra de memoria ja sendo usada
        }

        if (dec_verify(token, 0, 2147483647)) {
            dec_into_map(map[pos[0]], atoi(token));

        } else if (hex_verify(token)) {
            hex_to_map(map[pos[0]], token);

        } else {
            int value;

            if (token[strlen(token)-1] == ':') {
                token[strlen(token)-1] = '\0';
                value = get_value(rotulos, token);
                if (value == -1024) {
                    ERROR(".word: label does not exist");
                    // ERRO: Rotulo ou SYM não existe
                }
                value = abs(value) % 1025;

            } else {
                value = get_value(sets, token);
                if (value == -1024) {
                    ERROR(".word: SYM does not exist");
                    // ERRO: Rotulo ou SYM não existe
                }

            }
            dec_into_map(map[pos[0]], value); 
        }
        pos[0]++;
    }
}

void wfill_dir(BYTE ** map, unsigned short int pos[], bool rot) {
    char * token;

    if (!rot) {

        if (pos[1] == 1) {
            ERROR(".wfill: first word already bein partially used");
            // ERRO: Tentando colocar word em palavra parcialmente usada
        } else if (pos[0] > 0x3FF) {
            ERROR(".wfill: not a valid memory position");
            // ERRO: Tentando utilizar espaco de memoria inexistente 
        }

        token = NEW_TOKEN(NULL);

        test_eol(token);

        if (!dec_verify(token, 1, 0x3FF)) {
            ERROR(".wfill: invalid 1st parameter");
            // ERRO: parametro invalido
        }

        int lines = atoi(token);

        if (pos[0] + lines > 0x400) {
            ERROR(".wfill: cant alloc all words: surpass memory limit");
            //ERRO: Tentando utilizar espaco de memoria inexistente
        }
        
        token = NEW_TOKEN(NULL);

        test_eol(token);

        if (!dec_verify(token, -2147483648, 2147483647) && !hex_verify(token) &&
            !label_verify(token) && !SYM_verify(token)) {
            ERROR(".wfill: invalid 2nd parameter");
            // ERRO: Parametro invalido
        }

        pos[0] += lines;

    } else {
        token = NEW_TOKEN(NULL);

        int lines = atoi(token);

        token = NEW_TOKEN(NULL);

        bool hex;
        int value;
        if (dec_verify(token, -2147483648, 2147483647)) {
            value = atoi(token);
            hex = false;
        } else if (hex_verify(token)) {
            hex = true;
        } else {
            if (token[strlen(token)-1] == ':') {
                token[strlen(token)-1] = '\0';

                value = get_value(rotulos, token);

                if (value == -1024) {
                    ERROR(".wfill: label does not exist");
                }
                value = abs(value) % 1025;
            } else {
                value = get_value(sets, token);

                if (value == -1024) {
                    ERROR(".wfill: SYM does not exist");
                }
            }
            hex = false;
        }


        if (hex) {
            for (int i = pos[0]; i < pos[0] + lines; i++) {
                if (make_line(&(map[i]))) {
                    hex_to_map(map[i], token);
                } else {
                    ERROR(".wfill: trying to use word that is already being used");
                    // ERRO: Palavra ja sendo usada
                }
            }
        } else {
             for (int i = pos[0]; i < pos[0] + lines; i++) {
                if (make_line(&(map[i]))) {
                    dec_into_map(map[i], value);
                } else {
                    ERROR(".wfill: trying to use word that is already being used");
                    // ERRO: Palavra ja sendo usada
                }
            }
           
        }
        pos[0] += lines;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////// Function called by main //////////////////////////////
void manage_directive(BYTE ** map, char * directive, unsigned short int pos[], bool rot) {
    short int type = directive_type(directive);
    
    switch (type) {
        case set:
            set_dir(rot);
            break;

        case org:
            org_dir(pos, rot);
            break;

        case align:
            align_dir(pos, rot);
            break;

        case wfill:
            wfill_dir(map, pos, rot);
            break;

        case word:
            word_dir(map, pos, rot);
            break;

        default:
            ERROR("directive: invalid directive");
            // ERRO: Diretiva não existe
    }
}

