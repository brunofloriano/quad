CC=g++
CFLAGS=-W -Wall -ansi -pedantic
EXEC=acel_tempo8
OBJ=acel_tempo8.o medicao_acel.o

all:	$(EXEC)
acel_tempo8:	$(OBJ)
	$(CC) -o $(EXEC) $(OBJ)
medicao_acel.o:	medicao_acel.cpp
	$(CC) -o medicao_acel.o -c medicao_acel.cpp $(CFLAGS)
acel_tempo8.o: acel_tempo8.cpp medicao_acel.h
	$(CC) -o acel_tempo8.o -c acel_tempo8.cpp -W -Wall -ansi -pedantic
clean:
	rm -rf *.o
mrproper:	clear
	rm -rf $(EXEC)