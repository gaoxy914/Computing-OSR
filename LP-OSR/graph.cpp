#include "graph.h"

bool compare(pair<int, double> pair_a, pair<int, double> pair_b) {
	return pair_a.second < pair_b.second;
}

bool graph_t::in_matching(const size_t& u, const edge_t& edge) {
	if (matching.count(edge.edge_id) != 0)
		return matching[edge.edge_id];
	node_t* node_u = nodes[u];
	node_t* node_v = nodes[edge.v];
	int k_1 = 0, k_2 = 0;
	while (node_u->edges[k_1] < edge || node_v->edges[k_2] < edge) {
		if (node_u->edges[k_1] < node_v->edges[k_2]) {
			if (in_matching(u, node_u->edges[k_1])) {
				matching[edge.edge_id] = false;
				return false;
			}
			++k_1;
		}
		else {
			if (in_matching(edge.v, node_v->edges[k_2])) {
				matching[edge.edge_id] = false;
				return false;
			}
			++k_2;
		}
	}
	matching[edge.edge_id] = true;
	return true;
}


bool graph_t::in_matching_online(const size_t& u, const edge_t& edge)
{
	if (matching.count(edge.edge_id) != 0)
		return matching[edge.edge_id];
	size_t k_1 = 1, k_2 = 1;
	pair<size_t, double> u_edge = get_lowest(u, k_1);
	pair<size_t, double> v_edge = get_lowest(edge.v, k_2);
	while (u_edge.first != edge.v || v_edge.first != u) {
		if (u_edge.second < v_edge.second) {
			if (in_matching_online(u, nodes[u]->get_edge(u_edge.first))) {
				matching[edge.edge_id] = false;
				return false;
			}
			u_edge = get_lowest(u, ++k_1);
		}
		else {
			if (in_matching_online(edge.v, nodes[edge.v]->get_edge(v_edge.first))) {
				matching[edge.edge_id] = false;
				return false;
			}
			v_edge = get_lowest(edge.v, ++k_2);
		}
	}
	matching[edge.edge_id] = true;
	return true;
}

bool graph_t::in_vertexcover(const size_t& node_id) {
//	if (vertex_cover.count(node_id) != 0)
//		return vertex_cover[node_id];
	node_t* n = nodes[node_id];
	for (edge_t edge : n->edges) {
		if (in_matching(node_id, edge)) {
//			vertex_cover[node_id] = true;
			return true;
		}
	}
//	vertex_cover[node_id] = false;
	return false;
}

bool graph_t::in_vertexcover_online(const size_t& node_id)
{
//	if (vertex_cover.count(node_id) != 0)
//		return vertex_cover[node_id];
	node_t* n = nodes[node_id];
	size_t i = 1;
	pair<size_t, double> neighbor = get_lowest(node_id, i);
	while (neighbor.second != INFINITY) {
		if (in_matching_online(node_id, n->get_edge(neighbor.first))) {
//			vertex_cover[node_id] = true;
			return true;
		}
		neighbor = get_lowest(node_id, ++i);
	}
//	vertex_cover[node_id] = false;
	return false;
}

bool graph_t::is_triangle(node_t* u, node_t* v, size_t& w) {
	for (edge_t edge_u : u->edges)
		for (edge_t edge_v : v->edges)
			if (edge_u.v == edge_v.v) {
				w = edge_u.v;
				return true;
			}

	return false;
}

void graph_t::del_triangle(const size_t& u, const size_t& v, const size_t& w) {
	nodes[u]->clear();
	nodes[v]->clear();
	nodes[w]->clear();
	for (node_t* node : nodes) {
		node->remove_edge(u);
		node->remove_edge(v);
		node->remove_edge(w);
	}
	vector<edge_t*>::iterator iter = edges.begin();
	while (iter != edges.end()) {
		if ((*iter)->u == u || (*iter)->v == u)
			iter = edges.erase(iter);
		else if ((*iter)->u == v || (*iter)->v == v)
			iter = edges.erase(iter);
		else if ((*iter)->u == w || (*iter)->v == w)
			iter = edges.erase(iter);
		else
			iter++;
	}
}

bool graph_t::uncolored_neighbor(const size_t& u) {
	vector<edge_t> edges = nodes[u]->edges;
	for (int i = 0; i < edges.size(); i++) {
		if (nodes[edges[i].v]->color == -1)
			return true;
	}
	return false;
}
void graph_t::color_node(node_t* node, const size_t& color_1, const size_t& color_2) {
	vector<node_t*> nodes_1, nodes_2;
	nodes_1.push_back(node);
	while (nodes_1.size() > 0) {
		for (int i = 0; i < nodes_1.size(); i++) {
			nodes_1[i]->color = color_1;
			vector<edge_t> edges = nodes_1[i]->edges;
			for (int j = 0; j < edges.size(); j++)
				if (nodes[edges[j].v]->color == -1)
					nodes_2.push_back(nodes[edges[j].v]);
		}
		for (int i = 0; i < nodes_2.size(); i++) {
			nodes_2[i]->color = color_2;
			vector<edge_t> edges = nodes_2[i]->edges;
			for (int j = 0; j < edges.size(); j++)
				if (nodes[edges[j].v]->color == -1)
					nodes_1.push_back(nodes[edges[j].v]);
		}
	}
}
			

