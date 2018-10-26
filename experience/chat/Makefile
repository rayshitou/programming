include ../Make.defines

CXX = g++
CXXFLAGS = -I../lib -g -O2 -D_REENTRANT -Wall -std=c++11

PROGS = serv client

all:    ${PROGS}

serv:        serv.o chat.o
	${CXX} ${CFLAGS} -o $@ $^ ${LIBS}

client:        client.o chat.o
	${CXX} ${CFLAGS} -o $@ $^ ${LIBS}

clean:
	rm -f ${PROGS} *.o
