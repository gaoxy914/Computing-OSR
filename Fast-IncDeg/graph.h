#pragma once

#include "oracle.h"
#include "util.h"

#include <cmath>
#include <unordered_map>
#include <ctime>

/* e = (u, v) */
struct edge_t {
	size_t v;
	size_t edge_id;
	size_t ranking;

	edge_t(size_t v, size_t edge_id, size_t ranking) : v(v), edge_id(edge_id), ranking(ranking) { }

	bool operator < (const edge_t& edge) const {
		return ranking < edge.ranking;
	}
};

class node_t {
public:
	size_t node_id;
	vector<edge_t> edges;

	node_t() : node_id(0) { }

	node_t(size_t node_id) : node_id(node_id) { }

	void add_edge(size_t v, size_t edge_id, size_t ranking) {
		edges.push_back(edge_t(v, edge_id, ranking));
	}

	void sort_edge() {
		sort(edges.begin(), edges.end());
	}

	edge_t get_edge(int v_node_id) {
		for (edge_t edge : edges) if (edge.v == v_node_id) return edge;
		exit(1);
	}

	void show() {
		cout << "node_id: " << node_id << ", edges: " << endl;
		for (edge_t edge : edges) cout << edge.v << "," << edge.ranking << ",";
		cout << endl;
	}
};

const double NONE = 0;

class neighbor_t {
public:
	double lower_bound;
	double next_lower_bound;
	unordered_map<size_t, double> assigned_number; // <node_id, ranking>
	vector<pair<size_t, double>> sorted; // <node_id, ranking>

	neighbor_t(size_t max_degree, node_t node) {
		lower_bound = 0;
		next_lower_bound = pow(2, -ceil(log2(max_degree)));
		for (edge_t edge : node.edges)
			assigned_number[edge.v] = NONE;
	}

	void set_lower_bound() { lower_bound = next_lower_bound; next_lower_bound *= 2; }

	void set_value(size_t node_id, double ranking) {
		assigned_number[node_id] = ranking;
	}

	void append_sorted(vector<pair<size_t, double>> append_sorted) {
		sorted.insert(sorted.end(), append_sorted.begin(), append_sorted.end());
	}
};

class graph_t {
	size_t node_size;
	size_t edge_size;
	size_t max_degree;
	size_t min_degree;
	vector<node_t*> nodes;
	vector<neighbor_t*> neighbors;

	unordered_map<size_t, bool> matching;
	unordered_map<size_t, bool> vertex_cover;

	void add_node(const size_t& node_id) {
		nodes.push_back(new node_t(node_id));
	}

	void add_edge(const size_t& u, const size_t& v, const size_t& edge_id, const size_t& ranking) {
		nodes[u]->add_edge(v, edge_id, ranking);
		nodes[v]->add_edge(u, edge_id, ranking);
	}

	double random(double lb, double next_lb) {
		return (next_lb - lb) * (rand() / (double)RAND_MAX) + lb;
	}

	bool in_matching(const size_t& u, const edge_t& edge);
	bool in_matching(const size_t& u, const edge_t& edge, const oracle& oracle);
	bool in_matching_sp(const size_t& u, const edge_t& edge, const oracle& oracle);

	bool in_matching_online(const size_t& u, const edge_t& edge);
	bool in_matching_online(const size_t& u, const edge_t& edge, const oracle& oracle);
	bool in_matching_online_sp(const size_t& u, const edge_t& edge, const oracle& oracle);

	size_t sample_node() {
		return rand() % node_size;
	}

	bool in_vertexcover(const size_t& node_id);
	bool in_vertexcover(const size_t& node_id, const oracle& oracle);
	bool in_vertexcover_sp(const size_t& node_id, const oracle& oracle);

	bool in_vertexcover_online(const size_t& node_id);
	bool in_vertexcover_online(const size_t& node_id, const oracle& oracle);
	bool in_vertexcover_online_sp(const size_t& node_id, const oracle& oracle);

	void clear() {
		matching.clear();
		vertex_cover.clear();
	}

public:
	graph_t(const char* path, const size_t& number, const double& rho,
		bool (*is_conflict)(const data_t&, const data_t&) = is_conflict);

	size_t get_node_size() const {
		return node_size;
	}

	size_t get_edge_size() const {
		return edge_size;
	}

	size_t get_max_degree() const {
		return max_degree;
	}

	size_t get_min_degree() const {
		return min_degree;
	}

	node_t get_node(const size_t& node_id) {
		return *nodes[node_id];
	}

	// edge_t get_edge(const size_t& edge_id);

	pair<size_t, double> get_lowest(int node_id, int k);

	void show() {
		cout << "node_size: " << node_size
			<< "\nedge_size: " << edge_size
			<< "\nmax_degree: " << max_degree
			<< "\nmin_degree: " << min_degree
			<< "\naverage_dgree: " << 2 * edge_size / node_size
			<< endl;
		// for (int i = 0; i < nodes.size(); i++)
		//	nodes[i]->show();
	}

	int vertexcover(const size_t& sample_threshold);

	int vertexcover_online(const size_t& sample_threshold);

	int subgraph_vertexcover(const size_t& sample_threshold, const oracle& oracle);

	int subgraph_vertexcover_sp(const size_t& sample_threshold, const oracle& oracle);

	int subgraph_vertexcover_online(const size_t& sample_threshold, const oracle& oracle);

	int subgraph_vertexcover_online_sp(const size_t& sample_threshold, const oracle& oracle);
};
