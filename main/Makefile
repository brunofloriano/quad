CC=g++
CFLAGS=-W -Wall -ansi -pedantic
EXEC=teste
OBJ=teste_motor.o command.o
DEPS=/include/command.h

all:	$(EXEC)
$(EXEC):	$(OBJ)
	$(CC) -o $(EXEC) $(OBJ)
%.o:	%.cpp $(DEPS)
	$(CC) -o $@ -c $< $(CFLAGS)
clean:
	rm -rf *.o
mrproper:	clear
	rm -rf $(EXEC)
