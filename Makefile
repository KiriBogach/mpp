all: secuencial.cpp omp.cpp mpi.cpp mpiopenmp.cpp
	g++ -std=c++11 secuencial.cpp -o secuencial.out
	g++ -std=c++11 omp.cpp -o omp.out -fopenmp
	mpic++ -std=c++11 mpi.cpp -o mpi.out
	mpic++ -w -std=c++11 mpiopenmp.cpp -o mpiopenmp.out -fopenmp

secuencial: secuencial.cpp
	g++ -std=c++11 secuencial.cpp -o secuencial.out

omp: omp.cpp
	g++ -std=c++11 omp.cpp -o omp.out -fopenmp

mpi: mpi.cpp
	mpic++ -w -std=c++11 mpi.cpp -o mpi.out

mpiopenmp: mpiopenmp.cpp
	mpic++ -w -std=c++11 mpiopenmp.cpp -o mpiopenmp.out -fopenmp

generador: generador.cpp
	g++ -std=c++11 generador.cpp -o generador.out

generador_perfecto: generador_perfecto.cpp
	g++ -std=c++11 generador_perfecto.cpp -o generador_perfecto.out

generar: generador
	./generador.out $(np) $(ng) $(na) $(p_matriculacion) > generado.txt

generar_perfecto: generador_perfecto
	./generador_perfecto.out $(np) $(ng) $(na) > generado_perfecto.txt

run_secuencial: secuencial
	./secuencial.out < $(file)
	@echo -e

run_omp: omp
	./omp.out < $(file)
	@echo -e

run_mpi: mpi
	mpirun -np $(np) mpi.out < $(file)
	@echo -e

run_mpiopenmp: mpiopenmp
	mpirun -np $(np) mpiopenmp.out < $(file)
	@echo -e
	
benchmark: run_secuencial run_omp run_mpi run_mpiopenmp

clean:
	rm *.out