/* Cabecera TODO

c++11



Esquema de un algoritmo genético:
    Initial population
    Fitness function
    Selection
    Crossover
    Mutation


*/

#include <omp.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <algorithm>  // std::sort
#include <cerrno>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

/* DEFINICION DE CONSTANTES */
const int GENERACIONES = 500;
const int TAM_POBLACION = 200;
const float PROB_CRUCE = 0.9;
const float PROB_MUT = 0.1;

/* DEFINICION DE DATOS */
typedef struct {
    int np;            // personas
    int ns;            // subgrupos
    int na;            // asignaturas
    int **matriculas;  // las asignaturas cursadas por cada alumno
} Datos;

typedef struct {
    vector<int> asignaciones;  // Vector con tamaño np con valores enteros del subgrupo al que pertence
    double fitness;            // El fitness que saca depende de la función fitness()
} Individuo;

typedef struct {
    vector<Individuo> individuos = vector<Individuo>(TAM_POBLACION);
} Poblacion;

Datos d;  // global

/* Lectura desde fichero hacia 'd' */
void leer(Datos *d, char *fichero) {
    ifstream f(fichero);

    if (f.fail()) {
        cerr << "Error: " << strerror(errno) << endl;
        exit(1);
    }

    f >> d->np >> d->ns >> d->na;

    d->matriculas = new int *[d->np];
    for (int i = 0; i < d->np; i++) {
        d->matriculas[i] = new int[d->na];
    }

    for (int i = 0; i < d->np; i++) {
        for (int j = 0; j < d->na; j++) {
            f >> d->matriculas[i][j];
        }
    }

    f.close();
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
    int suma = 0;
    int max = 0;
    for (int asignacion : asignaciones) {
        if (asignacion > max) {
            max = asignacion;
        }
        suma += asignacion;
    }
    return suma * max;
    /*
    int max = 0;
    double media = 0.0;
    for (int asignacion : asignaciones) {
        media += asignacion;
        if (asignacion > max) {
            max = asignacion;
        }
    }
    media /= d.na;

    double desviacion = 0.0;
    for (int asignacion : asignaciones) {
        desviacion += abs(media - asignacion);
    }
    desviacion /= d.na;

    return desviacion + max;
    */
}

