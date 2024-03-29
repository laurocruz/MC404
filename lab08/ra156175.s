@ Lauro Cruz e Souza - RA: 156175
@ laurocruzsouza@gmail.com | lauro.souza@students.ic.unicamp.br

.align 4
.global _start

.org 0x0
.section .iv,"a"

_start:

interrupt_vector:

    b RESET_HANDLER

.org 0x18
    b IRQ_HANDLER

.org 0x100

@@@@@@@@@@@@@@@@@@@@@@@@ TEXT @@@@@@@@@@@@@@@@@@@@
.text

RESET_HANDLER:

    @ Zera o contador
    ldr r2, =CONTADOR
    mov r0, #0
    str r0,[r2]

    @Set interrupt table base address on coprocessor 15.
    ldr r0, =interrupt_vector
    mcr p15, 0, r0, c12, c0, 0

    stmfd sp!, {r0-r3}
    bl SET_GPT
    bl SET_TZIC
    ldmfd sp!, {r0-r3}

    @instrucao msr - habilita interrupcoes
    msr  CPSR_c, #0x13       @ SUPERVISOR mode, IRQ/FIQ enabled

infinite_loop:
    b infinite_loop


IRQ_HANDLER:
    @ Constante para GPT_SR
    .set GPT_SR, 0x53FA0008

    ldr r1, =GPT_SR

    @ Informa ao GPT que o processador está ciente de que ocorreu interrupcao
    mov r0, #0x1
    str r0, [r1]

    @ Incrementa contador de interrupcoes
    ldr r1, =CONTADOR
    ldr r0, [r1]

    add r0, r0, #1

    str r0, [r1]

    @ Retorna da funcao
    sub lr, lr, #4 @ Conserta o valor de lr que até o momento era lr = PC + 8

    movs pc, lr @ Retorna da funcao e realiza CPSR <- SPSR_irq

SET_GPT:
    @ Constantes para os enderecos de GPT
    .set GPT_BASE, 0x53FA0000
    .set GPT_CR,   0x0
    .set GPT_PR,   0x4
    .set GPT_IR,   0xC
    .set GPT_OCR1, 0x10

    ldr r1, =GPT_BASE

    @ Habilita e configura o clock_src para periférico
    mov r0, #0x41
    str r0, [r1, #GPT_CR]

    @ Zera o prescaler
    mov r0, #0
    str r0, [r1, #GPT_PR]

    @ Coloca em GPT_OCR1 o valor que eu desejo contar
    mov r0, #100
    str r0, [r1, #GPT_OCR1]

    @ Habilita interrupcao Output Compare Channel 1
    mov r0, #1
    str r0, [r1, #GPT_IR]

    mov pc, lr

SET_TZIC:
    @ Constantes para os enderecos do TZIC
    .set TZIC_BASE,             0x0FFFC000
    .set TZIC_INTCTRL,          0x0
    .set TZIC_INTSEC1,          0x84
    .set TZIC_ENSET1,           0x104
    .set TZIC_PRIOMASK,         0xC
    .set TZIC_PRIORITY9,        0x424

    @ Liga o controlador de interrupcoes
    @ R1 <= TZIC_BASE
    ldr	r1, =TZIC_BASE

    @ Configura interrupcao 39 do GPT como nao segura
    mov	r0, #(1 << 7)
    str	r0, [r1, #TZIC_INTSEC1]

    @ Habilita interrupcao 39 (GPT)
    @ reg1 bit 7 (gpt)

    mov	r0, #(1 << 7)
    str	r0, [r1, #TZIC_ENSET1]

    @ Configure interrupt39 priority as 1
    @ reg9, byte 3

    ldr r0, [r1, #TZIC_PRIORITY9]
    bic r0, r0, #0xFF000000
    mov r2, #1
    orr r0, r0, r2, lsl #24
    str r0, [r1, #TZIC_PRIORITY9]

    @ Configure PRIOMASK as 0
    eor r0, r0, r0
    str r0, [r1, #TZIC_PRIOMASK]

    @ Habilita o controlador de interrupcoes
    mov	r0, #1
    str	r0, [r1, #TZIC_INTCTRL]

    mov pc, lr

@@@@@@@@@@@@@@@@@@@@@@@ DATA @@@@@@@@@@@@@@@@@@@@@
.data

CONTADOR: .word 0x0
