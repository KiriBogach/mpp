#include <stdlib.h>
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

const int TAM_POBLACION = 100;
const float PROB_CRUCE = 0.8;
const float PROB_MUT = 0.1;

struct Datos {
    int np;  // personas
    int ns;  // subgrupos
    int na;  // asignaturas
    int **matricula;
};

struct Datos d;

void leer(struct Datos *d, char *fichero) {
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

/*
 (1, 2, 2, 0)
 La media es 5/4
 ((5/4-1) + (2-5/4) + (2-5/4) + (5/4-0)) / 4 = 0.75
*/
double fitness(int *array) {
    double media = 0.0;
    for (int i = 0; i < d.na; i++) {
        media += array[i];
    }
    media /= d.na;

    float desviacion = 0.0;
    for (int i = 0; i < d.na; i++) {
        desviacion += abs(media - array[i]);
    }
    desviacion /= d.na;

    return desviacion;
}

struct Individuo {
    double fitness;
    vector<int> v;
};

struct Poblacion {
    vector<Individuo> individuos = vector<Individuo>(TAM_POBLACION);
};

// Uso de vectores con copy?
void crossover() {
}

int main(int argc, char *argv[]) {
    if (argc != 1 + 1) {
        cout << "\tUso: " << argv[0] << " [fichero] [tam_poblacion] [prob_cruce] [prob_mut] [selector]" << endl;
        cout << "\tSelector: {0 = Ruleta, 1 = Torneo}" << endl;
        return 1;
    }
    leer(&d, argv[1]);

    cout << d.np << ' ';
    cout << d.ns << ' ';
    cout << d.na << endl;
    for (int i = 0; i < d.np; i++) {
        for (int j = 0; j < d.na; j++) {
            cout << d.matricula[i][j] << ' ';
        }
        cout << endl;
    }

    Poblacion poblacion;
    for (int i = 0; i < poblacion.individuos.size(); i++) {
        poblacion.individuos.at(i).v = vector<int>(d.na);
    }

    //int array[] = {1, 2, 2, 0};
    //cout << fitness(array) << endl;
    return 0;
}

/*
    Initial population
    Fitness function
    Selection
    Crossover
    Mutation
*/
