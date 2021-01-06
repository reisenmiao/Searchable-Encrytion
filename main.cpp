// 2020-12-22
// author: 081720120
// description: an implement of a multi-keyword ranked search scheme over encrypted cloud data.
//					it includes the TF_IDF system, the tree-based index, the greedy depth-first search and the kNN encryption.

// we don't check the correctness and properties of the TF_IDF system, as it beyond the topic of the searchable encryption.
// we assume the unencrypted dynamic multi-keyword ranked search can do what it supposed to do. 
// we only care the same search can be deduced after we encrypte it.
// and the encryption can provide real security and privacy, which it means the attacker can't get any or just get limited information from the encrypted information. 

#include "index_tree.h"
#include "encrypted_index_tree.h"

using namespace std;

int main (void) {

	float Q[] = {0, 0.92, 0, 0.38};
	int n_files = 3;
	int n_keywords = 4;

	ofstream out("log.txt");
    streambuf *coutbuf = cout.rdbuf(); 
    cout.rdbuf(out.rdbuf()); 

	cout << "*********Unencrypted Multi-Keyword Ranked Search Scheme On Plaintext*********" << endl;

	Index_Tree t(n_keywords);

	cout << "Index tree:" << endl;
	t.print_tree();
	cout << endl;

	cout.precision(2);

	cout << "The query vector:" << endl;
	print_vector(Q, n_keywords);
	cout << "\n" << endl;

	cout << "Search result:" << endl;

	vector<RScore_FID> result = t.keywords_search(Q, n_files);
	for (vector<RScore_FID>::iterator it = result.begin(); it != result.end(); it++) 
		cout << "File ID: " << it->FID << " Relevance Score: " << it->R_score << endl;

	cout << endl;

	cout << "*********Encrypted Multi-Keyword Ranked Search Scheme On Ciphertext**********" << endl;

	// create the secret key
	int *S = (int *)malloc(4 * sizeof(int));

	for (int i = 0; i < n_keywords; i++)
		S[i] = rand() % 2;

	Eigen::MatrixXf M1 = Eigen::MatrixXf::Random(n_keywords, n_keywords);

	while (M1.determinant() == 0) {

		M1 = Eigen::MatrixXf::Random(n_keywords, n_keywords);
	}

	Eigen::MatrixXf M2 = Eigen::MatrixXf::Random(n_keywords, n_keywords);

	while (M2.determinant() == 0) {

		M2 = Eigen::MatrixXf::Random(n_keywords, n_keywords);
	}

	cout << "S:";
	for (int i = 0; i < n_keywords; i++)
		cout << S[i];
	cout << endl << endl;

	cout << "M1:\n" << M1 << endl << endl;

	cout << "M2:\n" << M2 << endl << endl;

	vector<Encrypted_Index_Node *> nodelist = t.create_encrypted_tree(S, M1, M2);

	Encrypted_Index_Tree e_t(n_keywords, nodelist);

	cout << "Vector D1 of the encrypted index tree:" << endl;
	e_t.print_tree(1);
	cout << endl;

	cout << "Vector D2 of the encrypted index tree:" << endl;
	e_t.print_tree(2); 
	cout << "\n";

	float *temp1 = (float *)malloc(n_keywords * sizeof(float));
	float *temp2 = (float *)malloc(n_keywords * sizeof(float));

	for (int i = 0; i < n_keywords; i++) {

		if (S[i] == 0) {

			temp1[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			temp2[i] = Q[i] - temp1[i];

		} else {

			temp1[i] = temp2[i] = Q[i];
		}
	}

	Eigen::VectorXf trapdoor1 = M1.inverse() * to_eigen_vector(temp1, n_keywords);
	Eigen::VectorXf trapdoor2 = M2.inverse() * to_eigen_vector(temp2, n_keywords);

	cout << "Trapdoor(the encrypted form of the query vector):" << endl;
	print_vector(trapdoor1, n_keywords);
	cout << ", ";
	print_vector(trapdoor1, n_keywords);
	cout << endl << endl;

	cout << "Search result on ciphertext:" << endl;

	vector<RScore_FID> e_result = e_t.keywords_search(trapdoor1, trapdoor2, n_files);

	for (vector<RScore_FID>::iterator it = e_result.begin(); it != e_result.end(); it++) 
		cout << "File ID: " << it->FID << " Relevance Score: " << it->R_score << endl;

	cout << endl;

	std::cout.rdbuf(coutbuf);
	return 0;
}

