CC = g++

CFLAGS = -std=c++11

EXP1 = experiment_1
OBJ1 = bplus_tree.o graph.o oracle.o util.o experiment_1.o

EXP2 = experiment_2
OBJ2 = bplus_tree.o graph.o  oracle.o util.o experiment_2.o

all: $(EXP1) $(EXP2)

clean:
	rm *.o *.db $(EXP1) $(EXP2)

experiment_1: $(OBJ1)
	$(CC) $(CFLAGS) -o $(EXP1) $(OBJ1)

experiment_2: $(OBJ2)
	$(CC) $(CFLAGS) -o $(EXP2) $(OBJ2)

%.o: $.cpp
	$(CC) $(CFLAGS) -c $< -o $@

bplus_tree.o: util.h bplus_tree.h bplus_tree.cpp
oracle.o: bplus_tree.h oracle.h oracle.cpp
graph.o: util.h oracle.h graph.h graph.cpp
