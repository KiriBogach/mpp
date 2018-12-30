/* Cabecera TODO

c++11



Esquema de un algoritmo genético:
    Initial population
    Fitness function
    Selection
    Crossover
    Mutation


*/

#include <mpi.h>
#include <omp.h>
#include <sys/time.h>
#include <time.h>
#include <algorithm>
#include <cerrno>
#include <climits>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <vector>

using namespace std;

/* DEFINICION DE VARIABLES DEL PROBLEMA */
int np;            // Personas
int ng;            // Grupos
int na;            // Asignaturas
int *asignaturas;  // Las asignaturas cursadas por cada alumno

/* DEFINICION DE VARIABLES DEL ALGORITMO */
// Nº iteraciones para buscar la mejor solución.
#define GENERACIONES 1000
int generaciones;

// Nº individiuos de una población.
#define TAM_POBLACION 200
int tam_poblacion;

// Probabilidad de cruce de una pareja
#define P_CRUCE 0.9
float p_cruce;

// Probabilidad de mutación de genes
#define P_MUT 0.1
float p_mut;

/* DEFINICION DE ESTRUCTURAS DE DATOS */

typedef struct {
    vector<int> asignaciones;  // Vector con tamaño np con valores enteros del subgrupo al que pertence
    double fitness;            // El fitness que saca depende de la función fitness()
} Individuo;

typedef struct {
    vector<Individuo> individuos = vector<Individuo>(tam_poblacion);
} Poblacion;

/* FUNCIONES ÚTILES */
long long mseconds() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000 + t.tv_usec / 1000;
}

/* Devuelve un número decimal al azar entre [0, 1] */
double uniforme() {
    return rand() % 100 / 100.0;
}

/*
 Dado el vector [1, 2, 2, 0] =>
    La media es 5/4
    ((5/4-1) + (2-5/4) + (2-5/4) + (5/4-0)) / 4 = 0.75
*/
double desviacionTipicaRespectoMedia(vector<int> &asignaciones) {
    int max = 0;
    double media = 0.0;
    for (int asignacion : asignaciones) {
        media += asignacion;
        if (asignacion > max) {
            max = asignacion;
        }
    }
    media /= na;

    double desviacion = 0.0;
    for (int asignacion : asignaciones) {
        desviacion += abs(media - asignacion);
    }
    desviacion /= na;

    return desviacion + max;
}

int maximaDiferencia(vector<int> v) {
    int max = INT_MIN;
    int min = INT_MAX;

    for (int elem : v) {
        if (elem > max) {
            max = elem;
        }
        if (elem < min) {
            min = elem;
        }
    }

    return max - min;
}

void leer() {
    cin >> np >> ng >> na;
    asignaturas = new int[np * na];
    for (int i = 0; i < np; i++) {
        for (int j = 0; j < na; j++) {
            int index = i * na + j;
            cin >> asignaturas[index];
        }
    }
}

/* FUNCIONES PARA DEBUGGIN */
void imprimirDatos() {
    cout << np << ' ';
    cout << ng << ' ';
    cout << na << endl;
    for (int i = 0; i < np; i++) {
        for (int j = 0; j < na; j++) {
            int index = i * na + j;
            cout << asignaturas[index] << ' ';
        }
        cout << endl;
    }
}

void imprimirVector(vector<int> v, bool incremento = false) {
    cout << "[ ";
    for (int elem : v) {
        int toPrint = (incremento) ? elem + 1 : elem;
        cout << toPrint << " ";
    }
    cout << "]";
}

void imprimirIndividuo(Individuo &individuo) {
    vector<int> &asignaciones = individuo.asignaciones;
    imprimirVector(asignaciones, true);
    cout << " fitness = " << individuo.fitness << endl;
}

void impimirPoblacion(Poblacion poblacion) {
    for (int i = 0; i < poblacion.individuos.size(); i++) {
        cout << "individuo " << i + 1 << " = ";
        Individuo &individuo = poblacion.individuos.at(i);
        imprimirIndividuo(individuo);
    }
    cout << endl;
}

vector<int> imprimirResultadoIndividuo(Individuo &individuo) {
    vector<vector<int>> subgrupos;  // todos los alumnos de cada asignatura de todos lo subgrupos

    for (int subgrupo = 0; subgrupo < ng; subgrupo++) {
        cout << "subgrupo " << subgrupo + 1 << " = ";
        vector<int> alumnosAsignatura;  // el nº de alumnos de cada asignatura del subgrupo
        for (int j = 0; j < na; j++) {
            alumnosAsignatura.push_back(0);
        }

        vector<int> &asignaciones = individuo.asignaciones;

        for (int alumno = 0; alumno < asignaciones.size(); alumno++) {
            if (subgrupo != asignaciones.at(alumno)) continue;
            for (int m = 0; m < na; m++) {
                int index = alumno * na + m;
                alumnosAsignatura.at(m) += asignaturas[index];
            }
        }

        subgrupos.push_back(alumnosAsignatura);
        imprimirVector(alumnosAsignatura);
        cout << endl;
    }

    vector<int> maximas;
    for (int asignatura = 0; asignatura < na; asignatura++) {
        vector<int> diferencias;
        for (vector<int> v : subgrupos) {
            diferencias.push_back(v.at(asignatura));
        }
        maximas.push_back(maximaDiferencia(diferencias));
    }

    cout << endl
         << "diff max = ";
    imprimirVector(maximas);
    cout << endl
         << endl;

    return maximas;
}

