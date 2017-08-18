CC=g++
CFLAGS=-W -Wall -ansi -pedantic
EXEC=executavel
OBJ=acel_tempo9.o medicao_acel.o filtro.o
DEPS=medicao_acel.h filtro.h

all:	$(EXEC)
$(EXEC):	$(OBJ)
	$(CC) -o $(EXEC) $(OBJ)
%.o:	%.cpp $(DEPS)
	$(CC) -o $@ -c $< $(CFLAGS)
clean:
	rm -rf *.o
mrproper:	clear
	rm -rf $(EXEC)
