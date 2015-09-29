#include <string.h>
#include <stdlib.h>

#include "instruction.h"

//////////////////////////////////////////////////////////////////////////////////////
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

BYTE i_code [17] = {0x14, 0x15, 0x0D, 0x0F, 0x12, 0x01, 0x02, 0x03, 
    0x0A, 0x09, 0x21, 0x05, 0x07, 0x06, 0x08, 0x0B, 0x0C};
//////////////////////////////////////////////////////////////////////////////////////


//////////////////////// Auxiliar functions //////////////////////////////////////////
int instruction_type(char * inst) {
    for (int i = 0; i < 17; i++) {
        if (strcmp(inst, instructions[i]) == 0)
            return i;
    }
    return -1;
}

void encode_instruction(BYTE * mem_word, BYTE code, int param, bool right) {

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
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////// Instructions ////////////////////////////////////
void lsh_rsh(BYTE ** map, int type, unsigned short int pos[], bool rot) {

    if (!rot) {
        if (pos[0] > 0x3FF) {
            ERROR("instruction: invalid memory position");
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
                ERROR("instruction: memory word already being used");
                // ERRO: Palavra de memoria ja sendo usada
            }
            pos[1]++;
        } else {
            if (!make_line(&(map[pos[0]]))) {
                encode_instruction(map[pos[0]], i_code[type], 0, true);
            } else {
                ERROR("instruction: ERROR sincronization");
                // Problema de sincronizacao
            }
            pos[0]++;
            pos[1] = 0;
        }
    }
}

void one_param_ins(BYTE ** map, int type, unsigned short int pos[], bool rot) {
    char * token;

    if (!rot) {
        token = NEW_TOKEN(NULL);

        if (pos[0] > 0x3FF) {
            ERROR("instruction: invalid memory position");
            // ERRO: Tentando escrever em posicao inexistente
        }

        test_eol(token);

        if (token[0] != '"' || token[strlen(token)-1] != '"') {
            ERROR("instruction: parameter without aspas");
            // ERRO: Parametro de instrucao passado sem aspas
        }
        token[strlen(token)-1] = '\0';
        token++;

        if (!dec_verify(token, 0, 0x3FF) && !SYM_verify(token)) {
            if (hex_verify(token)) {
                long int value = strtol(token, NULL, 16);
                if (value < 0)
                    ERROR("instruction: invalid parameter value");
                else if (value > 0x3FF) {
                    if (value != 0x400)
                        ERROR("instruction: invalid parameter value");
                    else if (type != 2 && type != 3)
                        ERROR("instruction: invalid parameter value");
                }
            } else {
                ERROR("instruction: invalid parameter");
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
                ERROR("instruction: label does not exist");
                // ERRO: rotulo nao existe
            }
        }

        if (pos[1] == 0) {
            if(make_line(&(map[pos[0]]))) {
                if (type >= 2 && type <= 4) { // Se é JMP, JUMP+ ou STaddr
                    if (param >= 0)
                        encode_instruction(map[pos[0]], i_code[type], param, false);
                    else {
                        param = ((-1)*param) % 1025;
                        encode_instruction(map[pos[0]], i_code[type]+1, param, false);
                    }
                } else {
                    encode_instruction(map[pos[0]], i_code[type], abs(param) % 1025, false);
                }
            } else {
                ERROR("instruction: memory word already being used");
                // ERRO: palavra de memoria ja sendo usada
            }
            pos[1]++;

        } else {
            if (!make_line(&(map[pos[0]]))) {
                if (type >= 2 && type <= 4) { // Se é JMP, JUMP+ ou Staddr
                    if (param >= 0)
                        encode_instruction(map[pos[0]], i_code[type], param, true);
                    else {
                        param = ((-1)*param) % 1025;
                        encode_instruction(map[pos[0]], i_code[type]+1, param, true);
                    }
                } else {
                    encode_instruction(map[pos[0]], i_code[type], abs(param) % 1025, true);
                }
            } else {
                ERROR("instruction: SIncronization error");
                // Falha na sincronizacao
            }
            pos[0]++;
            pos[1] = 0;
        }
    }
}



/////////////////////////////////// Function called by main ///////////////////////////

void manage_instruction(BYTE ** map, char * op, unsigned short int pos[], bool rot) {

    short int type = instruction_type(op);

    if (type == 0 || type == 1) { // Instrucoes sem parametros
        lsh_rsh(map, type, pos, rot);

    } else if (type >= 2) { //Instrucoes de um parametro
        one_param_ins(map, type, pos, rot);

    } else {
        ERROR("Invalid instruction");
        // ERRO: Instrucao nao existe
    }
}

////////////////////////////////////////////////////////////////////////////////////////


