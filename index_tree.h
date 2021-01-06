#ifndef INDEX_TREE_H
#define INDEX_TREE_H

// the structure of KBB index tree

// node:	<ID, D, P_l, P_r, FID>
// ID:		unique identity ID for each node
// D: 		a vector consisting of normalized TF values of the keywords
// P_l:		the left child of the node
// P_r:		the right child of the node 
// FID:		file ID. If the node u is an internal node, FID is set to null.

#include "data_type.h"

class Index_Tree {

public:

	Index_Tree(int n_keywords);

	void print_tree();

	vector<RScore_FID> keywords_search(float *Q, int n_files);	

	vector<Encrypted_Index_Node *> create_encrypted_tree(int* S, Eigen::MatrixXf M1, Eigen::MatrixXf M2);

private:

	int n_keywords;

	Index_Node *T;
	vector<Index_Node *> nodelist;

	Index_Node* create_leaf (float* D, int FID);
	Index_Node* create_node (Index_Node *l, Index_Node *r);

	static int count;
	int gen_ID();

	void print_tree(const string& prefix, const Index_Node* node, bool is_left);

	float *Q;
	int n_files;
	vector<RScore_FID> R_list;
	void greedy_depth_first_search(Index_Node *node);

	float R_score(Index_Node *node);

};

#endif
