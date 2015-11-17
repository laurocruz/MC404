@ Lauro Cruz e Souza - RA: 156175
@ laurocruzsouza@gmail.com | lauro.souza@students.ic.unicamp.br

@ SOUL - Sistema Operacional UoLi

.align 4

.org 0x0
.section .iv,"a"

_start:


interrupt_vector:

@ 0x00
    b RESET_HANDLER
.org 0x04
    b UND_HANDLER
.org 0x08
    b SVC_HANDLER
.org 0x0C
    b ABTI_HANDLER
.org 0x10
    b ABTD_HANDLER
.org 0x18
    b IRQ_HANDLER
.org 0x1C
    b FIQ_HANDLER


.org 0x100

.text

@@@@@@@@@@@@@@@@@@@@@@@@@ SETTING HARDWARE @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
SET_GPT:
    @ Constantes para os enderecos de GPT
    .set GPT_BASE, 0x53FA0000
    .set GPT_CR,   0x0
    .set GPT_PR,   0x4
    .set GPT_IR,   0xC
    .set GPT_OCR1, 0x10

    .set TIME_SZ 1000

    ldr r1, =GPT_BASE

    @ Habilita GPT
    @ Configura o clock_src para o clock periférico
    mov r0, #0x41
    str r0, [r1, #GPT_CR]

    @ Zera o prescaler
    mov r0, #0
    str r0, [r1, #GPT_PR]

    @ GPT_OCR1 -> interruption to increment SYS_TIME
    ldr r0, =TIME_SZ
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


SET_GPIO:
    @ Constants for the GPIO addresses
    .set GPIO_BASE, 0x53F84000
    .set GPIO_DR,   0x00
    .set GPIO_GDIR, 0x04
    .set GPIO_PSR,  0x08

    @ Starts the I/O controller
    @ R1 <- GPIO_BASE
    ldr r1, =GPIO_BASE

    @ Set definitions of I/O of the GPIO pins in GDIR
    ldr r0, =0xFFFC003F @1111 1111 1111 1100 0000 0000 0011 1110
    str r0, [r1, #GPIO_GDIR]


@@@@@@@@@@@@@@@@@@@@@@@@@@@@ OPERATION MODES @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

RESET_HANDLER:
    @ Reset the SYS_TIME
    ldr r1, =SYS_TIME
    mov r0, #0
    str r0,[r1]

    @Set interrupt table base address on coprocessor 15.
    ldr r0, =interrupt_vector
    mcr p15, 0, r0, c12, c0, 0

    stmfd sp!, {r0-r3}
    bl SET_GPIO
    bl SET_GPT
    bl SET_TZIC
    ldmfd sp!, {r0-r3}

    @instrucao msr - habilita interrupcoes
    msr  CPSR_c, #0x10       @ USER mode, IRQ/FIQ enabled

infinite_loop:
    b infinite_loop


SVC_HANDLER:
    .set MAX_ALARMS,    8
    .set MAX_CALLBACKS, 8

    @ Enter in supervisor mode

    cmp r7, #16         @ Compares r7 to determine the syscall
    addne pc, pc #0xC   @ If the syscall is not the one being tested, jumps for the next
    bleq read_sonar     @ If this is the right syscall, jumps to the syscall routine
    b svc_end           @ Finishes the syscall

    cmp r7, #17
    addne pc, pc, #0xC
    bleq register_proximity_callback
    b svc_end

    cmp r7, #18
    addne pc, pc, #0xC
    bleq set_motor_speed
    b svc_end

    cmp r7, #19
    addne pc, pc, #0xC
    bleq set_motors_speed
    b svc_end

    cmp r7, #20
    addne pc, pc, #0xC
    bleq get_time
    b svc_end

    cmp r7, #21
    addne pc, pc, #0xC
    bleq set_time
    b svc_end

    cmp r7, #22
    addne pc, pc, #0xC
    bleq set_alarm

svc_end:
    movs pc, lr

@@@@@@@@ Syscall functions @@@@@@@@

@@@
@ in: r0 = sonar id
@ out : r0 = distance value
@          = -1 for invalid sonar id
@@@
svc_read_sonar:
    @ if sonar id is invalid, exit syscall
    @ return -1
    cmp r0, #15
    movgt r0, #-1
    bgt end_read_sonar

    cmp r0, #0
    movlt r0, #-1
    blt end_read_sonar

    @ gets GPIO base
    ldr r1, =GPIO_BASE

    @ Set sonar to be read
    @ Initially trigger = 0
    mov r0, r0, lsl #2

    str r0, [r1, #GPIO_DR]

    @@@@@@@@ delay 15 ms
    stmfd sp!, {r0-r3}
    mov r0, #15
    bl delay
    ldmfd sp!, {r0-r3}

    @ Set trigger = 1
    orr r0, r0, #0x2
    str r0, [r1, #GPIO_DR]

    @@@@@@@@ delay 15 ms
    stmfd sp!, {r0-r3}
    mov r0, #15
    bl delay
    ldmfd sp!, {r0-r3}

    @ Set trigger = 0
    ldr r0, [r1, #GPIO_DR]
    eor r0, r0, #0x2
    str r0, [r1, #GPIO_DR]

check_flag:
    ldr r0, [r1, #GPIO_DR]
    mov r2, r0
    and r2, r2, #1

    @ if flag = 1, so te read is over and we have the distance
    cmp r2, #1
    beq flag_defined

    @ else, the program waits more 10 ms
    @@@@@@@ delay 10 ms
    stmfd sp!, {r0-r3}
    mov r0, #10
    bl delay
    ldmfd sp!, {r0-r3}

    b check_flag

flag_defined:
    @ Update r0 with the routine's return value
    mov r0, r0, lsr #6

end_read_sonar:
    mov pc, lr

@@@@
@ in:  r0 = sonar id
@      r1 = distance limit
@      r2 = pointer for the function to be called in case the alarm rings
@
@ out: r0 = -1: number of max callbacks active in the system is higher than MAX_CALLBACKS
@           -2: invalid sonar id
@            0: standard
@@@@
svc_register_proximity_callback:

    stmfd sp!, {r4-r6, lr}
    @ Already at maximum number of callbacks
    ldr r3, =CALLBACK_REGS
    ldr r3, [r3]
    cmp r3, #MAX_CALLBACKS
    moveq r0, #-1
    beq end_rpc

    @ Invalid sonar id
    cmp r0, #15
    movgt r0, #-2
    bgt end_rpc

    cmp r0, #0
    movlt r0, #-2
    blt end_rpc

    @ No errors
                            @ r3 = Number os callbacks registered
    ldr r4, =CALLBACK_IDS   @ r4 = Array of callback ids
    ldr r5, =CALLBACK_THRES @ r5 = Array of callback thresholds
    ldr r6, =CALLBACK_FUN   @ r6 = Array of callback functions

    mul r3, r3, #4          @ Array displacement

    str r0, [r4, r3]        @ Stores the sensor id
    str r1, [r5, r3]        @ Stores the distance threshold
    str r2, [r6, r3]        @ Stores the function pointer

    mov r0, #0              @ Return flag indicating that all went well

end_rpc:
    ldmfd sp!, {r4-r6, lr}

    mov pc, lr

@@@@
@ in:  r0 = motor id (0 or 1)
@      r1 = speed
@
@ out: r0 = -1: invalid motor id
@           -2: invalid speed
@            0: ok
@@@@
svc_set_motor_speed:


@@@@
@ in:  r0 = motor0 speed
@      r1 = motor1 speed
@
@ out: r0 = -1: invalid motor0 speed
@           -2: invalid motor1 speed
@            0: ok
@@@@
svc_set_motors_speed:


@@@@
@ in: --
@
@ out: r0 = system time
@@@@
svc_get_time:
    @ Gets system time
    ldr r1, =SYS_TIME
    ldr r0, [r1]

    mov pc, lr

@@@@
@ in: r0 = new system time
@
@ out: --
@@@@
svc_set_time:
    @ Sets system time
    ldr r1, =SYS_TIME
    str r0, [r1]

    mov pc, lr

@@@@
@ in:  r0 = pointer to alarm funtion
@      r1 = system time
@
@ out: r0 = -1: max number of active alarms is higher than MAX_ALARMS
@           -2: passed systime is higher than current systime
@            0: ok
@@@@
svc_set_alarm:


IRQ_HANDLER:
    @ Constante para GPT_SR
    .set GPT_SR, 0x53FA0008
    .set DIST_INTERVAL 1000 @ Callback every ~ 5 ms

    stmfd sp!, {r4-r7, lr}
    ldr r1, =GPT_SR

    @ Informa ao GPT que o processador está ciente de que ocorreu interrupcao
    mov r0, #0x1
    str r0, [r1]

    @ Incrementa SYS_TIME
    ldr r1, =SYS_TIME
    ldr r0, [r1]

    add r0, r0, #1

    str r0, [r1]

@@@@@ CHECK ALL ALARMS
    ldr r1, =ALARM_TIMES    @ Times to ring the alarms
    ldr r2, =ALARM_FUN      @ Functions to be called if alarma is ringed
    ldr r3, =ALARM_REGS     @ Number of alarms registered
    ldr r3, [r3]
    mov r4, #0              @ Loop counter

loop_check_alarms:
    add r4, r4, #1          @ Increments counter
    cmp r4, r3              @ Tests if checked all alarms
    bgt finish_alarms

    ldr r5, [r1], #4        @ r5 <- alarm time

    cmp r5, r0
    addne r2, r2, #4        @ if r5 != systime
    bne loop_check_alarms   @ goes to next alarm

    stmfd sp!, {r0-r3}      @ if r5 == systime
    bl [r2], #4             @ run function
    ldmfd sp!, {r0-r3}

finish_alarms:

    @ Incrementa CALLBACK_TIME
    ldr r1, =CALLBACK_TIME
    ldr r0, [r1]

    add r0, r0, #1

    ldr r2, =DIST_INTERVAL  @ if number of cycles != 1000 systimes
    cmp r0, r2              @ continue with the counting
    strne r0, [r1]
    bne finish_callback

@@@@@ CHECK ALL CALLBACKS
    mov r0, #0              @ Resets the callback timer
    str r0, [r1]

    ldr r4, =CALLBACK_REGS  @ Number of registered callbacks
    ldr r4, [r4]

    ldr r1, =CALLBACK_IDS   @ Array of sensor ids
    ldr r2, =CALLBACK_FUN   @ Array of functions
    ldr r3, =CALLBACK_THRES @ Array of threshold distances

loop_make_callbacks:
    add r0, r0, #1
    cmp r0, r4
    bgt finish_callback

    stmfd sp!, {r0-r3}
    ldr r0, [r1], #4
    bl svc_read_sonar          @ read the distance from the sonar stored in the array
    ldr r5, [r3], #4       @ r5 <- threshold distance

    cmp r0, r5               @ if r0 > r5
    ldmgtfd sp!, {r0-r3}     @ Loads r0-r3 from the stack
    addgt r2, r2, #4         @ Skips function
    bgt loop_make_callbacks  @ Goes for the next sonar

                             @ if r0 <= r5
    bl [r2], #4              @ Jumps to the function
    ldmfd sp!, {r0-r3}       @ Restores the values of r0-r3

    b loop_make_callbacks

finish_callback:

    ldmfd sp!, {r4-r7, lr}

    @ Retorna da funcao
    sub lr, lr, #4 @ Conserta o valor de lr que até o momento era lr = PC + 8

    movs pc, lr @ Retorna da funcao e realiza CPSR <- SPSR_irq

@@@@@@@@@@@@@@@@@@@@@@ AUXILIAR FUNCTIONS @@@@@@@@@@@@@@@@@@@@@@@@

@@@
@ in: r0 = time delay (ms)
@@@
delay:
    stmfd sp!, {r4, lr}

    @ number of systimes to time delay (ms) (based on a clock of 200 MHz)
    cmp r0, #15
    ldreq r3, =3000
    beq end_define_delay

    cmp r0, #10
    ldreq r3, =2000

end_define_delay:
    @ gets initial value of systime
    ldr r4, =SYS_TIME
    ldr r0, [r4]

loop_delay:
    @ new value of systime
    ldr r1, [r4]

    @ difference of systime
    sub r2, r1, r0

    @ if r2 <= r3, makes another loop
    cmp r2, r3
    bls loop_delay

    ldmfd sp!, {r4, lr}

    mov pc, lr


@@@@@@@@@@@@@@@@@@@@@@@ DATA @@@@@@@@@@@@@@@@@@@@@
.data
@ System timer
SYS_TIME: .word 0x0


@ Number of registered alarms
ALARM_REGS: .word 0x0

@ Array of System Times of the alarms
ALARM_TIMES: .word 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0

@ ARRAY of function pointers to be called in the alarm
ALARM_FUN: .word 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0


@ Callback timer
CALLBACK_TIME: .word 0x0

@ Number of registered callbacks
CALLBACK_REGS: .word 0x0

@ Array of sensor ids to be verified in the callback
CALLBACK_IDS: .word 0x10 0x10 0x10 0x10 0x10 0x10 0x10 0x10

@ Array of threshold distances
CALLBACK_THRES: .word 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0

@ Array of function pointers to be called in the callback
CALLBACK_FUN: .word 0x0 0x0 0x0 0x0 0x0 0x0 0x0 0x0
