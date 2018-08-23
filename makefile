CC = g++
CFLAGS = -Wall -std=c++1z
DEPS = print.h common.h
OBJ = print.o

%.o: %.cpp ($DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

fileExplorer: $(OBJ)
	$(CC) $(FLAGS) -o $@ $^