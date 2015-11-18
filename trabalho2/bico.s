@ Lauro Cruz e Souza - RA: 156175
@ laurocruzsouza@gmail.com | lauro.souza@students.ic.unicamp.br

@ BiCo - Biblioteca de controle

.global set_motor_speed
.global set_motors_speed
.global read_sonar
.global read_sonars
.global register_proximity_callback
.global add_alarm
.global get_time
.global set_time

.align 4

@@@
@ r0 <- motor id (unsigned char)
@ r1 <- motor speed (unsigned char) only 6 lsb
@@@
set_motor_speed:
    stmfd sp!, {r7, lr}

    mov r7, #18 @ calls syscall set_motor_speed
    svc 0x0

    ldmfd sp!, {r7, lr}

    mov pc, lr

@@@
@ r0 <- speed motor0 (unsigned char) only 6 lsb
@ r1 <- speed motor1 (unsigned char)      ''
@@@
set_motors_speed:
    stmfd sp!, {r7, lr}

    mov r7, #19 @ calls syscall set_motors_speed
    svc 0x0

    ldmfd sp!, {r7, lr}

    mov pc, lr

@@@
@ r0 <- sonar id (unsigned char)
@ r1 <- pointer to variable that will receive distance (unsigned short*)
@@@
read_sonar:
    stmfd sp!, {r7, lr}

    mov r7, #16 @ calls syscall read_sonar
    svc 0x0

    str r0, [r1] @ stores the distance read in the variable in the memory

    ldmfd sp!, {r7, lr}
    mov pc, lr

@@@
@ r0 <- address of array of unsigned integers
@@
read_sonars:
    stmfd sp!, {r7, lr}

    mov r1, r0        @ saves array address in r1
    mov r3, #0

do_while:
    mov r0, r3

    stmfd sp!, {r2-r3}
    bl read_sonar        @ calls the read_sonar function
    ldmfd sp!, {r2-r3}   @ read_sonar stores the distance in the array

    add r1, r1, #4        @ move forward in the array

    add r3, r3, #1       @ increments r3 for the loop
    cmp r3, #16
    blt do_while         @ continue in the loop if r3 < 16

    ldmfd sp!, {r7, lr}
    mov pc, lr

@@@
@ r0 <- sensor id (unsigned char)
@ r1 <- distance threshold (unsigned short)
@ r2 <- pointer to void function
@@@
register_proximity_callback:
    stmfd sp!, {r7, lr}

    mov r7, #17 @ calls syscall register_proximity_callback
    svc 0x0

    ldmfd sp!, {r7, lr}
    mov pc, lr

@@@
@ r0 <- pointer to void function
@ r1 <- time (unsigned int)
@@@
add_alarm:
    stmfd sp!, {r7, lr}

    mov r7, #22 @ calls syscall set_alarm
    svc 0x0

    ldmfd sp!, {r7, lr}
    mov pc, lr

@@@
@ void
@ r0 -> systime (unsigned int)
@@@
get_time:
    stmfd sp!, {r7, lr}

    mov r7, #20 @ calls syscall get_time
    svc 0x0     @ outputs the syscall in r0

    ldmfd sp!, {r7, lr}
    mov pc, lr

@@@
@ r0 <- new systime (unsigned int)
@@@
set_time:
    stmfd sp!, {r7, lr}

    mov r7, #21 @ calls syscal set_time
    svc 0x0

    ldmfd sp!, {r7, lr}
    mov pc, lr
