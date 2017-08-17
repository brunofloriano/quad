CC=g++
CFLAGS=-W -Wall -ansi -pedantic
EXEC=acel_tempo8
OBJ=acel_tempo8.o medicao_acel.o
DEPS=medicao_acel.h

all:	$(EXEC)
$(EXEC):	$(OBJ)
	$(CC) -o $(EXEC) $(OBJ)
%.o:	%.cpp $(DEPS)
	$(CC) -o $@ -c $< $(CFLAGS)
clean:
	rm -rf *.o
mrproper:	clear
	rm -rf $(EXEC)