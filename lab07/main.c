// Lauro Cruz e Souza - RA: 156175
// laurocruzsouza@gmail.com | lauro.souza@students.ic.unicamp.br

#include "api_robot.h" /* Robot control API */

void delay();

/* main function */
void _start(void) {

    unsigned int distances[16];

    while (1) {
        if (read_sonar(3) > 1200 && read_sonar(4) > 1200)
            set_speed_motors(40, 40);
        else
            set_speed_motors(15,0);
    }

    /*
    do {

        set_speed_motors(40, 40);
        do {
        } while (read_sonar(3) > 1500 && read_sonar(4) > 1500);

        set_speed_motors(0,15);
        do {
        } while (read_sonar(3) > 1500 && read_sonar(4) > 1500);

    } while(1);
    */
}

/* Spend some time doing nothing. */
void delay()
{
  int i;
  /* Not the best way to delay */
  for(i = 0; i < 10000; i++ );  
}
