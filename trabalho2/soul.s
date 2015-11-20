@ Lauro Cruz e Souza - RA: 156175
@ laurocruzsouza@gmail.com | lauro.souza@students.ic.unicamp.br

@ SOUL - Sistema Operacional UoLi

.align 4

.org 0x0
.section .iv,"a"

interrupt_vector:
@ 0x00
    b RESET_HANDLER

.org 0x08
    b SVC_HANDLER

.org 0x18
    b IRQ_HANDLER

.org 0x100

.text


@@@@@@@@@@@@@@@@@@@@@@@@@@@@ OPERATION MODES @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

RESET_HANDLER:
    .set USER_start_program, 0x77802500
    .set SYS_MODE_INT_DIS,   0xDF
    .set IRQ_MODE_INT_DIS,   0xD2
    .set SVC_MODE_INT_DIS,   0xD3
    .set USR_MODE_INT_ENA,   0x10
    .set SVC_MODE_INT_ENA,   0x13
    .set IRQ_ENABLE,         0x80

    @Set interrupt table base address on coprocessor 15.
    ldr r0, =interrupt_vector
    mcr p15, 0, r0, c12, c0, 0

    @ Enters in Supervisor
    msr CPSR_c, #SVC_MODE_INT_DIS  @ Supervisor mode, IRQ/FIQ disabled

    mov r0, #0
    @ Reset the SYS_TIME
    ldr r1, =SYS_TIME
    str r0, [r1]

    @ Reset the alarms
    ldr r1, =ALARM_REGS
    str r0, [r1]

    @ Reset the callbacks
    ldr r1, =CALLBACK_REGS
    str r0, [r1]

@@@@@@@@@@@@@@@@@@@@@@@@@ SETTING HARDWARE @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
SET_GPT:
    @ Constantes para os enderecos de GPT
    .set GPT_BASE, 0x53FA0000
    .set GPT_CR,   0x0
    .set GPT_PR,   0x4
    .set GPT_IR,   0xC
    .set GPT_OCR1, 0x10

    .set TIME_SZ,  100

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


