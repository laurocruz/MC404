#include "api_robot.h" /* Robot control API */

void delay();

/* main function */
void _start(void) 
{
  unsigned int distances[16];

  /* While not close to anything. */
  do {
    set_speed_motors(25,25);
    delay();
    set_speed_motor(10,0);
    delay();
    set_speed_motors(25,10);
    delay();
    read_sonars(distances);
  } while ( ( distances[4] > 1200 ) && ( distances[3] > 1200 ));
}

/* Spend some time doing nothing. */
void delay()
{
  int i;
  /* Not the best way to delay */
  for(i = 0; i < 10000; i++ );  
}
