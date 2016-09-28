#ifndef _MYFILT_H
#define _MYFILT_H

using namespace std;

#include "../include/read_ckt.h" //needed for the round function

vector<double> fir_filtering(vector<double> coef, vector<double> inp, int M, double* Apx, int type);
double fulladd_signedRCA(double A, double B, int N, int N_apx, int type);
vector<int> fulladd_call(int A, int B, int c_in, int type);
void destroyEntry(double *entry);

#endif


