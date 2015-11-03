@ Lauro Cruz e Souza - RA: 156175
@ MC404
@ laurocruzsouza@gmail.com | lauro.souza@students.ic.unicamp.br

.text
.align 4
.globl _start

_start:                     @ main
    mov r0, #0              @ Carrega em r0 a velocidade do motor 0.
                            @ Lembre-se: apenas os 6 bits menos significativos
                            @ serao utilizados.
    mov r1, #0              @ Carrega em r1 a velocidade do motor 1.
    mov r7, #124            @ Identifica a syscall 124 (write_motors).
    svc 0x0                 @ Faz a chamada da syscall.

    ldr r6, =1200           @ r6 <- 1200 (Limiar para parar o robo)


loop:
    mov r0, #3              @ Define em r0 o identificador do sonar a ser consultado.
    mov r7, #125            @ Identifica a syscall 125 (read_sonar).
    svc 0x0
    mov r5, r0              @ Armazena o retorno da syscall.

    mov r0, #4              @ Define em r0 o sonar.
    mov r7, #125
    svc 0x0

    cmp r5, r0              @ Compara o retorno (em r0) com r5.
    bge forward                 @ Se r5 > r0: Salta pra min
    mov r0, r5              @ Senao: r0 <- r5

forward:
    cmp r0, r6
    blt turn

    mov r0, #63             @ Faz o robê seguir em frente
    mov r1, #63
    mov r7, #124
    svc 0x0

    b loop

turn:
    mov r0, #0              @ Vira o robô para a direita
    mov r1, #15

    mov r7, #124
    svc 0x0

    b loop

@ Não utiliza, mas just in case
end:                            @ Parar o robo
    mov r0, #0
    mov r1, #0
    mov r7, #124
    svc 0x0

    mov r7, #1              @ syscall exit
    svc 0x0
