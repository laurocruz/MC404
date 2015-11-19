ASSEMBLER=arm-eabi-as
LINKER=arm-eabi-ld
SDMAKER=mksd.sh
SO_LDFLAGS=--section-start=.iv=0x778005e0 -Ttext=0x77800700 -Tdata=0x77801800 -e 0x778005e0
USER_LDFLAGS=-Ttext=0x77802500
SIMULATOR=armsim_player
GDB=arm-eabi-gdb
GCC=arm-eabi-gcc

sd: link assemble
	${SDMAKER} --so soul --user robot_ctrl

link: assemble
	${LINKER} soul.o -o soul -g ${SO_LDFLAGS}
	${LINKER} loco.o bico.o -o robot_ctrl ${USER_LDFLAGS}

assemble: gcc bico.s soul.s
	${ASSEMBLER} soul.s -o soul.o -g
	${ASSEMBLER} bico.s -o bico.o
	${ASSEMBLER} loco.s -o loco.o

gcc: loco.c
	${GCC} loco.c -S -o loco.s

run: 
	${SIMULATOR} --rom=/home/specg12-1/mc404/simulador/simulador_player/bin/dumboot.bin --sd=disk.img --debug=core

player:
	player /home/specg12-1/mc404/simulador/simulador_player/worlds_mc404/simple.cfg 

gdb:
	${GDB} ra156175

source:
	source /home/specg12-1/mc404/simulador/set_path_player.sh

clean:
	rm *.o soul robot_ctrl disk.img loco.s -f