SET_GPIO:
    @ Constants for the GPIO addresses
    .set GPIO_BASE, 0x53F84000
    .set GPIO_DR,   0x00
    .set GPIO_GDIR, 0x04
    .set GPIO_PSR,  0x08

    .set GDIR_INIT, 0xFFFC003E
    .set DR_INIT,   0x02040000

    @ Starts the I/O controller
    @ R1 <- GPIO_BASE
    ldr r1, =GPIO_BASE

    ldr r0, =GDIR_INIT

    @ Set definitions of I/O of the GPIO pins in GDIR
    str r0, [r1, #GPIO_GDIR]

    @ Init the GPIO_DR
    ldr r2, [r1, #GPIO_DR]
    bic r2, r2, r0
    ldr r3, =DR_INIT
    orr r2, r2, r3
    str r2, [r1, #GPIO_DR]


@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

    @@@ Set mode stacks

    @ SVC
    ldr sp, =SVC_STACK

    @ USER/SYSTEM
    msr  CPSR_c, #SYS_MODE_INT_DIS  @ SYSTEM mode, IRQ/FIQ disabled
    ldr sp, =USER_STACK

    @ IRQ
    msr CPSR_c, #IRQ_MODE_INT_DIS  @ IRQ mode, IRQ/FIQ disabled
    ldr sp, =IRQ_STACK

    @@@ Changes to user mode and jumps to user code
    @instrucao msr - habilita interrupcoes
    msr  CPSR_c, #USR_MODE_INT_ENA    @ USER mode, IRQ/FIQ enabled

    @ Jumps to the start of the user function
    ldr pc, =USER_start_program

SVC_HANDLER:
    .set MAX_ALARMS,    8
    .set MAX_CALLBACKS, 8

    stmfd sp!, {r1-r12, lr}

    @ Enter in supervisor mode

svc_16:
    cmp r7, #16             @ Compares r7 to determine the syscall
    bne svc_17              @ If the syscall is not the one being tested, jumps for the next
    bleq svc_read_sonar     @ If this is the right syscall, jumps to the syscall routine
    b svc_end               @ Finishes the syscall

svc_17:
    cmp r7, #17
    bne svc_18
    bleq svc_register_proximity_callback
    b svc_end

svc_18:
    cmp r7, #18
    bne svc_19
    bleq svc_set_motor_speed
    b svc_end

svc_19:
    cmp r7, #19
    bne svc_20
    bleq svc_set_motors_speed
    b svc_end

svc_20:
    cmp r7, #20
    bne svc_21
    bleq svc_get_time
    b svc_end

svc_21:
    cmp r7, #21
    bne svc_22
    bleq svc_set_time
    b svc_end

svc_22:
    cmp r7, #22
    bne svc_end
    bleq svc_set_alarm

svc_end:
    ldmfd sp!, {r1-r12, lr}
    movs pc, lr

@@@@@@@@ Syscall functions @@@@@@@@

@@@
@ in: r0 = sonar id
@ out : r0 = distance value
@          = -1 for invalid sonar id
@@@
svc_read_sonar:
    stmfd sp!, {lr}

    @ if sonar id is invalid, exit syscall
    @ return -1
    cmp r0, #15
    movhi r0, #-1
    bhi end_read_sonar

    @ gets GPIO base
    ldr r1, =GPIO_BASE

    @ Set sonar to be read

    ldr r2, [r1, #GPIO_DR]
    bic r2, r2, #0x3E        @ Resets the sonar_mux ang trigger positions
    orr r2, r2, r0, lsl #2           @ Makes the new DR array
    str r2, [r1, #GPIO_DR]

    @@@@@@@@ delay 15 ms
    stmfd sp!, {r0-r3}
    mov r0, #15
    bl delay
    ldmfd sp!, {r0-r3}

    @ Set trigger = 1
    ldr r0, [r1, #GPIO_DR]

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

    @ if flag = 1, so the read is over and we have the distance
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
    ldr r1, =0xFFF
    and r0, r0, r1

end_read_sonar:
    ldmfd sp!, {lr}
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

    mov r3, r3, lsl #2          @ Array displacement

    str r0, [r4, r3]        @ Stores the sensor id
    str r1, [r5, r3]        @ Stores the distance threshold
    str r2, [r6, r3]        @ Stores the function pointer

    mov r0, #0              @ Return flag indicating that all went well

end_rpc:
    ldmfd sp!, {r4-r6, lr}

    mov pc, lr

@@@@
@ in:  r0 = motor id (0 or 1)
@      r1 = motor speed
@
@ out: r0 = -1: invalid motor id
@           -2: invalid speed
@            0: ok
@@@@
svc_set_motor_speed:
    stmfd sp!, {lr}

    @ Invalid motor speed
    cmp r1, #63
    movhi r0, #-2
    bhi end_motor_speed

    @ Verify motor id
    @ motor0
    cmp r0, #0
    beq set_motor0

    @ motor1
    cmp r0, #1
    beq set_motor1

    @ Invalid id
    mov r0, #-1
    b end_motor_speed

set_motor0:
    .set BIT_CLEAR_MOTOR_0, 0x01FC0000
    @ Stores the array in GPIO_DR
    ldr r2, =GPIO_BASE

    ldr r0, [r2, #GPIO_DR]

    @ sets new motor0_speed
    ldr r3, =BIT_CLEAR_MOTOR_0
    bic r0, r0, r3
    orr r0, r0, r1, lsl #19
    str r0, [r2, #GPIO_DR]

    mov r0, #0

    b end_motor_speed

set_motor1:
    .set BIT_CLEAR_MOTOR_1, 0xFE000000
    @ Stores the array in GPIO_DR
    ldr r2, =GPIO_BASE

    ldr r0, [r2, #GPIO_DR]

    @ sets new motor1_speed
    ldr r3, =BIT_CLEAR_MOTOR_1
    bic r0, r0, r3
    orr r0, r0, r1, lsl #26
    str r0, [r2, #GPIO_DR]

    mov r0, #0

end_motor_speed:
    ldmfd sp!, {lr}
    mov pc, lr


@@@@
@ in:  r0 = motor0 speed (0 - 63)
@      r1 = motor1 speed (0 - 63)
@
@ out: r0 = -1: invalid motor0 speed
@           -2: invalid motor1 speed
@            0: ok
@@@@
svc_set_motors_speed:
    stmfd sp!, {r4, lr}

    @ Invalid motor0 speed
    cmp r0, #63  @ Max speed
    movhi r0, #-1
    bhi end_motors_speed

    @ Invalid motor1 speed
    cmp r1, #63
    movhi r0, #-2
    bhi end_motors_speed

    @ All ok
    ldr r2, =GPIO_BASE
    ldr r3, [r2, #GPIO_DR]

    ldr r4, =0xFFFC
    bic r3, r3, r4, lsl #16 @ Resets the speeds and writes

    @ Makes the array for GPIO_DR in r1
    mov r1, r1, lsl #26
    orr r1, r1, r0, lsl #19
    orr r3, r3, r1

    str r3, [r2, #GPIO_DR]

    @ Sets the ok flag
    mov r0, #0

end_motors_speed:
    ldmfd sp!, {r4, lr}
    mov pc, lr


@@@@
@ in: --
@
@ out: r0 = system time
@@@@
svc_get_time:
    stmfd sp!, {lr}
    @ Gets system time
    ldr r1, =SYS_TIME
    ldr r0, [r1]

    ldmfd sp!, {lr}
    mov pc, lr

@@@@
@ in: r0 = new system time
@
@ out: --
@@@@
svc_set_time:
    stmfd sp!, {lr}

    @ Sets system time
    ldr r1, =SYS_TIME
    str r0, [r1]

    ldmfd sp!, {lr}
    mov pc, lr

@@@@
@ in:  r0 = pointer to alarm funtion
@      r1 = system time
@
@ out: r0 = -1: max number of active alarms is higher than MAX_ALARMS
@           -2: passed systime is lower than current systime
@            0: ok
@@@@
svc_set_alarm:
    stmfd sp!, {r4, lr}
    @ Already at maximum number of alarms
    ldr r2, =ALARM_REGS
    ldr r2, [r2]
    cmp r2, #MAX_ALARMS
    moveq r0, #-1
    beq end_set_alarm

    @ New systime lower than current systime
    mov r4, r1          @ r4 <- systime
    stmfd sp!, {r0-r3}
    bl svc_get_time     @ Gets system time
    cmp r4, r0
    ldmfd sp!, {r0-r3}
    movlt r0, #-2       @ if new systime < current systime
    blt end_set_alarm   @ ends routine

    @ All Fine
                         @ r2 = Number of registered alarms
    ldr r3, =ALARM_FUN   @ r3 = Array of functions
    ldr r4, =ALARM_TIMES @ r4 = Array of alarm times

    mov r2, r2, lsl #2       @ r2 = Array displacement

    str r0, [r3, r2]
    str r1, [r4, r2]

end_set_alarm:
    ldmfd sp!, {r4, lr}
    mov pc, lr


IRQ_HANDLER:
    @ Constante para GPT_SR
    .set GPT_SR,        0x53FA0008
    .set DIST_INTERVAL, 1000 @ Callback every ~ 5 ms

    stmfd sp!, {r0-r12, lr}
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
    blx r2                  @ run function
    add r2, r2, #4
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
    blx r2                   @ Jumps to the function
    add r2, r2, #4
    ldmfd sp!, {r0-r3}       @ Restores the values of r0-r3

    b loop_make_callbacks

finish_callback:

    ldmfd sp!, {r0-r12, lr}

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
    ldreq r3, =30000
    beq end_define_delay

    cmp r0, #10
    ldreq r3, =20000

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

.set STACK_SIZE, 0xFF

@ Number of registered alarms
ALARM_REGS: .word 0x0

@ Array of System Times of the alarms
ALARM_TIMES: .fill MAX_CALLBACKS, 4, 0x0

@ ARRAY of function pointers to be called in the alarm
ALARM_FUN: .fill MAX_CALLBACKS, 4, 0x0


@ Callback timer
CALLBACK_TIME: .word 0x0

@ Number of registered callbacks
CALLBACK_REGS: .word 0x0

@ Array of sensor ids to be verified in the callback
CALLBACK_IDS: .fill MAX_CALLBACKS, 4, 0x10

@ Array of threshold distances
CALLBACK_THRES: .fill MAX_CALLBACKS, 4, 0x0

@ Array of function pointers to be called in the callback
CALLBACK_FUN: .fill MAX_CALLBACKS, 4, 0x0

@ Stack for the USER mode
.fill STACK_SIZE, 4, 0x0
USER_STACK:

@ Stack for the IRQ mode
.fill STACK_SIZE, 4, 0x0
IRQ_STACK:

@ Stack for the SUPERVISOR mode
.fill STACK_SIZE, 4, 0x0
SVC_STACK:
