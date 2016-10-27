# Makefile
#

prefix: main.o source.o gettoken.o interpret.o util.o
	g++ main.o source.o gettoken.o interpret.o util.o
 

main.o: main.cpp
	g++ -c main.cpp -o main.o
	
source.o: source.cpp
	g++ -c source.cpp -o source.o

gettoken.o: gettoken.cpp
	g++ -c gettoken.cpp -o gettoken.o

interpret.o: interpret.cpp
	g++ -c interpret.cpp -o interpret.o

util.o: util.cpp
	g++ -c util.cpp -o util.o

.PHONY: clean
clean:
	rm -f *.o
