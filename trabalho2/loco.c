#include "bico.h"

void _start(void) {
    unsigned short a = 1010, b;
    unsigned int i;

    while(a > 1000) {
        set_motors_speed(63, 63);
        read_sonar(3, &a);
    }
    set_motors_speed(0, 0);

}
