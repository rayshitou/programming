CC=g++
CFLAGS=-Wall -g -std=c++14 -I/usr/local/Cellar/openssl/1.0.2o_2/include -lcrypto -ldl -lstdc++

TARGET = main
 
SRCS := $(wildcard *.cpp)  
 
OBJS := $(patsubst %cpp, %o, $(SRCS))  
 
all: $(TARGET)  
 
$(TARGET): $(OBJS)  
	$(CC) $(CFLAGS) -o $@ $^
%.o:%.cpp
	$(CC) $(CFLAGS) -c $<
 
clean:  
	rm -f *.o $(TARGET)
