CC=gcc
TARGET=allocator
SRC=$(wildcard *.c)
OBJ=$(patsubst %.c, %.o, $(SRC))

all : $(TARGET) 

TARGET : $(OBJ)
	$(CC) -o $@ $^

OBJ : $(SRC)
	$(CC) -o $@ -c $^

clean:
	$(RM) $(OBJ) $(TARGET)
