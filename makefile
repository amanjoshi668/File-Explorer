CC = g++
CFLAGS = -Wall -std=c++1z
FLAGS = -I. -L.
DEPS = common.h error.h
OBJ = file_explorer.o error.o

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) $(FLAGS) -c -o $@ $<

file_explorer: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $<

clean :
	rm $(OBJ)