all: simulator

simulator: main.o heuristics.o simulator.o
		g++ main.o heuristics.o simulator.o -o simulator

main.o: main.cpp
		g++ -c main.cpp

factorial.o: heuristic.cpp
		g++ -c heuristic.cpp

hello.o: simulator.cpp
		g++ -c simulator.cpp

clean:
		rm -rf *.o simulator

