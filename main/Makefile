CC=gcc
CFLAGS=-W -Wall -ansi -pedantic
EXEC=executavel
OBJ=acelerometro.o medicao_acelc.o
DEPS=medicao_acelc.h

all:	$(EXEC)
$(EXEC):	$(OBJ)
	$(CC) -o $(EXEC) $(OBJ)
%.o:	%.cpp $(DEPS)
	$(CC) -o $@ -c $< $(CFLAGS)
clean:
	rm -rf *.o
mrproper:	clear
	rm -rf $(EXEC)
