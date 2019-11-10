#include "oracle.h"
#include <ctime>

oracle::oracle(const char* bpt_id_path, const char* bpt_q_path, double left, double right) {
	bpt_id = bplus_tree(bpt_id_path);
	bpt_query = bplus_tree(bpt_q_path);
	this->left = left;
	this->right = right;
	bpt::key_t l(left), r(right);
	size = bpt_query.result_size(l, r, start, end, left_node_id, right_node_id);
	// cout << "size " << size << " start " << start << " end " << end << endl;
}

bool oracle::in_subgraph(const size_t& node_id) const {
//	clock_t s, e;
//	s = clock();
	data_t data;
	bpt_id.search(node_id, &data);
//	e = clock();
//	verify += (double)(e - s) / CLOCKS_PER_SEC;
//	v_n++;
	if (data.key <= right && data.key >= left)
		return true;
	return false;
	// if (node_id <= right_node_id && node_id >= left_node_id) return true;
	// return false;
}

size_t oracle::get_node_size() const {
	return size;
}

size_t oracle::sample_node() const {
//	clock_t s, e;
//	s = clock();
	size_t sample;
	// sample = rand() % (right_node_id - left_node_id + 1) + left_node_id;
	size_t number = rand() % (end - start + 1) + start;
	// cout << "number "<< number << endl;
	bpt_query.sample(number, sample);
//	e = clock();
//	s_n++;
//	this->sample += (double)(e - s) / CLOCKS_PER_SEC;
	return sample;
}

bool oracle::in_subgraph_sp(const size_t& node_id) const {
	if (node_id <= right_node_id && node_id >= left_node_id) return true;
	return false;
}

size_t oracle::sample_node_sp() const {
	size_t sample;
	sample = rand() % (right_node_id - left_node_id + 1) + left_node_id;
	return sample;
}

/*
void oracle::show()
{
	cout << "avg sample time " << sample/s_n << "s, " << "avg verify time " << verify/v_n << "s." << endl;
	sample = 0;
	s_n = 0;
	verify = 0;
	v_n = 0;
}
*/

