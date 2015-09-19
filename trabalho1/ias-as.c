#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "data.h"

#define MAP_SIZE 0x400
#define WORD_SIZE 5 
#define ROTULO_SIZE 64

#define BYTE unsigned char

#define NEW_TOKEN(place) \
    strtok(place, ignore)

#define ALLOC_LINE(line) \
    line = calloc(WORD_SIZE, sizeof(BYTE));

static Table rotulos; // Armazena os rótulos
static Table sets; // Armazena os valores gerados por .set

static char ignore[5] = {' ', '\n', '\r', 9, '\0'};
char * buffer; // String auxiliar
char * token;

int current;

/////////////////////////////////////// ERROS //////////////////////////////////////////
void wrong(char * msg) {
    fprintf(stderr, "ERROR on line %d\n", current);
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

////////////////////////////////////////////////////////////////////////////////////////

//////////////////// Auxiliares para tratar strings/caracteres /////////////////////////

void test_eol(char * word) {
    if (word == NULL) {
        //ERRO: Falta parâmetro
        wrong("missing parameter");
    } else if (word[0] == '#') {
        //ERRO: Falta parâmetro
        wrong("missing parameter");
    }  
}

Boolean make_line(BYTE ** line) {
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

Boolean label_verify(char * word) {

    if (word[strlen(word)-1] != ':') return false;

    for (int i = 0; i < strlen(word)-1; i++) {
        if (word[i] < '0' || (word[i] > '9' && word[i] < 'A') ||
            (word[i] > 'Z' && word[i] < '_') || word[i] == '`' ||
            word[i] > 'z')
            return false;
    }

    return true;
}

Boolean SYM_verify(char * word) {
    for (int i = 0; i < strlen(word)-1; i++) {
        if (word[i] < '0' || (word[i] > '9' && word[i] < 'A') ||
            (word[i] > 'Z' && word[i] < '_') || word[i] == '`' ||
            word[i] > 'z')
            return false;
    }

    return true;
}

Boolean hex_verify(char * hex) {
    if (strlen(hex) != 12) return false;

    if (hex[0] != '0' || hex[1] != 'x') return false;

    for (short int i = 2; i < 12; i++) {
        if (hex[i] <= 0x2F || (hex[i] >= 0x3A && hex[i] <= 0x40) ||
                (hex[i] >= 0x47 && hex[i] <= 0x60) || hex[i] >= 0x67)
            return false;
    }

    return true;
}

Boolean dec_verify(char * dec, int min, int max) {
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


////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////// Diretivas ///////////////////////////////////////
typedef enum {set, org, align, wfill, word} Directive;

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

short int directive_type(char * directive) {

    for (short int i = 0; i <= word; i++) {
        if (strcmp(directive, directives[i]) == 0)
            return i;
    }
    return -1; // diretiva não existe
}

void set_dir(Boolean rot) {

    if (!rot) {
        token = NEW_TOKEN(NULL);

        test_eol(token);

        if (!SYM_verify(token)) {
            wrong(".set with invalid SYM");
            // SYM com caracter inválido
        }

        token = NEW_TOKEN(NULL);

        test_eol(token);

        if (!dec_verify(token, 0, 2147483647)) {
            if (hex_verify(token)) {
                long int parameter = strtol(token, NULL, 16);
                if (parameter < 0 || parameter > 2147483647) {
                    wrong(".set with invalid parameter value");
                    // ERRO: Parâmetro inválido
                }
            } else {
                wrong(".set with invalid parameter value");
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
            wrong("already exists label with .set SYM name");
            // ERRO: Ja existe label com o nome desse SYM
        }

        if (!insert_instance(sets, token, value)) {
            wrong("already exists SYM with .set SYM name");
            // ERRO: SYM ja existe
        }
    }
}

void org_dir(unsigned short int pos[], Boolean rot) {

    if (!rot) {
        token = NEW_TOKEN(NULL);

        test_eol(token);

        if (hex_verify(token)) {
            long int parameter = strtol(token, NULL, 16);
            if (parameter < 0 || parameter > 0x3FF) {
                wrong("not a valid memory position on .org");
                // ERRO: Posicao Inexistente
            } 
            pos[0] = (unsigned short int)parameter;
            pos[1] = 0;
        } else if (dec_verify(token, 0, 0x3FF)) {
            unsigned short int parameter = (unsigned short int)atoi(token);
            pos[0] = parameter;
            pos[1] = 0;
        } else {
            wrong("invalid parameter");
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

void align_dir(unsigned short int pos[], Boolean rot) {

    if (!rot) {
        token = NEW_TOKEN(NULL);

        test_eol(token);

        if (pos[0] >= 0x3FF) {
            wrong("surpassed memory size");
            // ERRO: Ultrapassou limite da memoria
        }   

        if (dec_verify(token, 0, 0x3FF)) {
            unsigned short int parameter = (unsigned short int)atoi(token);

            if ((pos[0] % parameter == 0) && (pos[1] = 1)) {
                if (pos[0] + parameter > 0x3FF) {
                    wrong("surpassed memory size");
                    // ERRO: Ultrapassou limite da memoria
                }
                pos[0] += parameter;
                pos[1] = 0;
            } else {
                if (pos[0] + parameter - (pos[0] % parameter) > 0x3FF) {
                    wrong("surpassed memory size");
                    // ERRO: Ultrapassou limite de memoria
                }
                pos[0] += parameter - (pos[0] % parameter);
                pos[1] = 0;
            }
        } else {
            wrong("invalid parameter for .align");
            // ERRO: Parametro invalido
        }

    } else {
        token = NEW_TOKEN(NULL);

        unsigned short int parameter = (unsigned short int)atoi(token);

        if ((pos[0] % parameter == 0) && (pos[1] == 1)) {
            pos[0] += parameter;
            pos[1] = 0;
        } else {
            pos[0] += parameter - (pos[0] % parameter);
            pos[1] = 0;
        }
    }
}

/*


   void dec_into_map(unsigned int dec, char * line) {

   line[WORD_SIZE] = '\0';
   line[WORD_SIZE-1] = get_hex_char(dec & 0xF);
   line[WORD_SIZE-2] = get_hex_char((dec & 0xF0) >> 4);
   line[WORD_SIZE-3] = get_hex_char((dec & 0xF00) >> 8);
   line[WORD_SIZE-4] = get_hex_char((dec & 0xF000) >> 12);
   line[WORD_SIZE-5] = get_hex_char((dec & 0xF0000) >> 16);
   line[WORD_SIZE-6] = get_hex_char((dec & 0xF00000) >> 20);
   line[WORD_SIZE-7] = get_hex_char((dec & 0xF000000) >> 24);
   line[WORD_SIZE-8] = get_hex_char((dec & 0xF0000000) >> 28);
   line[1] = line[0] = '0';
   }

*/

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

void word_dir(BYTE ** map, unsigned short int pos[], Boolean rot) {

    if (!rot) {

        if (pos[1] == 1) {
            wrong(".word: word partially used");
            // ERRO: Tentando colocar word em palavra parcialmente usada
        } else if (pos[0] > 0x3FF) {
            wrong(".word: not a valid memory position");
            // ERRO: Tentando utilizar espaco de memoria inexistente 
        }

        token = NEW_TOKEN(NULL);

        test_eol(token);


        if (!dec_verify(token, 0, 2147483647) && !label_verify(token) && 
            !SYM_verify(token) && !hex_verify(token)) {
            wrong(".word: not a valid parameter");
            // ERRO: Parametro invalido
        }

        pos[0]++;

    } else {
        token = NEW_TOKEN(NULL);

        if (!make_line(&(map[pos[0]]))) {
            wrong(".word: word already being used");
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
                    wrong(".word: label does not exist");
                    // ERRO: Rotulo ou SYM não existe
                }
                value = abs(value);

            } else {
                value = get_value(sets, token);
                if (value == -1024) {
                    wrong(".word: SYM does not exist");
                    // ERRO: Rotulo ou SYM não existe
                }

            }
            dec_into_map(map[pos[0]], value); 
        }
        pos[0]++;
    }
}

void wfill_dir(BYTE ** map, unsigned short int pos[], Boolean rot) {

    if (!rot) {

        if (pos[1] == 1) {
            wrong(".wfill: first word already bein partially used");
            // ERRO: Tentando colocar word em palavra parcialmente usada
        } else if (pos[0] > 0x3FF) {
            wrong(".wfill: not a valid memory position");
            // ERRO: Tentando utilizar espaco de memoria inexistente 
        }

        token = NEW_TOKEN(NULL);

        test_eol(token);

        if (!dec_verify(token, 1, 0x3FF)) {
            wrong(".wfill: invalid 1st parameter");
            // ERRO: parametro invalido
        }

        int lines = atoi(token);

        if (pos[0] + lines > 0x400) {
            wrong(".wfill: cant alloc all words: surpass memory limit");
            //ERRO: Tentando utilizar espaco de memoria inexistente
        }
        
        token = NEW_TOKEN(NULL);

        test_eol(token);

        if (!dec_verify(token, -2147483648, 2147483647) && !hex_verify(token) &&
            !label_verify(token) && !SYM_verify(token)) {
            wrong(".wfill: invalid 2nd parameter");
            // ERRO: Parametro invalido
        }

        pos[0] += lines;

    } else {
        token = NEW_TOKEN(NULL);

        int lines = atoi(token);

        token = NEW_TOKEN(NULL);

        Boolean hex;
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
                    wrong(".wfill: label does not exist");
                }
                value = abs(value);
            } else {
                value = get_value(sets, token);

                if (value == -1024) {
                    wrong(".wfill: SYM does not exist");
                }
            }
            hex = false;
        }


        if (hex) {
            for (int i = pos[0]; i < pos[0] + lines; i++) {
                if (make_line(&(map[i]))) {
                    hex_to_map(map[i], token);
                } else {
                    wrong(".wfill: trying to use word that is already being used");
                    // ERRO: Palavra ja sendo usada
                }
            }
        } else {
             for (int i = pos[0]; i < pos[0] + lines; i++) {
                if (make_line(&(map[i]))) {
                    dec_into_map(map[i], value);
                } else {
                    wrong(".wfill: trying to use word that is already being used");
                    // ERRO: Palavra ja sendo usada
                }
            }
           
        }
        pos[0] += lines;
    }
}

void manage_directive(Directive type, BYTE ** map, unsigned short int pos[], Boolean rot) {
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
            wrong("directive: invalid directive");
            // ERRO: Diretiva não existe
    }
}
////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////// Rotulos ////////////////////////////////////////////

void manage_label(char * label, unsigned short int pos[]) {

    if (!label_verify(label)) {
        wrong("label: invalid label name");
        // ERRO: Nome invalido de rotulo
    }

    int value;

    if (pos[1] == 0) value = pos[0];
    else value = -pos[0];

    label[strlen(label)-1] = '\0';

    if (!insert_instance(rotulos, label, value)) {
        wrong("label: label already exists");
        // ERRO: rotulo ja existe
    }
}

////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////// Instrucoes /////////////////////////////////////////

char instructions[17][8] = 
{   {'L', 'S', 'H', '\0', 0, 0, 0, 0},
    {'R', 'S', 'H', '\0', 0, 0, 0, 0},
    {'J', 'M', 'P', '\0', 0, 0, 0, 0},
    {'J', 'U', 'M', 'P', '+', '\0', 0, 0},
    {'S', 'T', 'a', 'd','d','r', '\0', 0},
    {'L', 'D', '\0', 0, 0, 0, 0, 0},
    {'L', 'D', '-', '\0', 0, 0, 0, 0},
    {'L', 'D', '|', '\0', 0, 0, 0, 0},
    {'L', 'D', 'm', 'q', '\0', 0, 0, 0},
    {'L', 'D', 'm', 'q', '_', 'm', 'x', '\0'},
    {'S', 'T', '\0', 0, 0, 0, 0, 0},
    {'A', 'D', 'D', '\0', 0, 0, 0, 0},
    {'A', 'D', 'D', '|', '\0', 0, 0, 0},
    {'S', 'U', 'B', '\0', 0, 0, 0, 0},
    {'S', 'U', 'B', '|','\0', 0, 0, 0},
    {'M', 'U', 'L', '\0', 0, 0, 0, 0},
    {'D', 'I', 'V', '\0', 0, 0, 0, 0}
};

static BYTE i_code [17] = {0x14, 0x15, 0x0D, 0x0F, 0x12, 0x01, 0x02, 0x03, 
    0x0A, 0x09, 0x21, 0x05, 0x07, 0x06, 0x08, 0x0B, 0x0C};

int instruction_type(char * inst) {
    for (int i = 0; i < 17; i++) {
        if (strcmp(inst, instructions[i]) == 0)
            return i;
    }
    return -1;
}

void encode_instruction(BYTE * mem_word, BYTE code, int param, Boolean right) {

    if (!right) {
        mem_word[0] = code;

        mem_word[1] = param >> 4;
        mem_word[2] |= (param & 0x0F) << 4;
            ///////////////////////
    } else {
        mem_word[2] |= code >> 4;

        mem_word[3] = (code << 4) | (param >> 8);

        mem_word[4] = param & 0xFF;
        ////////////////////////
    }
}

void lsh_rsh(BYTE ** map, int type, unsigned short int pos[], Boolean rot) {

    if (!rot) {
        if (pos[0] > 0x3FF) {
            wrong("lsh/rsh: invalid memory position");
            // ERRO: Tentando escrever em posicao inexistente
        }

        if (pos[1] == 0) pos[1]++;
        else {
            pos[0]++;
            pos[1] = 0;
        }

    } else {

        if (pos[1] == 0) {
            if (make_line(&(map[pos[0]]))) {
                encode_instruction(map[pos[0]], i_code[type], 0, false);
            } else {
                wrong("lsh/rsh: memory word already being used");
                // ERRO: Palavra de memoria ja sendo usada
            }
            pos[1]++;
        } else {
            if (!make_line(&(map[pos[0]]))) {
                encode_instruction(map[pos[0]], i_code[type], 0, true);
            } else {
                wrong("lsh/rsh: wrong sincronization");
                // Problema de sincronizacao
            }
            pos[0]++;
            pos[1] = 0;
        }
    }
}

void one_param_ins(BYTE ** map, int type, unsigned short int pos[], Boolean rot) {

    if (!rot) {
        token = NEW_TOKEN(NULL);

        if (pos[0] > 0x3FF) {
            wrong("one_param: invalid memory position");
            // ERRO: Tentando escrever em posicao inexistente
        }

        test_eol(token);

        if (token[0] != '"' || token[strlen(token)-1] != '"') {
            wrong("one_param: parameter without aspas");
            // ERRO: Parametro de instrucao passado sem aspas
        }
        token[strlen(token)-1] = '\0';
        token++;

        if (!dec_verify(token, 0, 0x3FF) && !SYM_verify(token)) {
            if (hex_verify(token)) {
                long int value = strtol(token, NULL, 16);
                if (value < 0 || value > 0x3FF) {
                    wrong("one_param: invalid parameter value");
                    // ERRO: Valor excedido
                }
            } else {
                wrong("one_param: invalid parameter");
                // ERRO: Parametro invalido   
            }
        }

        if (pos[1] == 0) pos[1]++;
        else {
            pos[0]++;
            pos[1] = 0;
        }

    } else {
        token = NEW_TOKEN(NULL);

        token[strlen(token)-1] = '\0';
        token++;

        int param;

        if (dec_verify(token, 0, 0x3FF)) {
            param = atoi(token);
        } else if (hex_verify(token)) {
            param = strtol(token, NULL, 16);
        } else {
            param = get_value(rotulos, token);
            if (param == -1024) {
                wrong("one_param: label does not exist");
                // ERRO: rotulo nao existe
            }
        }

        if (pos[1] == 0) {
            if(make_line(&(map[pos[0]]))) {
                if (type >= 2 && type <= 4) { // Se é JMP, JUMP+ ou STaddr
                    if (param >= 0)
                        encode_instruction(map[pos[0]], i_code[type], param, false);
                    else 
                        encode_instruction(map[pos[0]], i_code[type]+1, (-1)*param,false);
                } else {
                    encode_instruction(map[pos[0]], i_code[type], abs(param), false);
                }
            } else {
                wrong("one_param: memory word already being used");
                // ERRO: palavra de memoria ja sendo usada
            }
            pos[1]++;

        } else {
            if (!make_line(&(map[pos[0]]))) {
                if (type >= 2 && type <= 4) { // Se é JMP, JUMP+ ou Staddr
                    if (param >= 0)
                        encode_instruction(map[pos[0]], i_code[type], param, true);
                    else 
                        encode_instruction(map[pos[0]], i_code[type]+1, (-1)*param, true);
                } else {
                    encode_instruction(map[pos[0]], i_code[type], abs(param), true);
                }
            } else {
                wrong("one_param: SIncronization error");
                // Falha na sincronizacao
            }
            pos[0]++;
            pos[1] = 0;
        }
    }
}

void manage_instruction(int type, BYTE ** map, unsigned short int pos[], Boolean rot) {

    if (type == 0 || type == 1) { // Instrucoes sem parametros
        lsh_rsh(map, type, pos, rot);

    } else if (type >= 2) { //Instrucoes de um parametro
        one_param_ins(map, type, pos, rot);

    } else {
        wrong("Invalid instruction");
        // ERRO: Instrucao nao existe
    }
}

////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////// Impressao //////////////////////////////////////////////
//
char get_hex_char(unsigned char four_bits) {
    if (four_bits <= 9) return (char)(four_bits + '0');
    else return (char)(four_bits - 10 + 'A');
}

void make_address(char * add, int i) {
    add[0] = get_hex_char(i >> 8);
    add[1] = get_hex_char((i >> 4) & 0x0F);
    add[2] = get_hex_char(i & 0x0F);
}

void make_word(char * hex_word, BYTE * mem_line) {
    hex_word[0] = get_hex_char(mem_line[0] >> 4);
    hex_word[1] = get_hex_char(mem_line[0] & 0x0F);
    hex_word[3] = get_hex_char(mem_line[1] >> 4);
    hex_word[4] = get_hex_char(mem_line[1] & 0x0F);
    hex_word[5] = get_hex_char(mem_line[2] >> 4);
    hex_word[7] = get_hex_char(mem_line[2] & 0x0F);
    hex_word[8] = get_hex_char(mem_line[3] >> 4);
    hex_word[10] = get_hex_char(mem_line[3] & 0x0F);
    hex_word[11] = get_hex_char(mem_line[4] >> 4);
    hex_word[12] = get_hex_char(mem_line[4] & 0x0F);
}

void print_map(FILE * output, BYTE ** map) {

    char address[4], hex_word[14];
    address[3] = hex_word[13] = '\0';
    hex_word[2] = hex_word[6] = hex_word[9] = ' ';

    for (int i = 0; i < MAP_SIZE; i++) {
        if (map[i] != NULL) {
            make_address(address, i);
            make_word(hex_word, map[i]);
            fprintf(output, "%s %s\n", address, hex_word);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////

int main (int argc, char **argv) {
    FILE * src = NULL;
    BYTE * map[MAP_SIZE];

    for (int i = 0; i < MAP_SIZE; i++)
        map[i] = NULL;

    Boolean dir, lab, ins;

    src = fopen(argv[1], "r");

    if (src == NULL) {
        wrong("main: cant open src file");
        // ERRO: Arquivo de entrada nao existe
    }

    int buf_size = 128;
    buffer = malloc(buf_size * sizeof(char));


    //pos[0] = line
    //pos[1] = side (0 == left | 1 == right)
    unsigned short int pos[2] = {0,0};

    rotulos = HashInit();

    current = 0;
    while (get_line(src, buffer, &buf_size) != NULL) {
        current++;
        dir = lab = ins = false;
        token = NEW_TOKEN(buffer);

        while (token != NULL && token[0] != '#') {
            if ((token[0] == '.') && !ins && !dir) { // Directive
                manage_directive(directive_type(token), map, pos, false);
                dir = true;
            }
            else if ((token[strlen(token)-1] == ':') && !dir && !ins && !lab) { // Rotulo
                manage_label(token, pos);
                lab = true;
            }
            else if (!dir && !ins) { // Instruction
                manage_instruction(instruction_type(token), map, pos, false);
                ins = true;
            }
            else {
                wrong("invalid construction of line");
                // ERRO
            }
            token = NEW_TOKEN(NULL);
        }
    }

    sets = HashInit();

    src = freopen(argv[1], "r", src);
    
    pos[0] = pos[1] = 0;
    current = 0;

    while(fgets(buffer, buf_size, src) != NULL) {
        current++;
        token = NEW_TOKEN(buffer);

        while (token != NULL && token[0] != '#') {
            if(token[0] == '.') {
                manage_directive(directive_type(token), map, pos, true);
            }
            else if (token[strlen(token)-1] != ':') {
                manage_instruction(instruction_type(token), map, pos, true);
            }
            token = NEW_TOKEN(NULL);
        }
    }

    if (argc == 3) {
        FILE * output = fopen(argv[2], "w");

        if (output == NULL) {
            wrong("main: cant open output file");
            // ERRO: 
        }

        print_map(output, map);
    } else {
        print_map(stdout, map);
    }
    return 0;
}


