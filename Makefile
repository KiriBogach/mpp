all: secuencial.cpp omp.cpp mpi.cpp omp_mpi.cpp
	g++ -std=c++11 secuencial.cpp -o secuencial.out
	g++ -std=c++11 omp.cpp -o omp.out -fopenmp
	mpic++ -std=c++11 mpi.cpp -o mpi.out
	mpic++ -w -std=c++11 omp_mpi.cpp -o omp_mpi.out -fopenmp

secuencial: secuencial.cpp
	g++ -std=c++11 secuencial.cpp -o secuencial.out

omp: omp.cpp
	g++ -std=c++11 omp.cpp -o omp.out -fopenmp

mpi: mpi.cpp
	mpic++ -w -std=c++11 mpi.cpp -o mpi.out

omp_mpi: omp_mpi.cpp
	mpic++ -w -std=c++11 omp_mpi.cpp -o omp_mpi.out -fopenmp

generador: generador.cpp
	g++ -std=c++11 generador.cpp -o generador.out

generar: all generador
	./generador.out $(np) $(ng) $(na) $(p_matriculacion) > temp.txt
	/usr/bin/time --format "%E" -a ./secuencial.out < temp.txt
	/usr/bin/time --format "%E" -a ./omp.out < temp.txt
	rm temp.txt

run_secuencial: secuencial
	./secuencial.out < $(file)
	@echo -e

run_omp: omp
	./omp.out < $(file)
	@echo -e

run_mpi: mpi
	mpirun -np $(np) mpi.out < $(file)
	@echo -e

run_omp_mpi: omp_mpi
	mpirun -np $(np) omp_mpi.out < $(file)
	@echo -e
	
benchmark: run_secuencial run_omp run_mpi run_omp_mpi

clean:
	rm *.out