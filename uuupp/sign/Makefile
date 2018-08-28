CC=clang++
CFLAGS=-Wall -g -std=c++14 -I/usr/local/Cellar/openssl/1.0.2o_2/include 
DYFLAGS=-shared -fPIC
DEPS=-lcrypto -lssl -lstdc++ -lboost_filesystem -lboost_system -lboost_date_time -lsecp256k1 -lboost_chrono

TARGET = libsign.so
 
SRCS := $(wildcard *.cpp)  
 
OBJS := $(patsubst %cpp, %o, $(SRCS))  
 
all: $(TARGET)  
 
$(TARGET): $(OBJS)  
	$(CC) $(CFLAGS) $(DEPS) $(DYFLAGS) -o $@ $^
%.o:%.cpp
	$(CC) $(CFLAGS) -c $<
 
clean:  
	rm -f *.o $(TARGET)