// TODO: los imprimir y tal meterlo en un utils.cpp
void imprimirDatos() {
    cout << d.np << ' ';
    cout << d.ns << ' ';
    cout << d.na << endl;
    for (int i = 0; i < d.np; i++) {
        for (int j = 0; j < d.na; j++) {
            cout << d.matriculas[i][j] << ' ';
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

vector<int> imprimirResultadoIndividuo(Individuo &individuo) {
    vector<vector<int>> subgrupos;  // todos los alumnos de cada asignatura de todos lo subgrupos

    for (int subgrupo = 0; subgrupo < d.ns; subgrupo++) {
        cout << "subgrupo " << subgrupo + 1 << " = ";
        vector<int> alumnosAsignatura;  // el nº de alumnos de cada asignatura del subgrupo
        for (int j = 0; j < d.na; j++) {
            alumnosAsignatura.push_back(0);
        }

        vector<int> &asignaciones = individuo.asignaciones;

        for (int alumno = 0; alumno < asignaciones.size(); alumno++) {
            if (subgrupo != asignaciones.at(alumno)) continue;
            for (int m = 0; m < d.na; m++) {
                alumnosAsignatura.at(m) += d.matriculas[alumno][m];
            }
        }

        subgrupos.push_back(alumnosAsignatura);
        imprimirVector(alumnosAsignatura);
        cout << endl;
    }

    vector<int> maximas;
    for (int asignatura = 0; asignatura < d.na; asignatura++) {
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
        for (int j = 0; j < d.np; j++) {
            int random = (rand() % d.ns);
            asignaciones.push_back(random);
        }
    }
}

vector<int> resultadoIndividuo(Individuo &individuo) {
    vector<vector<int>> subgrupos;  // todos los alumnos de cada asignatura de todos lo subgrupos

    for (int subgrupo = 0; subgrupo < d.ns; subgrupo++) {
        vector<int> alumnosAsignatura;  // el nº de alumnos de cada asignatura del subgrupo
        for (int j = 0; j < d.na; j++) {
            alumnosAsignatura.push_back(0);
        }

        vector<int> &asignaciones = individuo.asignaciones;

        for (int alumno = 0; alumno < asignaciones.size(); alumno++) {
            if (subgrupo != asignaciones.at(alumno)) continue;
            for (int m = 0; m < d.na; m++) {
                alumnosAsignatura.at(m) += d.matriculas[alumno][m];
            }
        }

        subgrupos.push_back(alumnosAsignatura);
    }

    vector<int> maximas;
    for (int asignatura = 0; asignatura < d.na; asignatura++) {
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

Individuo cogerMejor(Poblacion &poblacion) {
    vector<Individuo> &individuos = poblacion.individuos;
    /* Ordenamos por su fitness */
    sort(individuos.begin(), individuos.end(), [](Individuo &a, Individuo &b) { return a.fitness < b.fitness; });
    return individuos.at(0);  // con el menor fitness
}

Individuo seleccionIndividuoPorTorneo(Poblacion &poblacion) {
    const int k = 2;  // Binary tournament selection (k = 2) is most often used.
    vector<Individuo> azar;
    for (int j = 0; j < k; j++) {
        azar.push_back(poblacion.individuos.at(rand() % TAM_POBLACION));
    }
    sort(azar.begin(), azar.end(), [](Individuo &a, Individuo &b) { return a.fitness < b.fitness; });
    Individuo mejor = azar.at(0);
    return mejor;
}

vector<Individuo> seleccionPorTorneo(Poblacion &poblacion) {
    vector<Individuo> mejores;
    for (int i = 0; i < TAM_POBLACION; i++) {
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
    while (poblacionTotal < TAM_POBLACION) {
        int indexPadre = rand() % TAM_POBLACION;
        int indexMadre = rand() % TAM_POBLACION;

        Individuo &padre = poblacion.individuos.at(indexPadre);
        Individuo &madre = poblacion.individuos.at(indexMadre);

        /* 50% genes madre y 50% genes padre */
        if (uniforme() <= PROB_CRUCE) {
            Individuo hijo;
            for (int i = 0; i < d.np; i++) {
                if (uniforme() >= 0.5) {
                    hijo.asignaciones.push_back(padre.asignaciones.at(i));
                } else {
                    hijo.asignaciones.push_back(madre.asignaciones.at(i));
                }
            }
            individuos.at(poblacionTotal++) = hijo;
        }
    }
    return resultado;
}

void mutation(Poblacion &poblacion) {
    vector<Individuo> &individuos = poblacion.individuos;
    for (Individuo individuo : individuos) {
        if (uniforme() <= PROB_MUT) {
            int indice = rand() % d.np;
            individuo.asignaciones.at(indice) = rand() % d.ns;
        }
    }
}

void secuencial() {
    Poblacion poblacion;

    inicializarPoblacion(poblacion);
    medirFitness(poblacion);
    //impimirPoblacion(poblacion);

    int iteracion = 0;
    for (; iteracion < GENERACIONES; iteracion++) {
        Poblacion nuevaPoblacion;
        nuevaPoblacion.individuos = seleccionPorTorneo(poblacion);
        poblacion = crossover(nuevaPoblacion);
        mutation(poblacion);
        medirFitness(poblacion);
        //impimirPoblacion(poblacion);
        Individuo mejor = cogerMejor(poblacion);
        // cout << "ITERACION = " << iteracion << endl;
        // imprimirIndividuo(mejor);
        if (mejor.fitness == 0.0) {
            break;  // encontramos la solución óptima
        }
    }

    if (iteracion < GENERACIONES) {
        cout << "FIN ANTES DE LAS GENERACIONES" << endl;
    } else {
        cout << "TODAS LAS GENERACIONES PASADAS" << endl;
    }

    cout << "SUBGRUPOS Y DIFF MAX -> " << endl;
    Individuo mejor = cogerMejor(poblacion);
    imprimirResultadoIndividuo(mejor);
    cout << "MEJOR INDIVIDUO -> " << endl;
    imprimirIndividuo(mejor);
}

void openmp() {
    vector<Poblacion> poblaciones;
#pragma omp parallel shared(poblaciones)
    {
        Poblacion poblacion;

        inicializarPoblacion(poblacion);
        medirFitness(poblacion);

        int iteracion = 0;
        for (; iteracion < GENERACIONES / omp_get_num_threads(); iteracion++) {
            Poblacion nuevaPoblacion;
            nuevaPoblacion.individuos = seleccionPorTorneo(poblacion);
            poblacion = crossover(nuevaPoblacion);
            mutation(poblacion);
            medirFitness(poblacion);
            Individuo mejor = cogerMejor(poblacion);
            if (mejor.fitness == 0.0) {
                break;
            }
        }
        poblaciones.push_back(poblacion);
    }

    vector<Individuo> mejores;
    for (Poblacion p : poblaciones) {
        Individuo mejor = cogerMejor(p);
        mejores.push_back(mejor);
        //imprimirResultadoIndividuo(mejor);
        //imprimirIndividuo(mejor);
    }

    /* Ordenamos por su fitness */
    sort(mejores.begin(), mejores.end(), [](Individuo &a, Individuo &b) { return a.fitness < b.fitness; });

    Individuo mejor = mejores.at(0);
    imprimirResultadoIndividuo(mejor);
    cout << "MEJOR INDIVIDUO -> " << endl;
    imprimirIndividuo(mejor);

    return;
    /*
    if (iteracion < GENERACIONES) {
        cout << "FIN ANTES DE LAS GENERACIONES" << endl;
    } else {
        cout << "TODAS LAS GENERACIONES PASADAS" << endl;
    }

    cout << "SUBGRUPOS Y DIFF MAX -> " << endl;
    Individuo mejor = cogerMejor(poblacion);
    cogerMejor(poblacion);
    imprimirResultadoIndividuo(mejor);
    cout << "MEJOR INDIVIDUO -> " << endl;
    imprimirIndividuo(mejor);*/
}

long long mseconds() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000 + t.tv_usec / 1000;
}

int main(int argc, char *argv[]) {
    if (argc != 1 + 1) {
        cout << "\tUso: " << argv[0] << " [fichero] " << endl;
        return 1;
    }
    leer(&d, argv[1]);

    srand(time(NULL));

    long long ti, tf;
    ti = mseconds();
    secuencial();
    tf = mseconds();
    cout << "Tiempo secuencial: " << (tf - ti) / 1000.0 << " segundos" << endl;

    ti = mseconds();
    openmp();
    tf = mseconds();
    cout << "Tiempo paralelo: " << (tf - ti) / 1000.0 << " segundos" << endl;

    return 0;
}