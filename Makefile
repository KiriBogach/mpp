all: secuencial.cpp
	g++ -std=c++11 secuencial.cpp -o secuencial.out

generador: generador.cpp
	g++ -std=c++11 generador.cpp -o generador.out

generar: all generador
	./generador.out $(np) $(ns) $(na) $(p_matriculacion) > temp.txt
	./secuencial.out temp.txt
	rm temp.txt

clean:
	rm *.out

run: all
	./secuencial.out $(file)