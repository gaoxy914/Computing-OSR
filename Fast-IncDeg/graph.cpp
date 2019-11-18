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
		} else {
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

bool graph_t::in_matching(const size_t& u, const edge_t& edge, const oracle& oracle) {
	if (matching.count(edge.edge_id) != 0)
		return matching[edge.edge_id];
	node_t* node_u = nodes[u];
	node_t* node_v = nodes[edge.v];
	int k_1 = 0, k_2 = 0;
	while (node_u->edges[k_1] < edge || node_v->edges[k_2] < edge) {
		if (node_u->edges[k_1] < node_v->edges[k_2]) {
			if (oracle.in_subgraph(node_u->edges[k_1].v)) {
				if (in_matching(u, node_u->edges[k_1], oracle)) {
					matching[edge.edge_id] = false;
					return false;
				}
			}
			++k_1;
		} else {
			if (oracle.in_subgraph(node_v->edges[k_2].v)) {
				if (in_matching(edge.v, node_v->edges[k_2], oracle)) {
					matching[edge.edge_id] = false;
					return false;
				}
			}
			++k_2;
		}
	}
	matching[edge.edge_id] = true;
	return true;
}

bool graph_t::in_matching_sp(const size_t& u, const edge_t& edge, const oracle& oracle)
{
	if (matching.count(edge.edge_id) != 0)
		return matching[edge.edge_id];
	node_t* node_u = nodes[u];
	node_t* node_v = nodes[edge.v];
	int k_1 = 0, k_2 = 0;
	while (node_u->edges[k_1] < edge || node_v->edges[k_2] < edge) {
		if (node_u->edges[k_1] < node_v->edges[k_2]) {
			if (oracle.in_subgraph_sp(node_u->edges[k_1].v)) {
				if (in_matching_sp(u, node_u->edges[k_1], oracle)) {
					matching[edge.edge_id] = false;
					return false;
				}
			}
			++k_1;
		}
		else {
			if (oracle.in_subgraph_sp(node_v->edges[k_2].v)) {
				if (in_matching_sp(edge.v, node_v->edges[k_2], oracle)) {
					matching[edge.edge_id] = false;
					return false;
				}
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

bool graph_t::in_matching_online(const size_t& u, const edge_t& edge, const oracle& oracle) {
	if (matching.count(edge.edge_id) != 0)
		return matching[edge.edge_id];
	size_t k_1 = 1, k_2 = 1;
	pair<size_t, double> u_edge = get_lowest(u, k_1);
	pair<size_t, double> v_edge = get_lowest(edge.v, k_2);
	while (u_edge.first != edge.v || v_edge.first != u) {
		if (u_edge.second < v_edge.second) {
			if (oracle.in_subgraph(u_edge.first)) {
				if (in_matching_online(u, nodes[u]->get_edge(u_edge.first), oracle)) {
					matching[edge.edge_id] = false;
					return false;
				}
			}
			u_edge = get_lowest(u, ++k_1);
		} else {
			if (oracle.in_subgraph(v_edge.first)) {
				if (in_matching_online(edge.v, nodes[edge.v]->get_edge(v_edge.first), oracle)) {
					matching[edge.edge_id] = false;
					return false;
				}
			}
			v_edge = get_lowest(edge.v, ++k_2);
		}
	}
	matching[edge.edge_id] = true;
	return true;
}

bool graph_t::in_matching_online_sp(const size_t& u, const edge_t& edge, const oracle& oracle) {
	if (matching.count(edge.edge_id) != 0)
		return matching[edge.edge_id];
	size_t k_1 = 1, k_2 = 1;
	pair<size_t, double> u_edge = get_lowest(u, k_1);
	pair<size_t, double> v_edge = get_lowest(edge.v, k_2);
	while (u_edge.first != edge.v || v_edge.first != u) {
		if (u_edge.second < v_edge.second) {
			if (oracle.in_subgraph_sp(u_edge.first)) {
				if (in_matching_online_sp(u, nodes[u]->get_edge(u_edge.first), oracle)) {
					matching[edge.edge_id] = false;
					return false;
				}
			}
			u_edge = get_lowest(u, ++k_1);
		}
		else {
			if (oracle.in_subgraph_sp(v_edge.first)) {
				if (in_matching_online_sp(edge.v, nodes[edge.v]->get_edge(v_edge.first), oracle)) {
					matching[edge.edge_id] = false;
					return false;
				}
			}
			v_edge = get_lowest(edge.v, ++k_2);
		}
	}
	matching[edge.edge_id] = true;
	return true;
}

bool graph_t::in_vertexcover(const size_t& node_id) {
	if (vertex_cover.count(node_id) != 0)
		return vertex_cover[node_id];
	node_t* n = nodes[node_id];
	for (edge_t edge : n->edges) {
		if (in_matching(node_id, edge)) {
			vertex_cover[node_id] = true;
			return true;
		}
	}
	vertex_cover[node_id] = false;
	return false;
}

bool graph_t::in_vertexcover(const size_t& node_id, const oracle& oracle) {
	if (vertex_cover.count(node_id) != 0)
		return vertex_cover[node_id];
	node_t* n = nodes[node_id];
	for (edge_t edge : n->edges) {
		if (oracle.in_subgraph(edge.v)) {
			if (in_matching(node_id, edge, oracle)) {
				vertex_cover[node_id] = true;
				return true;
			}
		}
	}
	vertex_cover[node_id] = false;
	return false;
}

bool graph_t::in_vertexcover_sp(const size_t& node_id, const oracle& oracle)
{
	if (vertex_cover.count(node_id) != 0)
		return vertex_cover[node_id];
	node_t* n = nodes[node_id];
	for (edge_t edge : n->edges) {
		if (oracle.in_subgraph_sp(edge.v)) {
			if (in_matching_sp(node_id, edge, oracle)) {
				vertex_cover[node_id] = true;
				return true;
			}
		}
	}
	vertex_cover[node_id] = false;
	return false;
}

bool graph_t::in_vertexcover_online(const size_t& node_id)
{
	if (vertex_cover.count(node_id) != 0)
		return vertex_cover[node_id];
	node_t* n = nodes[node_id];
	size_t i = 1;
	pair<size_t, double> neighbor = get_lowest(node_id, i);
	while (neighbor.second != INFINITY) {
		if (in_matching_online(node_id, n->get_edge(neighbor.first))) {
			vertex_cover[node_id] = true;
			return true;
		}
		neighbor = get_lowest(node_id, ++i);
	}
	vertex_cover[node_id] = false;
	return false;
}

bool graph_t::in_vertexcover_online(const size_t& node_id, const oracle& oracle) {
	if (vertex_cover.count(node_id) != 0)
		return vertex_cover[node_id];
	node_t* n = nodes[node_id];
	size_t i = 1;
	pair<size_t, double> neighbor = get_lowest(node_id, i);
	while (neighbor.second != INFINITY) {
		if (oracle.in_subgraph(neighbor.first)) {
			if (in_matching_online(node_id, n->get_edge(neighbor.first), oracle)) {
				vertex_cover[node_id] = true;
				return true;
			}
		}
		neighbor = get_lowest(node_id, ++i);
	}
	vertex_cover[node_id] = false;
	return false;
}

bool graph_t::in_vertexcover_online_sp(const size_t& node_id, const oracle& oracle)
{
	if (vertex_cover.count(node_id) != 0)
		return vertex_cover[node_id];
	node_t* n = nodes[node_id];
	size_t i = 1;
	pair<size_t, double> neighbor = get_lowest(node_id, i);
	while (neighbor.second != INFINITY) {
		if (oracle.in_subgraph_sp(neighbor.first)) {
			if (in_matching_online_sp(node_id, n->get_edge(neighbor.first), oracle)) {
				vertex_cover[node_id] = true;
				return true;
			}
		}
		neighbor = get_lowest(node_id, ++i);
	}
	vertex_cover[node_id] = false;
	return false;
}

graph_t::graph_t(const char* path, const size_t& number, const double& rho,
	bool(*is_conflict)(const data_t&, const data_t&)) {
	// clock_t start, end;
	// double sum = 0;

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

	size_t clean = number * (1 - rho);
	size_t dirty = number - clean;
	size_t round = ceil((double)number / DATA_BATCH), dirty_round = ceil((double)dirty / DATA_BATCH);
	for (size_t i = 0; i < round; i++) {
	//	start = clock();
		int size_l =  read_data(path, offset_l, DATA_BATCH, datas_l);
		// end = clock();
		// sum += (double)(end - start) / CLOCKS_PER_SEC;
		size_t cur_id = datas_l[size_l - 1].id;
		// cout << "cur_id" << cur_id << endl;
		if (cur_id < clean) {
			offset_r = OFFSET_DATA + sizeof(data_t) * clean;
			for (int j = 0; j < dirty_round; j++) {
				int size_r = read_data(path, offset_r, DATA_BATCH, datas_r);
				// end = clock();
				// sum += (double)(end - start) / CLOCKS_PER_SEC;
				// cout << size_r << endl;
				for (int p = 0; p < size_l; p++)
					for (int q = 0; q < size_r; q++) 
						if (is_conflict(datas_l[p], datas_r[q])) {
							// datas_l[p].print();
							// datas_r[q].print();
							add_edge(datas_l[p].id, datas_r[q].id, edge_size++, rsu.next());
							if (edge_size % 10000000 == 0)
								cout << "processing tuple: " << datas_l[p].id
								<< ", the number of added edges: " << edge_size / 1000000 << "M." << endl;
						}

				offset_r += sizeof(data_t) * DATA_BATCH;
			}
		} else {
			for (int p = 0; p < size_l; p++)
				for (int q = p + 1; q < size_l; q++)
					if (is_conflict(datas_l[p], datas_l[q])) {
						// datas_l[p].print();
						// datas_l[q].print();
						add_edge(datas_l[p].id, datas_l[q].id, edge_size++, rsu.next());
						if (edge_size % 10000000 == 0)
							cout << "processing tuple: " << datas_l[p].id
							<< ", the number of added edges: " << edge_size / 1000000 << "M." << endl;
					}

			for (size_t j = i + 1; j < round; j++) {
				// start = clock();
				int size_r = read_data(path, offset_r, DATA_BATCH, datas_r);
				// end = clock();
				// sum += (double)(end - start) / CLOCKS_PER_SEC;
				for (int p = 0; p < size_l; p++)
					for (int q = 0; q < size_r; q++)
						if (is_conflict(datas_l[p], datas_r[q])) {
							// datas_l[p].print();
							// datas_r[q].print();
							add_edge(datas_l[p].id, datas_r[q].id, edge_size++, rsu.next());
							if (edge_size % 10000000 == 0)
								cout << "processing tuple: " << datas_l[p].id
								<< ", the number of added edges: " << edge_size / 1000000 << "M." << endl;
						}

				offset_r += sizeof(data_t) * DATA_BATCH;
			}
		}
		offset_l += sizeof(data_t) * DATA_BATCH;
		offset_r = offset_l + DATA_BATCH * sizeof(data_t);
	}

	/*data_t datas_l[DATA_BATCH], datas_r[DATA_BATCH];
	off_t offset_l = OFFSET_DATA, offset_r = OFFSET_DATA + sizeof(data_t) * DATA_BATCH;
	size_t round = ceil((double)number / DATA_BATCH);
	for (size_t i = 0; i < round; i++) {
		//	start = clock();
		int size_l = read_data(path, offset_l, DATA_BATCH, datas_l);
		// end = clock();
		// sum += (double)(end - start) / CLOCKS_PER_SEC;
		for (int p = 0; p < size_l; p++)
			for (int q = p + 1; q < size_l; q++)
				if (is_conflict(datas_l[p], datas_l[q])) {
					datas_l[p].print();
					datas_l[q].print();
					add_edge(datas_l[p].id, datas_l[q].id, edge_size++, rsu.next());
					if (edge_size % 10000000 == 0)
						cout << "processing tuple: " << datas_l[p].id
						<< ", the number of added edges: " << edge_size / 1000000 << "M." << endl;
				}

		for (size_t j = i + 1; j < round; j++) {
			// start = clock();
			int size_r = read_data(path, offset_r, DATA_BATCH, datas_r);
			// end = clock();
			// sum += (double)(end - start) / CLOCKS_PER_SEC;
			for (int p = 0; p < size_l; p++)
				for (int q = 0; q < size_r; q++)
					if (is_conflict(datas_l[p], datas_r[q])) {
						datas_l[p].print();
						datas_r[q].print();
						add_edge(datas_l[p].id, datas_r[q].id, edge_size++, rsu.next());
						if (edge_size % 10000000 == 0)
							cout << "processing tuple: " << datas_l[p].id
							<< ", the number of added edges: " << edge_size / 1000000 << "M." << endl;
					}

			offset_r += sizeof(data_t) * DATA_BATCH;
		}

		offset_l += sizeof(data_t) * DATA_BATCH;
		offset_r = offset_l + DATA_BATCH * sizeof(data_t);
	}*/

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
			if (iter->second > nei->lower_bound && iter->second <= nei->next_lower_bound)
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

int graph_t::vertexcover_online(const size_t& sample_threshold)
{
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

int graph_t::subgraph_vertexcover(const size_t& sample_threshold, const oracle& oracle) {
	size_t sample_number = 0;
	size_t vc_size = 0;
	while (sample_number < sample_threshold)
	{	
		size_t node_id = oracle.sample_node();
		// cout << "sample id" << node_id << endl;
		if (in_vertexcover(node_id, oracle))
			vc_size++;
		sample_number++;
	}
	clear();
	return vc_size * oracle.get_node_size() / sample_threshold;
}

int graph_t::subgraph_vertexcover_sp(const size_t& sample_threshold, const oracle& oracle)
{
	size_t sample_number = 0;
	size_t vc_size = 0;
	while (sample_number < sample_threshold)
	{
		size_t node_id = oracle.sample_node_sp();
		// cout << "sample id" << node_id << endl;
		if (in_vertexcover_sp(node_id, oracle))
			vc_size++;
		sample_number++;
	}
	clear();
	return vc_size * oracle.get_node_size() / sample_threshold;
}

int graph_t::subgraph_vertexcover_online(const size_t& sample_threshold, const oracle& oracle) {
	size_t sample_number = 0;
	size_t vc_size = 0;
	while (sample_number < sample_threshold)
	{
		size_t node_id = oracle.sample_node();
		// cout << node_id << endl;
		// node.show();
		if (in_vertexcover_online(node_id, oracle))
			vc_size++;
		sample_number++;
	}
	clear();
	return vc_size * oracle.get_node_size() / sample_threshold;
}

int graph_t::subgraph_vertexcover_online_sp(const size_t& sample_threshold, const oracle& oracle) {
	size_t sample_number = 0;
	size_t vc_size = 0;
	while (sample_number < sample_threshold)
	{
		size_t node_id = oracle.sample_node_sp();
		// cout << node_id << endl;
		// node.show();
		if (in_vertexcover_online_sp(node_id, oracle))
			vc_size++;
		sample_number++;
	}
	clear();
	return vc_size * oracle.get_node_size() / sample_threshold;
}
