#include "graph.h"
#include "util.h"
#include "lpmvc.h"
#include <ctime>

int main() {
	srand((unsigned int)time(NULL));

	string data_path = "data.db";
	string result_path = "result";

	vector<size_t> epsilons;
	epsilons.push_back(100);
	// epsilons.push_back(1000);

	Generator generator;
	generator.load_rawdata();

	ofstream outCsv;
	
	for (double rho = 0.06; rho <= 0.06; rho += 0.01) {
		result_path = "resultd.csv";
		outCsv.open(result_path, ios::app | ios::out);

		for (int i = 7; i <= 7; i++) {
			size_t number = i * 1000; // 10k -> 1000k
		
			generator.gen_data(data_path.c_str(), number, rho);

			graph_t graph(data_path.c_str(), number, rho);

			graph_t sum_graph(number);
			graph_t graph_1(data_path.c_str(), number, rho, is_conflict_fd1);
			graph_t graph_2(data_path.c_str(), number, rho, is_conflict_fd2);
			graph_t graph_3(data_path.c_str(), number, rho, is_conflict_fd3);
			// graph_t graph_4(data_path.c_str(), number, rho, is_conflict_fd4);
	
			forests_t forests(sum_graph);
			forests.add_graph(graph_1);
			forests.add_graph(graph_2);
			forests.add_graph(graph_3);
			// forests.add_graph(graph_4);

			outCsv << graph.get_node_size() << ","
				<< graph.get_edge_size() << ","
				<< graph.get_max_degree() << ","
				<< graph.get_min_degree() << endl;

			size_t vc_lp = 0, vc_lp2 = 0;

			vc_lp = lp_mvc(graph);

			vc_lp2 = forests.vertexcover();

			for (size_t epsilon : epsilons) {

				size_t vc = 0, vc_online = 0;

				size_t sample_threshold = 2 * epsilon * epsilon;

				for (int i = 0; i < 10; i++) {
					vc += graph.vertexcover(sample_threshold);

					vc_online += graph.vertexcover_online(sample_threshold);
				}

				outCsv << epsilon << "," << vc_lp << "," << vc_lp2 << "," << vc / 10 << "," << vc_online / 10 << endl;
			}
			

		}
		outCsv.close();
	}
	
	return 0;
}