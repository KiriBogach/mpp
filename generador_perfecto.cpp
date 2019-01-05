#include <stdlib.h>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>

using namespace std;

double uniforme() {
    return rand() % 100 / 100.0;
}

int main(int argc, char *argv[]) {
    if (argc != 3 + 1) {
        cout << "\tUso: " << argv[0] << " [np] [ns] [na]" << endl;
        return 1;
    }

    srand(time(NULL));

    int np = atoi(argv[1]);
    int ns = atoi(argv[2]);
    int na = atoi(argv[3]);

    int matriculados_alumno[np][na];
    for (int i = 0; i < np; i++) {
        for (int j = 0; j < na; j++) {
            matriculados_alumno[i][j] = 0;
        }
    }

    cout << np << " " << ns << " " << na << endl;

    int n_personas = np / ns;
    int suma_max = ((np / 2) + rand() % (np / 2)) / ns;
    for (int i = 0; i < ns; i++) {
        int suma_asignaturas[n_personas][na];
        for (int j = 0; j < n_personas; j++) {
            for (int k = 0; k < na; k++) {
                suma_asignaturas[j][k] = 0;
            }
        }
        for (int k = 0; k < na; k++) {
            int suma_actual = 0;
            for (int j = 0; j < n_personas; j++) {
                if (suma_actual < suma_max) {
                    if (n_personas - j > suma_max - suma_actual) {
                        if (uniforme() <= 0.5) {
                            suma_asignaturas[j][k] = 1;
                            suma_actual++;
                        } else {
                            suma_asignaturas[j][k] = 0;
                        }
                    } else {
                        suma_actual++;
                        suma_asignaturas[j][k] = 1;
                    }

                } else {
                    suma_asignaturas[j][k] = 0;
                }
            }
        }

        for (int j = 0; j < n_personas; j++) {
            for (int k = 0; k < na; k++) {
                cout << suma_asignaturas[j][k] << " ";
            }
            cout << endl;
        }
    }
}