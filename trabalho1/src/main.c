#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "directive.h"
#include "instruction.h"
#include "label.h"

/////////////////////////////// Impressao //////////////////////////////////////////////

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
    char * buffer;
    char * token;

    for (int i = 0; i < MAP_SIZE; i++)
        map[i] = NULL;

    bool dir, lab, ins;

    src = fopen(argv[1], "r");

    if (src == NULL) {
        ERROR("main: cant open src file");
        // ERRO: Arquivo de entrada nao existe
    }

    int buf_size = 128;
    buffer = malloc(buf_size * sizeof(char));


    //pos[0] = line
    //pos[1] = side (0 == left | 1 == right)
    unsigned short int pos[2] = {0,0};

    rotulos = HashInit();

    file_line = 0;
    while (get_line(src, buffer, &buf_size) != NULL) {
        file_line++;
        dir = lab = ins = false;
        token = NEW_TOKEN(buffer);

        while (token != NULL && token[0] != '#') {
            if ((token[0] == '.') && !ins && !dir) { // Directive
                manage_directive(map, token, pos, false);
                dir = true;
            }
            else if ((token[strlen(token)-1] == ':') && !dir && !ins && !lab) { // Rotulo
                manage_label(token, pos);
                lab = true;
            }
            else if (!dir && !ins) { // Instruction
                manage_instruction(map, token, pos, false);
                ins = true;
            }
            else {
                ERROR("invalid construction of line");
                // ERRO
            }
            token = NEW_TOKEN(NULL);
        }
    }

    sets = HashInit();

    src = freopen(argv[1], "r", src);
    
    pos[0] = pos[1] = 0;
    file_line = 0;

    while(fgets(buffer, buf_size, src) != NULL) {
        file_line++;
        token = NEW_TOKEN(buffer);

        while (token != NULL && token[0] != '#') {
            if(token[0] == '.') {
                manage_directive(map, token, pos, true);
            }
            else if (token[strlen(token)-1] != ':') {
                manage_instruction(map, token, pos, true);
            }
            token = NEW_TOKEN(NULL);
        }
    }

    fclose(src);
    free(buffer);

    if (argc == 3) {
        FILE * output = fopen(argv[2], "w");

        if (output == NULL) {
            ERROR("main: cant open output file");
            // ERRO: 
        }

        print_map(output, map);

        fclose(output);
    } else {
        print_map(stdout, map);
    }
    
    for (int i = 0; i < MAP_SIZE; i++) {
        free(map[i]);
    }

    free_table(rotulos);
    free_table(sets);

    return 0;
}

