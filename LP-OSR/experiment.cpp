#include "graph.h"
#include "util.h"
#include <ctime>

int main() {
	srand((unsigned int)time(NULL));

	string data_path = "data.db";
	string result_path = "result.csv";

	size_t epsilon = 100;

	Generator generator;
	generator.load_rawdata();

	ofstream outCsv;
	outCsv.open(result_path, ios::app | ios::out);

	for (int i = 10; i <= 40; i += 5) {
		size_t number = i * 1000; // 10k -> 40k

		for (double rho = 0.01; rho <= 0.1; rho += 0.01) {
			generator.gen_data(data_path.c_str(), number, rho);

			graph_t graph(data_path.c_str(), number, rho);

			// outCsv << graph.get_node_size() << ","
			//	<< graph.get_edge_size() << ","
			//	<< graph.get_max_degree() << ","
			//	<< graph.get_min_degree() << endl;

			size_t vc_bllp = 0, vc_telp = 0, vc_qtlp = 0, vc = 0, vc_online = 0;

			vc_bllp = graph.vertexcover_bllp();
			
			graph.reset();

			vc_telp = graph.vertexcover_telp();

			graph.reset();

			vc_qtlp = graph.vertexcover_qtlp(2, 0.3);

			size_t sample_threshold = 4 * epsilon * epsilon;

			for (int i = 0; i < 10; i++) {
				vc += graph.vertexcover(sample_threshold);

				vc_online += graph.vertexcover_online(sample_threshold);
			}

			outCsv << rho << "," << number << ","
				<< vc_bllp << "," << vc_telp << ","  << vc_qtlp << "," 
				<< vc / 10 << "," << vc_online / 10 << endl;
		}
	}

	outCsv.close();
	return 0;
}