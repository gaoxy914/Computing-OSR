#pragma once

#include "util.h"

#include <assert.h>
#include <cmath>
#include <unordered_map>
#include <ctime>

extern "C" {
#include "glpk.h"
}


/* e = (u, v) */
struct edge_t {
	size_t u;
	size_t v;
	size_t edge_id;
	size_t ranking;

	edge_t(size_t u, size_t v, size_t edge_id, size_t ranking) : u(u), v(v), edge_id(edge_id), ranking(ranking) { }

	bool operator < (const edge_t& edge) const {
		return ranking < edge.ranking;
	}
};

class node_t {
public:
	size_t node_id;
	vector<edge_t> edges;

	size_t color;
	double lp;

	node_t(size_t node_id, size_t color = -1, double lp = -1) : node_id(node_id), color(color), lp(lp) { }

	void add_edge(size_t v, size_t edge_id, size_t ranking) {
		edges.push_back(edge_t(node_id, v, edge_id, ranking));
	}

	void sort_edge() {
		sort(edges.begin(), edges.end());
	}

	edge_t get_edge(int v_node_id) {
		for (edge_t edge : edges) if (edge.v == v_node_id) return edge;
		exit(1);
	}

	void clear() {
		edges.clear();
	}

	void remove_edge(size_t node_id) {
		vector<edge_t>::iterator iter = edges.begin();
		while (iter != edges.end()) {
			if (iter->v == node_id)
				iter = edges.erase(iter);
			else
				iter++;
		}
	}

	void show() {
		cout << "node_id: " << node_id 
			<< ", node_color: " << color << ", edges: " << endl;
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
	vector<edge_t*> edges;
	
	vector<neighbor_t*> neighbors; 

	unordered_map<size_t, bool> matching;
	// unordered_map<size_t, bool> vertex_cover;

	void add_node(const size_t& node_id) {
		nodes.push_back(new node_t(node_id));
	}

	void add_edge(const size_t& u, const size_t& v, const size_t& edge_id, const size_t& ranking) {
		nodes[u]->add_edge(v, edge_id, ranking);
		nodes[v]->add_edge(u, edge_id, ranking);
		edges.push_back(new edge_t(u, v, edge_id, ranking));
	}

	double random(double lb, double next_lb) {
		return (next_lb - lb) * (rand() / (double)RAND_MAX) + lb;
	}

	bool in_matching(const size_t& u, const edge_t& edge);

	bool in_matching_online(const size_t& u, const edge_t& edge);

	size_t sample_node() {
		return rand() % node_size;
	}

	bool in_vertexcover(const size_t& node_id);

	bool in_vertexcover_online(const size_t& node_id);

	void clear() {
		matching.clear();
		// vertex_cover.clear();
	}

	bool is_triangle(node_t* u, node_t* v, size_t& w);

	void del_triangle(const size_t& u, const size_t& v, const size_t& w);

	bool uncolored_neighbor(const size_t& u);

	void color_node(node_t* node, const size_t& color_1, const size_t& color_2);

public:

	graph_t() : node_size(0), edge_size(), max_degree(0), min_degree(0) { }

	graph_t(const size_t& number);

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

	edge_t get_edge(const size_t& edge_id) {
		return *edges[edge_id];
	}

	vector<edge_t*> get_edges() {
		return edges;
	}

	double get_lp(const size_t& node_id) {
		return nodes[node_id]->lp;
	}

	size_t get_color(const size_t& node_id) {
		return nodes[node_id]->color;
	}

	void set_lp(const size_t& no, const double& lp) {
		nodes[no - 1]->lp = lp;
	}

	void set_color(const size_t& node_id, const size_t& color) {
		nodes[node_id]->color = color;
	}

	void set_edge_size() {
		edge_size = edges.size();
	}

	pair<size_t, double> get_lowest(int node_id, int k);

	void show() {
		cout << "node_size: " << node_size
			<< ", edge_size: " << edge_size
			<< ", max_degree: " << max_degree
			<< ", min_degree: " << min_degree << endl;
		// for (int i = 0; i < nodes.size(); i++)
		//	nodes[i]->show();
		// for (int i = 0; i < edges.size(); i++)
		//	cout << edges[i]->u << " " << edges[i]->v << endl;
	}

	int vertexcover(const size_t& sample_threshold);

	int vertexcover_online(const size_t& sample_threshold);

	void eliminate_triangles(vector<size_t>& vectorcover);

	/* coloring for graph after triangle elimination */
	void coloring(size_t color_1, size_t color_2);

	void append_edges(vector<edge_t*> append_edges) {
		edges.insert(edges.end(), append_edges.begin(), append_edges.end());
	}

	int lp_solver();
};

class forests_t {
	vector<size_t> vertex_cover;

	graph_t sum_graph;
	vector<graph_t> graphs;

	size_t most_color();

	void eliminate_triangles();

	void coloring();

	void combine_graphs();

	void lp_solver();

public:

	forests_t(graph_t& graph) { sum_graph = graph; }
	
	void add_graph(graph_t graph) {
		graphs.push_back(graph);
	}

	int vertexcover();

	void show() {
		for (int i = 0; i < graphs.size(); i++)
			graphs[i].show();
	}
};

