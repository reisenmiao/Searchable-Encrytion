#ifndef ENCRYPTED_INDEX_TREE_H
#define ENCRYPTED_INDEX_TREE_H

// the structure of KBB index tree

// node:	<ID, D, P_l, P_r, FID>
// ID:		unique identity ID for each node
// D: 		a vector consisting of normalized TF values of the keywords
// P_l:		the left child of the node
// P_r:		the right child of the node 
// FID:		file ID. If the node u is an internal node, FID is set to null.

#include "data_type.h"

class Encrypted_Index_Tree {

public:

	Encrypted_Index_Tree(int n_keywords, vector<Encrypted_Index_Node *> nodelist);

	void print_tree(int vector_NO);

	vector<RScore_FID> keywords_search(Eigen::VectorXf TD1, Eigen::VectorXf TD2, int n_files);	

private:

	int n_keywords;

	Encrypted_Index_Node *T;
	vector<Encrypted_Index_Node *> nodelist;

	void print_tree(const string& prefix, const Encrypted_Index_Node* node, bool is_left, int vector_NO);

	Eigen::VectorXf TD1;
	Eigen::VectorXf TD2;

	int n_files;
	vector<RScore_FID> R_list;
	void greedy_depth_first_search(Encrypted_Index_Node *node);

	float R_score(Encrypted_Index_Node *node);

};

#endif
