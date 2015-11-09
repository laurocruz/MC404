ASSEMBLER=arm-eabi-as
LINKER=arm-eabi-ld
SDMAKER=mksd.sh
LDFLAGS=-Ttext=--section-start=.iv=0x778005e0 -Ttext=0x77800700 -Tdata=0x77801800 -e 0x778005e0
SIMULATOR=arm-sim
GDB=arm-eabi-gdb

sd: link assemble
	${SDMAKER} --so ra156175 --user faz_nada

link: assemble
	${LINKER} ra156175.o -o ra156175 -g ${LDFLAGS}

assemble: ra156175.s
	${ASSEMBLER} -g ra156175.s -o ra156175.o

run: 
	${SIMULATOR} --rom=/home/specg12-1/mc404/simulador/simulador_player/bin/dumboot.bin --sd=disk.img

gdb:
	${GDB} ra156175

source:
	source /home/specg12-1/mc404/simulador/set_path_player.sh

clean:
	rm *.o ra156175 disk.img -f
