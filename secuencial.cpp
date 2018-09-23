/* Cabecera TODO

c++11

*/

#include <stdlib.h>
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

const int GENERACIONES = 10;
const int TAM_POBLACION = 100;
const float PROB_CRUCE = 0.8;
const float PROB_MUT = 0.1;

typedef struct {
    int np;  // personas
    int ns;  // subgrupos
    int na;  // asignaturas
    int **matricula;
} Datos;

typedef struct {
    vector<int> asignaciones;  // Vector con tamaño np con valores enteros del subgrupo al que pertence
    double fitness;            // El fitness que saca depende de la función fitness()
} Individuo;

typedef struct {
    vector<Individuo> individuos = vector<Individuo>(TAM_POBLACION);
} Poblacion;

Datos d;  // global

void leer(Datos *d, char *fichero) {
    ifstream f(fichero);

    if (f.fail()) {
        cerr << "Error: " << strerror(errno) << endl;
        exit(1);
    }

    f >> d->np >> d->ns >> d->na;

    d->matricula = new int *[d->np];
    for (int i = 0; i < d->np; i++) {
        d->matricula[i] = new int[d->na];
    }

    for (int i = 0; i < d->np; i++) {
        for (int j = 0; j < d->na; j++) {
            f >> d->matricula[i][j];
        }
    }

    f.close();
}

// TODO: los imprimir y tal meterlo en un utils.cpp
void imprimirDatos() {
    cout << d.np << ' ';
    cout << d.ns << ' ';
    cout << d.na << endl;
    for (int i = 0; i < d.np; i++) {
        for (int j = 0; j < d.na; j++) {
            cout << d.matricula[i][j] << ' ';
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

/*
 (1, 2, 2, 0)
 La media es 5/4
 ((5/4-1) + (2-5/4) + (2-5/4) + (5/4-0)) / 4 = 0.75
*/
double desviacionTipicaRespectoMedia(vector<int> &asignaciones) {
    double media = 0.0;
    for (int asignacion : asignaciones) {
        media += asignacion;
    }
    media /= d.na;

    float desviacion = 0.0;
    for (int asignacion : asignaciones) {
        desviacion += abs(media - asignacion);
    }
    desviacion /= d.na;

    return desviacion;
}

void inicializarPoblacion(Poblacion &poblacion) {
    for (int i = 0; i < poblacion.individuos.size(); i++) {
        vector<int> &asignaciones = poblacion.individuos.at(i).asignaciones;
        for (int j = 0; j < d.np; j++) {
            int random = (rand() % d.na);
            asignaciones.push_back(random);
        }
    }
}

// vector copy
void crossover() {
}

vector<int> resultadoIndividuo(Individuo &individuo) {
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
                alumnosAsignatura.at(m) += d.matricula[alumno][m];
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
    cout << endl;

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

bool ordenarPorFitness(Individuo a, Individuo b) { return a.fitness < b.fitness; }

void cogerMejores(Poblacion &poblacion) {
    vector<Individuo> &individuos = poblacion.individuos;
    sort(individuos.begin(), individuos.end(), ordenarPorFitness);
    Individuo &mejor = individuos.at(0);
    imprimirIndividuo(mejor);
    cout << endl;
    resultadoIndividuo(mejor);
}

/*
    Initial population
    Fitness function
    Selection
    Crossover
    Mutation
*/

int main(int argc, char *argv[]) {
    if (argc != 1 + 1) {
        cout << "\tUso: " << argv[0] << " [fichero] [tam_poblacion] [prob_cruce] [prob_mut] [selector]" << endl;
        cout << "\tSelector: {0 = Ruleta, 1 = Torneo}" << endl;
        return 1;
    }
    leer(&d, argv[1]);

    Poblacion poblacion;

    inicializarPoblacion(poblacion);
    medirFitness(poblacion);
    impimirPoblacion(poblacion);

    cogerMejores(poblacion);

    //impimirPoblacion(poblacion);

    /*
    for (int i = 0; i < GENERACIONES; i++) {
        medirFitness(poblacion);
    }*/

    return 0;
}