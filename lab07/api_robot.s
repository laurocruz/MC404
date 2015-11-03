 @ Lauro Cruz e Souza - RA: 156175
@ laurocruzsouza@gmail.com | lauro.souza@students.ic.unicamp.br

.global set_speed_motor
.global set_speed_motors
.global read_sonar
.global read_sonars

.align 4

@@@
set_speed_motor:
    @ r0 <- speed 
    @ r1 <- motor id 
    stmfd sp!, {r7, lr} 

    cmp r1, #0          @ se r1 == 0, vai para funcao write_motor0
    beq write_motor0    @ caso contrario, vai para write_motor1

write_motor1:
    mov r7, #127
    svc 0x0

    ldmfd sp!, {r7, lr}
    mov pc, lr

write_motor0:
    mov r7, #126
    svc 0x0

    ldmfd sp!, {r7, lr}
    mov pc, lr

@@@
set_speed_motors:
    @ r0 <- speed 0
    @ r1 <- speed 1
    stmfd sp!, {r7, lr} 

    mov r7, #124
    svc 0x0

    ldmfd sp!, {r7, lr}
    
    mov pc, lr

@@@
read_sonar:
    @ r0 <- sonar id                    
    @ return (r0) <- 13-bit unsigned short 
    stmfd sp!, {r7, lr}

    mov r7, #125 @ num for read_sonar
    svc 0x0

    ldmfd sp!, {r7, lr}
    mov pc, lr

@@@
read_sonars:
    @ r0 <- array of integers address
    stmfd sp!, {r7, lr}

    mov r1, r0        @ saves array address in r1 and r2
    mov r2, r0
    mov r3, #0

do_while:
    mov r0, r3
    mov r7, #125      @ num for read_sonar
    svc 0x0

    str r0, [r1], #4  @ stores the sonar distance in the array (32-bit integers)

    add r3, r3, #1    @ increments r0 for the loop
    cmp r3, #15
    ble do_while      @ continue in the loop if r3 <= 15

    mov r0, r2        @ stores the array address again in r0

    ldmfd sp!, {r7, lr}
    mov pc, lr

