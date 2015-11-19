#include "bico.h"

void _start(void) {
//    unsigned short a, b;
    int i;
/*
    while(1) {
        set_motors_speed(63, 63);
        for (i = 0; i < 10000000; i++); 
        set_motor_speed(0, 0);
        for (i = 0; i < 10000000; i++); 
        set_motors_speed(63, 0);
        for (i = 0; i < 10000000; i++); 

    }
    */
    while(1) set_motor_speed(0, 40);

}