graph_t::graph_t(const size_t& number) {
	node_size = number;
	min_degree = edge_size = max_degree = 0;
	for (size_t node_id = 0; node_id < number; ++node_id)
		add_node(node_id);
}

graph_t::graph_t(const char* path, const size_t& number,
	const double& rho, bool (*is_conflict)(const data_t&, const data_t&)) {
	// clock_t start, end;
	// double sum = 0;
	size_t clean_number = number * (1 - rho);
	size_t dirty_number = number - clean_number;

	unsigned int seed = (unsigned int)time(NULL);
	RandomSequenceOfUnique rsu(seed, seed + 1);
	node_size = min_degree = number;
	edge_size = max_degree = 0;

	for (size_t node_id = 0; node_id < number; ++node_id)
		add_node(node_id);

	/*FILE* fp = fopen(path, "rb");
	data_t data_l, data_r;
	off_t offset_l = OFFSET_DATA, offset_r = OFFSET_DATA + sizeof(data_t);
	for (size_t i = 0; i < number - 1; ++i) {
		start = clock();
		fseek(fp, offset_l, SEEK_SET);
		fread(&data_l, sizeof(data_t), 1, fp);
		end = clock();
		sum += (double)(end - start) / CLOCKS_PER_SEC;
		for (size_t j = i + 1; j < number; ++j) {
			// fseek(fp, offset_r, SEEK_SET);
			start = clock();
			fread(&data_r, sizeof(data_t), 1, fp);
			end = clock();
			sum += (double)(end - start) / CLOCKS_PER_SEC;
			if (is_conflict(data_l, data_r)) {
				// data_l.print();
				// data_r.print();
				add_edge(data_l.id, data_r.id, edge_size++, rsu.next());
				if (edge_size % 10000000 == 0)
					cout << "processing tuple: " << data_l.id << ", the number of added edges: " << edge_size / 1000000 << "M." << endl;
			}
			// offset_r += sizeof(data_t);
		}
		offset_l += sizeof(data_t);
		// offset_r = offset_l + sizeof(data_t);
		nodes[data_l.id]->sort_edge();
		int degree = nodes[data_l.id]->edges.size();
		if (degree > max_degree) max_degree = degree;
		if (degree < min_degree) min_degree = degree;
	}
	fclose(fp);*/

	data_t datas_l[DATA_BATCH], datas_r[DATA_BATCH];
	off_t offset_l = OFFSET_DATA, offset_r = OFFSET_DATA + sizeof(data_t) * DATA_BATCH;
	size_t round = ceil((double)number / DATA_BATCH), cur_node = 0,
		dirty_round = ceil((double)dirty_number / DATA_BATCH);
	for (size_t i = 0; i < round; i++) {
		//	start = clock();
		int size_l = read_data(path, offset_l, DATA_BATCH, datas_l);
		// end = clock();
		// sum += (double)(end - start) / CLOCKS_PER_SEC;
		for (int p = 0; p < size_l; p++)
			for (int q = p + 1; q < size_l; q++)
				if (is_conflict(datas_l[p], datas_l[q])) {
					// cout << datas_l[p].id << " " << datas_l[q].id << endl;
					// datas_l[p].print();
					// datas_l[q].print();
					add_edge(datas_l[p].id, datas_l[q].id, edge_size++, rsu.next());
				}


		for (size_t j = i + 1; j < round; j++) {
			// start = clock();
			int size_r = read_data(path, offset_r, DATA_BATCH, datas_r);
			// end = clock();
			// sum += (double)(end - start) / CLOCKS_PER_SEC;
			for (int p = 0; p < size_l; p++)
				for (int q = 0; q < size_r; q++)
					if (is_conflict(datas_l[p], datas_r[q])) {
						// cout << datas_l[p].id << " " << datas_r[q].id << endl;
						// datas_l[p].print();
						// datas_r[q].print();
						add_edge(datas_l[p].id, datas_r[q].id, edge_size++, rsu.next());
					}

			offset_r += sizeof(data_t) * DATA_BATCH;
		}

		offset_l += sizeof(data_t) * DATA_BATCH;
		offset_r = offset_l + DATA_BATCH * sizeof(data_t);
	}

	for (size_t i = 0; i < node_size; i++) {
		nodes[i]->sort_edge();
		int degree = nodes[i]->edges.size();
		if (degree > max_degree) max_degree = degree;
		if (degree < min_degree) min_degree = degree;
	}

	for (size_t node_id = 0; node_id < number; ++node_id)
		neighbors.push_back(new neighbor_t(max_degree, *nodes[node_id]));

	// std::cout << "file operation time consumption : " << sum << "s.\n";
	show();
}

