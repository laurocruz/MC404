GCC=arm-eabi-gcc
ASSEMBLER=arm-eabi-as
LINKER=arm-eabi-ld
SDMAKER=mksd.sh
LDFLAGS=-Ttext=0x77802000
SIMULATOR=armsim_player

sd: link assemble compile
	${SDMAKER} --so /home/specg12-1/mc404/simulador/simulador_player/bin/knrl --user robot_ctrl

link: assemble compile
	${LINKER} main.o api_robot.o -o robot_ctrl ${LDFLAGS}

assemble: compile api_robot.s
	${ASSEMBLER} main.s -o main.o
	${ASSEMBLER} api_robot.s -o api_robot.o

compile: main.c
	${GCC} main.c -S -o main.s

run: 
	${SIMULATOR} --rom=/home/specg12-1/mc404/simulador/simulador_player/bin/dumboot.bin --sd=disk.img

player:
	player /home/specg12-1/mc404/simulador/simulador_player/worlds_mc404/simple.cfg 

source:
	source /home/specg12-1/mc404/simulador/set_path_player.sh

clean:
	rm *.o main.s robot_ctrl disk.img -f