void imprimirFitness(Individuo &individuo) {
    vector<int> resultado = imprimirResultadoIndividuo(individuo);
    individuo.fitness = desviacionTipicaRespectoMedia(resultado);
}
void imprimirMedirFitness(Poblacion &poblacion) {
    for (int i = 0; i < poblacion.individuos.size(); i++) {
        Individuo &individuo = poblacion.individuos.at(i);
        imprimirFitness(individuo);
    }
}

void inicializarPoblacion(Poblacion &poblacion) {
    for (int i = 0; i < poblacion.individuos.size(); i++) {
        vector<int> &asignaciones = poblacion.individuos.at(i).asignaciones;
        for (int j = 0; j < np; j++) {
            int random = (rand() % ng);
            asignaciones.push_back(random);
        }
    }
}

/* FUNCIONALIDAD INTRÍNSECA */

vector<int> resultadoIndividuo(Individuo &individuo) {
    vector<vector<int>> subgrupos;  // todos los alumnos de cada asignatura de todos lo subgrupos

    for (int subgrupo = 0; subgrupo < ng; subgrupo++) {
        vector<int> alumnosAsignatura;  // el nº de alumnos de cada asignatura del subgrupo
        for (int j = 0; j < na; j++) {
            alumnosAsignatura.push_back(0);
        }

        vector<int> &asignaciones = individuo.asignaciones;

        for (int alumno = 0; alumno < asignaciones.size(); alumno++) {
            if (subgrupo != asignaciones.at(alumno)) continue;
            for (int m = 0; m < na; m++) {
                int index = alumno * na + m;
                alumnosAsignatura.at(m) += asignaturas[index];
            }
        }

        subgrupos.push_back(alumnosAsignatura);
    }

    vector<int> maximas;
    for (int asignatura = 0; asignatura < na; asignatura++) {
        vector<int> diferencias;
        for (vector<int> v : subgrupos) {
            diferencias.push_back(v.at(asignatura));
        }
        maximas.push_back(maximaDiferencia(diferencias));
    }

    return maximas;
}

void fitness(Individuo &individuo) {
    vector<int> resultado = resultadoIndividuo(individuo);
    individuo.fitness = desviacionTipicaRespectoMedia(resultado);
}

void medirFitness(Poblacion &poblacion) {
    for (int i = 0; i < poblacion.individuos.size(); i++) {
        Individuo &individuo = poblacion.individuos.at(i);
        fitness(individuo);
    }
}

bool ordenacion(Individuo a, Individuo b) {
    return a.fitness < b.fitness;
}

Individuo cogerMejor(Poblacion &poblacion) {
    vector<Individuo> &individuos = poblacion.individuos;
    /* Ordenamos por su fitness */
    sort(individuos.begin(), individuos.end(), ordenacion);
    return individuos.at(0);  // con el menor fitness
}

Individuo seleccionIndividuoPorTorneo(Poblacion &poblacion) {
    const int k = 2;  // Binary tournament selection (k = 2) is most often used.
    vector<Individuo> azar;
    for (int j = 0; j < k; j++) {
        azar.push_back(poblacion.individuos.at(rand() % tam_poblacion));
    }
    sort(azar.begin(), azar.end(), ordenacion);
    Individuo mejor = azar.at(0);
    return mejor;
}

vector<Individuo> seleccionPorTorneo(Poblacion &poblacion) {
    vector<Individuo> mejores;
    for (int i = 0; i < tam_poblacion; i++) {
        Individuo mejor = seleccionIndividuoPorTorneo(poblacion);
        mejores.push_back(mejor);
        // OJO podemos llegar a seleccionar mejores iguales
    }
    return mejores;
}