pair<size_t, double> graph_t::get_lowest(int node_id, int k) {
	neighbor_t* nei = neighbors[node_id];
	node_t node = get_node(node_id);
	while (nei->sorted.size() < k && nei->lower_bound < 1) {
		vector<pair<size_t, double>> S;
		unordered_map<size_t, double>::iterator iter = nei->assigned_number.begin();
		for (; iter != nei->assigned_number.end(); iter++) {
			if (iter->second > nei->lower_bound&& iter->second <= nei->next_lower_bound)
				S.push_back(pair<size_t, double>(iter->first, iter->second));
		}
		vector<size_t> T;
		for (size_t no = 0; no < node.edges.size(); ++no)
			if (rand() / (double)RAND_MAX < (nei->next_lower_bound - nei->lower_bound) / (1 - nei->lower_bound))
				T.push_back(no);
		for (size_t t : T) {
			size_t w_node_id = node.edges[t].v;
			double ranking = random(nei->lower_bound, nei->next_lower_bound);
			neighbor_t* w_nei = neighbors[w_node_id];
			if (w_nei->lower_bound <= nei->lower_bound) {
				if (nei->assigned_number[w_node_id] == NONE) {
					nei->set_value(w_node_id, ranking);
					w_nei->set_value(node_id, ranking);
					S.push_back(pair<size_t, double>(w_node_id, ranking));
				}
			}
		}
		sort(S.begin(), S.end(), compare);
		nei->append_sorted(S);
		nei->set_lower_bound();
	}
	if (nei->sorted.size() < k) return pair<int, double>(node_id, INFINITY);
	return nei->sorted[k - 1];
}

int graph_t::vertexcover(const size_t& sample_threshold) {
	size_t sample_number = 0;
	size_t vc_size = 0;
	while (sample_number < sample_threshold) {
		size_t node_id = sample_node();
		if (in_vertexcover(node_id))
			vc_size++;
		sample_number++;
	}
	clear();
	// cout << vc_size << endl;
	return vc_size * node_size / sample_threshold;
}

int graph_t::vertexcover_online(const size_t& sample_threshold) {
	size_t sample_number = 0;
	size_t vc_size = 0;
	while (sample_number < sample_threshold)
	{
		size_t node_id = sample_node();
		// node.show();
		if (in_vertexcover_online(node_id))
			vc_size++;
		sample_number++;
	}
	clear();
	return vc_size * node_size / sample_threshold;
}


void graph_t::eliminate_triangles(vector<size_t>& vertexcover) {
	vector<edge_t*>::iterator iter = edges.begin();
	size_t sum = 0;
	for (; iter != edges.end();) {
		node_t* u = nodes[(*iter)->u], * v = nodes[(*iter)->v];
		if (u->edges.size() != 0 && v->edges.size() != 0) {
			size_t w;
			if (is_triangle(u, v, w)) {
				sum++;
				del_triangle(u->node_id, v->node_id, w);
				vertexcover.push_back(u->node_id);
				vertexcover.push_back(v->node_id);
				vertexcover.push_back(w);
				iter = edges.begin();
			} else
				iter++;
		} else {
			iter++;
		}
	}
	cout << "eliminate triangles: " << sum << endl;
//	cout << edge_size << " " << edges.size() << endl;
	/*node_size = nodes.size();
	edge_size = edges.size();
	min_degree = max_degree;
	max_degree = 0;
	for (size_t i = 0; i < nodes.size(); i++) {
		if (nodes[i]->edges.size() > max_degree)
			max_degree = nodes[i]->edges.size();

		if (nodes[i]->edges.size() < min_degree)
			min_degree = nodes[i]->edges.size();
	}*/
	// show();
}

void graph_t::coloring(size_t color_1, size_t color_2) {
	for (size_t i = 0; i < node_size; i++) {
		if (nodes[i]->color == -1) {
			vector<node_t*> nodes_1, nodes_2;
			nodes_1.push_back(nodes[i]);
			while (nodes_1.size() > 0) {
				for (int i = 0; i < nodes_1.size(); i++) {
					nodes_1[i]->color = color_1;
					vector<edge_t> edges = nodes_1[i]->edges;
					for (int j = 0; j < edges.size(); j++)
						if (nodes[edges[j].v]->color == -1)
							nodes_2.push_back(nodes[edges[j].v]);
				}
				nodes_1.clear();
				for (int i = 0; i < nodes_2.size(); i++) {
					nodes_2[i]->color = color_2;
					vector<edge_t> edges = nodes_2[i]->edges;
					for (int j = 0; j < edges.size(); j++)
						if (nodes[edges[j].v]->color == -1)
							nodes_1.push_back(nodes[edges[j].v]);
				}
				nodes_2.clear();
			}
		}
	}	
}


