CC=g++
CFLAGS=-W -Wall -ansi -pedantic
EXEC=executavel
OBJ=acel_tempo5.o medicao.o
DEPS=medicao.h

all:	$(EXEC)
$(EXEC):	$(OBJ)
	$(CC) -o $(EXEC) $(OBJ)
%.o:	%.cpp $(DEPS)
	$(CC) -o $@ -c $< $(CFLAGS)
clean:
	rm -rf *.o
mrproper:	clear
	rm -rf $(EXEC)
