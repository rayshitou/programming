include /home/ccyrhel65/cplusplus/dev/network/unix-network/Make.defines

CXX = g++
CXXFLAGS = -I../lib -I/home/ccyrhel65/cplusplus/dev/network/unix-network/lib -g -O2 -D_REENTRANT -Wall -std=c++11
NETCONN_LIBS = /home/ccyrhel65/cplusplus/dev/network/unix-network/libunp.a
LIBS = -lpthread

PROGS = serv client

all:    ${PROGS}

serv:        serv.o chat.o
	${CXX} ${CFLAGS} -o $@ $^ ${LIBS} ${NETCONN_LIBS}

client:        client.o chat.o
	${CXX} ${CFLAGS} -o $@ $^ ${LIBS} ${NETCONN_LIBS}

clean:
	rm -f ${PROGS} *.o
