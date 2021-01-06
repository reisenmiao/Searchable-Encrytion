#include "index_tree.h"

int Index_Tree::count = 0;

Index_Tree::Index_Tree(int n_keywords) {

	this->n_keywords = n_keywords;

	float D1[] = {0.5, 0.3, 0, 0.8};
	nodelist.push_back(create_leaf(D1, 1));

	float D2[] = {0, 0.6, 0.7, 0.3};
	nodelist.push_back(create_leaf(D2, 2));
	
	float D3[] = {0.7, 0, 0.7, 0.1};
	nodelist.push_back(create_leaf(D3, 3));

	float D4[] = {0, 1, 0, 0};
	nodelist.push_back(create_leaf(D4, 4));

	float D5[] = {0.7, 0.1, 0.7, 0.1};
	nodelist.push_back(create_leaf(D5, 5));

	float D6[] = {0.8, 0.4, 0, 0.4};
	nodelist.push_back(create_leaf(D6, 6));

	vector<Index_Node *> current_node_set, temp_node_set;

	for (vector<Index_Node *>::iterator it = nodelist.begin(); it != nodelist.end(); it++) {
		current_node_set.push_back(*it);
	}

	while (current_node_set.size() > 1) {

		int n_nodes = current_node_set.size();
		
		if (n_nodes % 2 == 0) {

			int n_pairnodes = n_nodes / 2;
			
			for (int i = 0; i < n_pairnodes ; i++) {

				Index_Node *temp = create_node(current_node_set[2 * i], current_node_set[2 * i + 1]);

				nodelist.push_back(temp);
				temp_node_set.push_back(temp);
			}

		} else {

			int n_pairnodes = n_nodes / 2;

			for (int i = 0; i < n_pairnodes - 1; i++) {

				Index_Node *temp = create_node(current_node_set[2 * i], current_node_set[2 * i + 1]);
				nodelist.push_back(temp);
				temp_node_set.push_back(temp);
			}

			Index_Node *u = create_node(current_node_set[(n_pairnodes - 1) * 2], current_node_set[(n_pairnodes - 1) * 2 + 1]);
			nodelist.push_back(u);

			Index_Node *temp = create_node(u, current_node_set[2 * n_pairnodes]);
			nodelist.push_back(temp);
			temp_node_set.push_back(temp);
		}

		current_node_set.clear();

		for(vector<Index_Node *>::iterator it = temp_node_set.begin(); it != temp_node_set.end(); it++) {
			current_node_set.push_back(*it);
		}

		temp_node_set.clear();
	}

	T = current_node_set[0];
}

vector<RScore_FID> Index_Tree::keywords_search(float *Q, int n_files) {

	this->Q = Q;
	this->n_files = n_files;

	R_list.clear();

	greedy_depth_first_search(T);

	return R_list;
}

void Index_Tree::greedy_depth_first_search(Index_Node *node) {

	float k_score = (R_list.size() < n_files) ? 0 : R_list[n_files - 1].R_score;

	if (node->FID == -1) {

		if (R_score(node) > k_score) {

			// the child node of a tree node with higher relevance score
			Index_Node* hchild = R_score(nodelist[node->P_l]) > R_score(nodelist[node->P_r]) ? nodelist[node->P_l] : nodelist[node->P_r];

			// the child node of a tree node with lower relevance score
			Index_Node* lchild = hchild == nodelist[node->P_l] ? nodelist[node->P_r] : nodelist[node->P_l];

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

int Index_Tree::gen_ID() {

	return count++;
}

Index_Node* Index_Tree::create_leaf(float *D, int FID) {

	Index_Node *node = (Index_Node *)malloc(sizeof(Index_Node));

	node->ID = gen_ID();
	node->D = (float *) malloc (n_keywords * sizeof (float));

	for (int i = 0; i < n_keywords; i++) 
		node->D[i] = D[i];

	node->P_l = -1;
	node->P_r = -1;

	node->FID = FID;

	return node;
}

Index_Node* Index_Tree::create_node(Index_Node *l, Index_Node *r) {

	Index_Node *node = (Index_Node *)malloc(sizeof(Index_Node));

	node->ID = gen_ID();
	node->D = (float *) malloc (n_keywords * sizeof(float));

	for (int i = 0; i < n_keywords; i++)
		node->D[i] = max(l->D[i], r->D[i]);

	node->P_l = l->ID;
	node->P_r = r->ID;

	node->FID = -1;

	return node;
}

void Index_Tree::print_tree(void) {

	print_tree("", T, false);
}

void Index_Tree::print_tree (const string& prefix, const Index_Node* node, bool is_left) {

	if (node != nullptr) {

		cout.setf(ios::fixed, ios::floatfield);
		cout.precision(1);

		cout << prefix;

		cout << (is_left ? "|---------" : "\\---------");

		cout << "[";

		for (int i = 0; i < n_keywords; i++) {
			
			cout << node->D[i];

			if (i < n_keywords - 1)
				cout << ", ";
		}

		cout << "]" << endl;

		print_tree(prefix + (is_left ? "|         " : "          "), (node->P_l != - 1 ? nodelist[node->P_l] : nullptr), true);
		print_tree(prefix + (is_left ? "|         " : "          "), (node->P_r != - 1 ? nodelist[node->P_r] : nullptr), false);

	}

}

float Index_Tree::R_score(Index_Node *node) {

	assert(node != nullptr);

	return to_eigen_vector(node->D, n_keywords).adjoint() * to_eigen_vector(Q, n_keywords);
}

vector<Encrypted_Index_Node *> Index_Tree::create_encrypted_tree(int* S, Eigen::MatrixXf M1, Eigen::MatrixXf M2) {

	vector<Encrypted_Index_Node *> encrypted_nodelist;

	for (vector<Index_Node *>::iterator it = nodelist.begin(); it != nodelist.end(); it++) {

		Encrypted_Index_Node * temp = (Encrypted_Index_Node *)malloc(sizeof(Encrypted_Index_Node));

		temp->ID = (*it)->ID;

		temp->D1 = (float *)malloc(n_keywords * sizeof(float));
		temp->D2 = (float *)malloc(n_keywords * sizeof(float));

		for (int i = 0; i < n_keywords; i++) {

			if (S[i] == 0) {

				temp->D1[i] = temp->D2[i] = (*it)->D[i];

			} else {

				temp->D1[i] = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				temp->D2[i] = (*it)->D[i] - temp->D1[i];
			}
		}

		Eigen::VectorXf v1 = M1.adjoint() * to_eigen_vector(temp->D1, n_keywords);
		Eigen::VectorXf v2 = M2.adjoint() * to_eigen_vector(temp->D2, n_keywords);

		for (int i = 0; i < n_keywords; i++) {

			temp->D1[i] = v1[i];
			temp->D2[i] = v2[i];

		}

		temp->P_l = (*it)->P_l;
		temp->P_r = (*it)->P_r;

		temp->FID = (*it)->FID;

		encrypted_nodelist.push_back(temp);

	}

	return encrypted_nodelist;
}

