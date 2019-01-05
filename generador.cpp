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
    if (argc != 4 + 1) {
        cout << "\tUso: " << argv[0] << " [np] [ns] [na] [p_matriculacion]" << endl;
        return 1;
    }

    srand(time(NULL));

    int np = atoi(argv[1]);
    int ns = atoi(argv[2]);
    int na = atoi(argv[3]);
    double p_matriculacion = atof(argv[4]);

    cout << np << " " << ns << " " << na << endl;
    for (int i = 0; i < np; i++) {
        for (int j = 0; j < na; j++) {
            if (uniforme() <= p_matriculacion) {
                cout << 1 << " ";
            } else {
                cout << 0 << " ";
            }
        }
        cout << endl;
    }
}