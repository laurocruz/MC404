#include "config.h"

typedef enum {set, org, align, wfill, word} Directive;

void manage_directive(BYTE ** map, char * directive, unsigned short int pos[], bool rot);

