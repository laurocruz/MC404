// Lauro Cruz e Souza - RA: 156175
// laurocruzsouza@gmail.com | lauro.souza@students.ic.unicamp.br

#include "bico.h" /* Robot control API */

/* main function */
void _start(void) {
    unsigned char stop;
    unsigned int distances[16];
    unsigned short a, b;

    // super simple track to exercise all the control functions
    while (1) {

        stop = 0;
        while(!stop) {
            read_sonar(3, &a); read_sonar(4, &b);
            if (a > 1500 && b > 1500) {
                set_motors_speed(40, 40);
                stop = 1;
            }
        }

        stop = 0;
        while (!stop) {
            read_sonar(3, &a); read_sonar(4, &b);
            if (a < 1500 || b < 1500) {
                set_motors_speed(2, 2);
                stop = 1;
            }
        }

        read_sonars(distances);

        if ((distances[7] + distances[8]) < (distances[0] + distances[15]))
            set_motor_speed(0,15);
        else set_motor_speed(1,15);

    }
}
