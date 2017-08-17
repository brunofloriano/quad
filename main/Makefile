
all:	acel_tempo8
acel_tempo8:	acel_tempo8.o	medicao_acel.o
	g++ -o acel_tempo8 acel_tempo8.o medicao_acel.o
medicao_acel.o:	medicao_acel.cpp
	g++ -o medicao_acel.o -c medicao_acel.cpp -W -Wall -ansi -pedantic
acel_tempo8.o: acel_tempo8.cpp medicao_acel.h
	g++ -o acel_tempo8.o -c acel_tempo8.cpp -W -Wall -ansi -pedantic
clean:
	rm -rf *.o
mrproper:	clear
	rm -rf acel_tempo8