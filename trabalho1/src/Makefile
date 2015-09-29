ias-as: main.o label.o instruction.o directive.o config.o data.o
	ld -dynamic-linker /lib64/ld-linux-x86-64.so.2 /usr/lib64/crt1.o /usr/lib64/crti.o -L/usr/lib64 *.o -lc /usr/lib64/crtn.o -o ias-as
	
main.o: main.s
	as main.s -o main.o
label.o: label.s
	as label.s -o label.o
instruction.o: instruction.s
	as instruction.s -o instruction.o
directive.o: directive.s
	as directive.s -o directive.o
config.o: config.s
	as config.s -o config.o
data.o: data.s
	as data.s -o data.o

main.s: main.c
	gcc -S main.c -o main.s
label.s: label.c
	gcc -S label.c -o label.s
instruction.s: instruction.c
	gcc -S instruction.c -o instruction.s
directive.s: directive.c
	gcc -S directive.c -o directive.s
config.s: config.c
	gcc -S config.c -o config.s
data.s: data.c
	gcc -S data.c -o data.s

clean:
	rm ias-as *.o *.s

