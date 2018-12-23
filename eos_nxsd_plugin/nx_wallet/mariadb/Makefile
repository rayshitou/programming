CC=g++
CFLAGS=-Wall -g -std=c++11
DEPS=-L/usr/local/lib64 -L/usr/lib64/mysql/ -lmysqlclient

TARGET=gencode
 
SUB_HEAD_DIR=$(shell find ./ -type f -name '*.hpp')
SUB_ALL_DIR=$(dir $(SUB_HEAD_DIR) )
SUB_DIR=$(sort $(SUB_ALL_DIR))
SOURCES =$(shell find ./ -type f -name  '*.cpp')

INCLUDE_PATH=include
OBJS=$(addsuffix .o , $(basename $(notdir $(SOURCES) ) ) )

VPATH = $(SUB_DIR)

all: $(TARGET)  
 
$(TARGET): $(OBJS)  
	$(CC) $(CFLAGS) $(DEPS) -o $(TARGET) $(OBJS) -lpthread

%.o:%.cpp
	$(CC) $(CFLAGS) -I$(INCLUDE_PATH) -c $< -o $@
 
clean:  
	rm -f $(OBJS) $(TARGET)
