##
# succeed compiled on Centos6.5 with gcc 4.8.2
##

CXX = g++
CXXFLAGS = -g -O2 -D_REENTRANT -Wall -std=c++11
LIBS = -lpthread

PROGS = testexe
OBJ = stack_array.o

all:    ${PROGS}

testexe: ${OBJ} 
	${CXX} ${CFLAGS} -o $@ $^ ${LIBS}
	mv ${PROGS} bin
	mv ${OBJ} bin

run:
	./bin/${PROGS}
clean:
	rm -f bin/* 
