CC = g++
CFLAGS = -std=c++11 -lglpk

EXP = experiment
OBJ = util.o graph.o experiment.cpp

experiment: $(OBJ)
	$(CC) $(CFLAGS) -o $(EXP) $(OBJ)

graph.o: util.h graph.h graph.cpp
	$(CC) $(CFLAGS) -c graph.cpp

util.o: util.h util.cpp
	$(CC) $(CFLAGS) -c util.cpp

clean:
	rm *.o *.db $(EXP)
