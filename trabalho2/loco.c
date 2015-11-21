// Lauro Cruz e Souza - RA: 156175
// laurocruzsouza@gmail.com | lauro.souza@students.ic.unicamp.br

#include "bico.h" /* Robot control API */

void segue_parede(void);
void busca_parede_mode(void);
void segue_parede_mode(void);
void turn_left(void);
void spin(void);

/* main function */
void _start(void) {
    
    register_proximity_callback(3, 900, turn_left);
    register_proximity_callback(4, 900, turn_left);

    while (1) {
        set_motors_speed(40,40);
    }

}

void ronda(void) {

    

}



void segue_parede(void) {
    busca_parede_mode();
    segue_parede_mode();
}

void busca_parede_mode(void) {
    unsigned int d[16];
    unsigned short s0, s1;

    set_motors_speed(40, 40);

    read_sonar(3, &s0); read_sonar(4, &s1);

    while (s0 > 900 && s1 > 900) {
        read_sonar(3, &s0); 
        read_sonar(4, &s1);
    }

    set_motors_speed(0,0);

    read_sonar(0, &s0); read_sonar(15, &s1);
    set_motor_speed(1, 6);

    while ( (s0 > 750 && s1 > 750) || ((s0 - s1 >= 12) || (s1 - s0 >= 12)) ) {
        read_sonar(0, &s0); 
        read_sonar(15, &s1);
    }
    set_motors_speed(0,0);
}

void segue_parede_mode(void) {
    unsigned short s0, s15;
//    register_proximity_callback(3, 900, end_wall);
//    register_proximity_callback(4, 900, end_wall);

    do {
        read_sonar(0, &s0); read_sonar(15, &s15);
        set_motors_speed(15, 15);

        if (s0 > s15) { // Vai se afastar ao seguir reto

            while ((s0 < 500 || s15 < 500)) {
                read_sonar(0, &s0); 
                read_sonar(15, &s15);
            }
    
            set_motors_speed(5, 0);

            while (s0 - s15 > 10) {
                read_sonar(0, &s0); 
                read_sonar(15, &s15);
            }
            set_motors_speed(0, 0);

        } else { // Vai se aproximar ao seguir reto
            while (s0 > 300 || s15 > 300) {
                read_sonar(0, &s0); 
                read_sonar(15, &s15);
            }

            set_motors_speed(0, 5);

            while (s15 - s0 > 10) {
                read_sonar(0, &s0); 
                read_sonar(15, &s15);
            }
            set_motors_speed(0, 0);
        }

    } while(1);

}

void adjust_pos(void) {
    set_motor_speed(0, 20);


}

void turn_left(void) {
    unsigned short a, b;

    set_motor_speed(1, 0);

    read_sonar(3, &a); read_sonar(4, &b);
    while(a < 1000 || b < 1000) {
        read_sonar(3, &a);
        read_sonar(4, &b);
    }
    set_motor_speed(1, 40);
    
    return;
}

