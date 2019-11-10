#pragma once

#include "bplus_tree.h"
#include <iostream>

using bpt::bplus_tree;

class oracle
{
	bplus_tree bpt_id;
	bplus_tree bpt_query;

	double left;
	double right;

	// double verify = 0;
	// int v_n = 0;
	// double sample = 0;
	// int s_n = 0;

	size_t size;
	size_t start;
	size_t end;
	size_t left_node_id;
	size_t right_node_id;
public:
	oracle(const char* bpt_id_path, const char* bpt_q_path, double left, double right);

	bool in_subgraph(const size_t& node_id) const;
	
	size_t get_node_size() const;
	
	size_t sample_node() const;

	bool in_subgraph_sp(const size_t& node_id) const;

	size_t sample_node_sp() const;

	// void show();
};


