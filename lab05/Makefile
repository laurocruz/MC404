ASSEMBLER=arm-eabi-as
LINKER=arm-eabi-ld
SDMAKER=mksd.sh
LDFLAGS=-Ttext=0x77802000 -Tdata=0x778020c0
SIMULATOR=arm-sim

sd: link assemble
	${SDMAKER} --so /home/specg12-1/mc404/simulador/dummyos.elf --user ra156175

link: assemble
	${LINKER} ra156175.o -o ra156175 ${LDFLAGS}

assemble: ra156175.s
	${ASSEMBLER} ra156175.s -o ra156175.o

run: 
	${SIMULATOR} --rom=/home/specg12-1/mc404/simulador/dumboot.bin --sd=disk.img

clean:
	rm ra156175.o ra156175 disk.img -f