/* The probability of crossover is the probability that crossover will occur at a particular mating */
Poblacion crossover(Poblacion &poblacion) {
    Poblacion resultado;
    vector<Individuo> &individuos = resultado.individuos;
    int poblacionTotal = 0;
    while (poblacionTotal < tam_poblacion) {
        int indexPadre = rand() % tam_poblacion;
        int indexMadre = rand() % tam_poblacion;

        Individuo &padre = poblacion.individuos.at(indexPadre);
        Individuo &madre = poblacion.individuos.at(indexMadre);

        // Se cruzan en función de la probabilidad de cruce
        if (uniforme() <= p_cruce) {
            Individuo hijo;
            Individuo hija;
            for (int i = 0; i < np; i++) {
                /* 50% genes madre y 50% genes padre */
                if (uniforme() >= 0.5) {
                    hijo.asignaciones.push_back(padre.asignaciones.at(i));
                    hija.asignaciones.push_back(madre.asignaciones.at(i));
                } else {
                    hijo.asignaciones.push_back(madre.asignaciones.at(i));
                    hija.asignaciones.push_back(padre.asignaciones.at(i));
                }
            }
            individuos.at(poblacionTotal++) = hijo;
            if (poblacionTotal < tam_poblacion) {
                individuos.at(poblacionTotal++) = hija;
            }
        } else {
            individuos.at(poblacionTotal++) = padre;
            if (poblacionTotal < tam_poblacion) {
                individuos.at(poblacionTotal++) = madre;
            }
        }
    }
    return resultado;
}

void mutation(Poblacion &poblacion) {
    vector<Individuo> &individuos = poblacion.individuos;
    for (Individuo individuo : individuos) {
        if (uniforme() <= p_mut) {
            int indice = rand() % np;
            individuo.asignaciones.at(indice) = rand() % ng;
        }
    }
}

double omp_mpi(int np, int ng, int na, int *asignaturas, int generaciones, int tam_poblacion, double p_cruce, double p_mut) {
    vector<Poblacion> poblaciones;
    int iteraciones = generaciones / omp_get_max_threads();
#pragma omp parallel shared(poblaciones) firstprivate(iteraciones) num_threads(2)
    {
        Poblacion poblacion;

        inicializarPoblacion(poblacion);
        medirFitness(poblacion);

        for (int iteracion = 0; iteracion < iteraciones; iteracion++) {
            Poblacion nuevaPoblacion;
            nuevaPoblacion.individuos = seleccionPorTorneo(poblacion);
            poblacion = crossover(nuevaPoblacion);
            mutation(poblacion);
            medirFitness(poblacion);
            Individuo mejor = cogerMejor(poblacion);
            if (mejor.fitness == 0.0) {
                break;  // Termina el hilo
            }
        }

        poblaciones.push_back(poblacion);
    }

    vector<Individuo> mejores;
    for (Poblacion p : poblaciones) {
        Individuo mejor = cogerMejor(p);
        mejores.push_back(mejor);
    }

    /* Ordenamos por su fitness */
    sort(mejores.begin(), mejores.end(), ordenacion);
    Individuo mejor = mejores.at(0);

    return mejor.fitness;
}

int main(int argc, char *argv[]) {
    /* Inicialización de números pseudoaleatorios */
    srand(time(NULL));

    /* Inicialización MPI */
    int nodo, procesos, ROOT = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &procesos);
    MPI_Comm_rank(MPI_COMM_WORLD, &nodo);

    generaciones = GENERACIONES / procesos;

    /* Lectura y compartición de datos */
    if (nodo == ROOT) {
        leer();
        int parametros[] = {np, ng, na};
        MPI_Bcast(parametros, 3, MPI_INT, ROOT, MPI_COMM_WORLD);
        MPI_Bcast(asignaturas, np * na, MPI_INT, ROOT, MPI_COMM_WORLD);
    } else {
        int parametros[3];
        MPI_Bcast(parametros, 3, MPI_INT, ROOT, MPI_COMM_WORLD);
        np = parametros[0];
        ng = parametros[1];
        na = parametros[2];
        asignaturas = new int[np * na];
        MPI_Bcast(asignaturas, np * na, MPI_INT, ROOT, MPI_COMM_WORLD);
    }

    /* Parámetros del algoritmo genético */

    tam_poblacion = TAM_POBLACION;
    p_cruce = P_CRUCE;
    p_mut = P_MUT;

    /* Tiempos de ejecución */
    long long ti, tf;

    /* Ejecución secuencial */
    ti = mseconds();
    double fitness = omp_mpi(np, ng, na, asignaturas, generaciones, tam_poblacion, p_cruce, p_mut);
    tf = mseconds();

    if (nodo != ROOT) {
        MPI_Send(&fitness, 1, MPI_DOUBLE, ROOT, 20, MPI_COMM_WORLD);
    } else {
        double mejor_fitness = fitness;
        for (int proceso = 1; proceso < proceso; proceso++) {
            int fitness_recibido;
            MPI_Recv(&fitness_recibido, 1, MPI_DOUBLE, proceso, 20, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (fitness_recibido < mejor_fitness) {
                mejor_fitness = fitness_recibido;
            }
        }
        cout << "Tiempo OMP_MPI: " << (tf - ti) / 1000.0 << " segundos" << endl;
        cout << "Fitness obtenido: " << mejor_fitness << endl;
    }

    MPI_Finalize();
    return 0;
}