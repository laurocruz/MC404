ASSEMBLER=arm-eabi-as
LINKER=arm-eabi-ld
SDMAKER=mksd.sh
LDFLAGS=-Ttext=0x77802000
SIMULATOR=armsim_player

sd: link assemble
	${SDMAKER} --so /home/specg12-1/mc404/simulador/simulador_player/bin/knrl --user ra156175

link: assemble
	${LINKER} ra156175.o -o ra156175 ${LDFLAGS}

assemble: ra156175.s
	${ASSEMBLER} ra156175.s -o ra156175.o

run: 
	${SIMULATOR} --rom=/home/specg12-1/mc404/simulador/simulador_player/bin/dumboot.bin --sd=disk.img

source:
	source /home/specg12-1/mc404/simulador/set_path_player.sh 

clean:
	rm ra156175.o ra156175 disk.img -f
