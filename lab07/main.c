// Lauro Cruz e Souza - RA: 156175
// laurocruzsouza@gmail.com | lauro.souza@students.ic.unicamp.br

#include "api_robot.h" /* Robot control API */

/* main function */
void _start(void) {
    unsigned char stop;
    unsigned int distances[16];

    // super simple track to exercise all the control functions
    while (1) {

        stop = 0;
        while(!stop) {
            if (read_sonar(3) > 1500 & read_sonar(4) > 1500) {
                set_speed_motors(40, 40);
                stop = 1;
            }
        }

        stop = 0;
        while (!stop) {
            if (read_sonar(3) < 1500 || read_sonar(4) < 1500) {
                set_speed_motors(2, 2);
                stop = 1;
            }
        }

        read_sonars(distances);

        if ((distances[7] + distances[8]) < (distances[0] + distances[15]))
            set_speed_motor(15,0);
        else set_speed_motor(15,1);

    }
}
