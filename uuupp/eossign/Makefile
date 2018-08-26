CC=clang
CXX=clang++
CFLAGS=-Wall -g
CXXFLAGS=-Wall -g -std=c++14  -I/usr/local/Cellar/openssl/1.0.2o_2/include 

DEPS=sign.hpp
OBJ=main.o fc.o crypto.o io.o log.o
SRC = ./src
INC = ./include

SOURCE = $(wildcard ${SRC}/*.cpp)
SOURCE_IO = $(wildcard ${SRC}/io/*.cpp)
SOURCE_LOG = $(wildcard ${SRC}/log/*.cpp)
SOURCE_CRYPTO = $(wildcard ${SRC}/crypto/*.cpp)
OBJECT = $(patsubst %.cpp,${OBJ}/%.o,$(notdir ${SOURCE}))


RM=rm -f

# compile only, C source
#%.o: %.c
#	$(CC) -c -o $@ $< $(CFLAGS)

# compile only, C++ source
%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)
	
%.o: ${SOURCE}
	$(CXX) -c -o $@ $< $(CXXFLAGS)

#fc.o: ${SOURCE}
#	$(CXX) -c -o $@ $< $(CXXFLAGS)

#io.o: ${SOURCE_IO}
#	$(CXX) -c -o $@ $< $(CXXFLAGS)

#log.o: ${SOURCE_LOG}
#	$(CXX) -c -o $@ $< $(CXXFLAGS)

#crypto.o: ${SOURCE_CRYPTO}
#	$(CXX) -c -o $@ $< $(CXXFLAGS)

# link
main: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS)

clean:
	$(RM) $(OBJ)
