all: secuencial.cpp
	g++ -std=c++11 secuencial.cpp -o secuencial.out

clean:
	rm *.out

run: all
	time ./secuencial.out $(file)