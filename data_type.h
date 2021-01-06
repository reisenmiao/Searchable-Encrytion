#ifndef DATA_TYPE_H
#define DATA_TYPE_H

#include <iostream>
#include <iomanip>

#include <fstream>

#include <vector>
#include <algorithm>

#include <Eigen/Dense>

#include <assert.h>

#include <cstdlib>
#include <ctime>

using namespace std;

typedef struct Index_Node {

	int ID;
	float *D;
	int P_l;
	int P_r;
	int FID;
}Index_Node;

typedef struct Encrypted_Index_Node {

	int ID;

	float *D1;
	float *D2;

	int P_l;
	int P_r;
	int FID;

}Encrypted_Index_Node;

typedef struct RScore_FID {

	float R_score;
	int FID;
}RScore_FID;

bool compare_r_score_fid(RScore_FID a, RScore_FID b);

Eigen::VectorXf to_eigen_vector(float *v, int n); 

void print_vector(Eigen::VectorXf v, int n);
void print_vector(float *v, int n);

#endif
