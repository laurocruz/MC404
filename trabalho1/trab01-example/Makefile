##########################################################
# Description: Template to build the makefile for
#   "trab01".
#
# Author: Edson Borin - edson@ic.unicamp.br
#
# 1) You may add more files to your project changing the 
#    OBJ_FILES list.
#
# 2) You may remove or add new rules to compile your .c files 
#    (see as an example rules main.o and error.o)
#
# 3) You may not change the ias-as rule (do not add 
#    extra libraries!)
#
##########################################################

# List of object files that must be linked together to 
# build the ias-as executable
OBJ_FILES=main.o error.o

# Default rule
all: ias-as

# -- Rules for making each one of the .o files --

# -- main.o depends on main.c and on error.h
main.o: main.c error.h
	gcc -c -g main.c -o main.o

# -- error.o depends on error.c and on error.h
error.o: error.c error.h
	gcc -c -g error.c -o error.o

# -- Clean rule. It removes the ias-as file and any .o file on the directory --
clean:
	rm -f ias-as *.o

##########################################################
# Do not change this rule
##########################################################

# -- Rule to make the executable ias-as
ias-as: $(OBJ_FILES)
	gcc -g $(OBJ_FILES) -o ias-as 