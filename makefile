all:main

main: main.cpp kruskal.hpp prim.hpp funcs.hpp Graph.hpp
	g++ -o main main.cpp
	
clean:
	rm -f main
