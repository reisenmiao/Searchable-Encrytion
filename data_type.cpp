#include "data_type.h"

bool compare_r_score_fid(RScore_FID a, RScore_FID b) {

	return a.R_score > b.R_score;
}

Eigen::VectorXf to_eigen_vector(float *v, int n) {

	Eigen::VectorXf u(n);

	for (int i = 0; i < n; i++) {
		u[i] = v[i];
	}

	return u;
}

void print_vector(float *v, int n) {

	cout << "[";
	for (int i = 0; i < n; i++) {

		cout << v[i];

		if(i < n - 1)
			cout << ", ";
	}
	cout << "]";
}

void print_vector(Eigen::VectorXf v, int n) {

	cout << "[";
	for (int i = 0; i < n; i++) {

		cout << v[i];

		if(i < n - 1)
			cout << ", ";
	}
	cout << "]";
}