size_t forests_t::most_color() {
	unordered_map<size_t, size_t> counts;
	for (size_t i = 0; i < sum_graph.get_node_size(); i++) {
	//	cout << sum_graph.get_color(i) << endl;
		if (sum_graph.get_lp(i) == 0.5)
			if (counts.count(sum_graph.get_color(i)) == 0)
				counts[sum_graph.get_color(i)] = 1;
			else
				counts[sum_graph.get_color(i)] += 1;
	}
	unordered_map<size_t, size_t>::iterator iter = counts.begin();
	size_t max = 0, color = 0;
	for (; iter != counts.end(); iter++) {
		cout << iter->first << "," << iter->second << endl;
		if (iter->second > max) {
			max = iter->second;
			color = iter->first;
		}
	}
	cout << color << "," << max << endl;
	return color;
}

void forests_t::eliminate_triangles() {
	for (int i = 0; i < graphs.size(); i++)
		graphs[i].eliminate_triangles(vertex_cover);

	sort(vertex_cover.begin(), vertex_cover.end());
	vertex_cover.erase(unique(vertex_cover.begin(), vertex_cover.end()), vertex_cover.end());
}

void forests_t::coloring() {
	size_t color_1 = 0, color_2 = 1;
	for (int i = 0; i < graphs.size(); i++)
		graphs[i].coloring(color_1, color_2);
}

void forests_t::combine_graphs() {
	for (size_t i = 0; i < sum_graph.get_node_size(); i++) {
		size_t color = 0;
		for (int j = 0; j < graphs.size(); j++) {
			color += pow(2, j) * graphs[j].get_color(i);
		}
		sum_graph.set_color(i, color);
	}

	for (graph_t g : graphs)
		sum_graph.append_edges(g.get_edges());

	sum_graph.set_edge_size();
}

void forests_t::lp_solver() {
	int m = sum_graph.get_edge_size();
	int n = sum_graph.get_node_size();
initialize:
	glp_prob* lp;
	lp = glp_create_prob();
	glp_set_obj_dir(lp, GLP_MIN);

auxiliary_variables_rows:
	glp_add_rows(lp, m);
	for (int i = 1; i <= m; i++)
		glp_set_row_bnds(lp, i, GLP_LO, 1.0, 0.0);

variables_columns:
	glp_add_cols(lp, n);
	for (int i = 1; i <= n; i++)
		glp_set_col_bnds(lp, i, GLP_DB, 0.0, 1.0);

to_minimize:
	for (int i = 1; i <= n; i++)
		glp_set_obj_coef(lp, i, 1.0);

constrant_matrix:
	int size = m * n;
	int* ia = new int[size + 1];
	int* ja = new int[size + 1];
	double* ar = new double[size + 1];
	for (int i = 1; i <= m; i++) {
		edge_t edge = sum_graph.get_edge(i - 1);
		for (int j = 1; j <= n; j++) {
			int k = (i - 1) * n + j;
			ia[k] = i;
			ja[k] = j;
			if (j - 1 == edge.u || j - 1 == edge.v)
				ar[k] = 1.0;
			else
				ar[k] = 0.0;
		}
	}
	glp_load_matrix(lp, size, ia, ja, ar);

calculate:
	glp_simplex(lp, NULL);
	// glp_exact(lp, NULL);

output:
	cout << glp_get_obj_val(lp) << endl;
	for (int i = 1; i <= n; i++) {
		sum_graph.set_lp(i, glp_get_col_prim(lp, i));
	}
		
cleanup:
	glp_delete_prob(lp);
}

int forests_t::vertexcover() {
	eliminate_triangles();
	coloring();
	combine_graphs();
	lp_solver();
	size_t color = most_color();
//	cout << color << endl;
//	cout << vertex_cover.size() << endl;
	for (size_t i = 0; i < sum_graph.get_node_size(); i++) {
		if (sum_graph.get_lp(i) == 1)
			vertex_cover.push_back(i);
		if (sum_graph.get_lp(i) == 0.5 && sum_graph.get_color(i) != color)
			vertex_cover.push_back(i);
	}
//	cout << vertex_cover.size() << endl;
	sort(vertex_cover.begin(), vertex_cover.end());
	vertex_cover.erase(unique(vertex_cover.begin(), vertex_cover.end()), vertex_cover.end());
//	cout << vertex_cover.size() << endl;
	return vertex_cover.size();
}
