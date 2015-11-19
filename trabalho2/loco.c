#include "bico.h"

void _start(void) {
    unsigned short a, b;
    unsigned int i;

    while (1) {
        set_motors_speed(40, 40);
        for (i = 0; i < 100000; i++);
        read_sonar(3, &a);
        read_sonar(4, &b);
    }

}
