#include "encrypted_index_tree.h"

Encrypted_Index_Tree::Encrypted_Index_Tree(int n_keywords, vector<Encrypted_Index_Node *> nodelist) {

	this->n_keywords = n_keywords;
	this->nodelist = nodelist;
	this->T = nodelist.back();
}

void Encrypted_Index_Tree::print_tree(int vector_NO) {

	print_tree("", T, false, vector_NO);
}

void Encrypted_Index_Tree::print_tree (const string& prefix, const Encrypted_Index_Node* node, bool is_left, int vector_NO) {

	if (node != nullptr) {

		cout.setf(ios::fixed, ios::floatfield);
		cout.precision(2);

		cout << prefix;

		cout << (is_left ? "|---------" : "\\---------");

		cout << "[";

		for (int i = 0; i < n_keywords; i++) {
			
			if (vector_NO == 1)
				cout << node->D1[i];
			else
				cout << node->D2[i];

			if (i < n_keywords - 1)
				cout << ", ";
		}

		cout << "]" << endl;

		print_tree(prefix + (is_left ? "|         " : "          "), (node->P_l != - 1 ? nodelist[node->P_l] : nullptr), true, vector_NO);
		print_tree(prefix + (is_left ? "|         " : "          "), (node->P_r != - 1 ? nodelist[node->P_r] : nullptr), false, vector_NO);

	}

}

vector<RScore_FID> Encrypted_Index_Tree::keywords_search(Eigen::VectorXf TD1, Eigen::VectorXf TD2, int n_files) {

	this->TD1 = TD1;
	this->TD2 = TD2;

	this->n_files = n_files;

	R_list.clear();

	greedy_depth_first_search(T);

	return R_list;
}

void Encrypted_Index_Tree::greedy_depth_first_search(Encrypted_Index_Node *node) {

	float k_score = (R_list.size() < n_files) ? 0 : R_list[n_files - 1].R_score;

	if (node->FID == -1) {

		if (R_score(node) > k_score) {

			// the child node of a tree node with higher relevance score
			Encrypted_Index_Node* hchild = R_score(nodelist[node->P_l]) > R_score(nodelist[node->P_r]) ? nodelist[node->P_l] : nodelist[node->P_r];

			// the child node of a tree node with lower relevance score
			Encrypted_Index_Node* lchild = hchild == nodelist[node->P_l] ? nodelist[node->P_r] : nodelist[node->P_l];

			greedy_depth_first_search(hchild);
			greedy_depth_first_search(lchild);

		} else {

			return ;
		}

	} else {

		if (R_score(node) > k_score) {

			RScore_FID temp = {R_score(node), node->FID};

			if (R_list.size() >= n_files) {
				R_list.pop_back();
			}

			R_list.push_back(temp);

			sort(R_list.begin(), R_list.end(), compare_r_score_fid);
		} else {

			return;
		}
	}

}

float Encrypted_Index_Tree::R_score(Encrypted_Index_Node *node) {

	assert(node != nullptr);

	float a = TD1.adjoint() * to_eigen_vector(node->D1, n_keywords);
	float b = TD2.adjoint() * to_eigen_vector(node->D2, n_keywords);

	return a + b;
}


