CC = g++
CFLAGS = -Wall -std=c++1z -w
FLAGS = -I. -L.
DEPS = common.h error.h terminal.h directory.h command.h file_folder.h screen.h
OBJ = file_explorer.o error.o terminal.o directory.o command.o file_folder.o screen.o

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) $(FLAGS) -c -o $@ $<

file_explorer: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean :
	rm $(OBJ)