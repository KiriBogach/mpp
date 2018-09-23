
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