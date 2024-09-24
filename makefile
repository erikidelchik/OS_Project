all:main

main: main.cpp kruskal.hpp prim.hpp funcs.hpp Graph.hpp
	g++ -fprofile-arcs -ftest-coverage -pg -g -o main main.cpp
	
valgrind: main
	valgrind --tool=memcheck --leak-check=full ./main > valgrind_memcheck_report.txt 2>&1 

helgrind: main
	valgrind --tool=helgrind ./main > helgrind_report 2>&1
	
callgrind: main
	valgrind --tool=callgrind ./main > valgrind_callgraph_report.txt 2>&1 
	
	
clean:
	rm -f main main.gcno *.gcov *.gcda *.out
