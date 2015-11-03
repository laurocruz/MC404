 @ Lauro Cruz e Souza - RA: 156175
@ laurocruzsouza@gmail.com | lauro.souza@students.ic.unicamp.br

.global set_speed_motor
.global set_speed_motors
.global read_sonar
.global read_sonars

.align 4

@@@@@@@@@@@@@@@@@@
@ r0 <- speed    @
@ r1 <- motor id @
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
set_speed_motor:
    cmp r1, #0          @ se r1 == 0, vai para funcao write_motor0
    beq write_motor0    @ caso contrario, vai para write_motor1

write_motor1:
    mov r7, #127
    svc 0x0

    b final

write_motor0:
    mov r7, #126
    svc 0x0

    b final
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


@@@@@@@@@@@@@@@@
@ r0 <- speed 0@
@ r1 <- speed 1@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
set_speed_motors:

write_motors:
    mov r7, #124
    svc 0x0

    b final
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ r0 <- sonar id                    @
@ return (r0) <- 13-bit unsigned int@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
read_sonar:
    mov r7, #125 @ num for read_sonar
    svc 0x0

    b final
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@ r0 <- array of integers address@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
read_sonars:
    mov r7, #125 @ num for read_sonar
    mov r1, r0   @ saves array address in r1 and r2
    mov r2, r0
    mov r0, #0   @ uses r0 for the sonar id

do_while:
    svc 0x0
    str r0, [r1], #4  @ stores the sonar distance in the array (32-bit integers)

    add r0, r0, #1    @ increments r0 for the loop
    cmp r0, #15
    bls do_while      @ continue in the loop if r0 <= 15

    mov r0, r2        @ stores the array address again in r0

    b final:
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

final:
