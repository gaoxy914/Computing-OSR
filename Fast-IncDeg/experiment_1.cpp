#include "graph.h"
#include "bplus_tree.h"
#include "oracle.h"
#include "util.h"
#include <ctime>
/* general experiment */
int main() {
	srand((unsigned int)time(NULL));

	size_t epsilon = 100;
	size_t round = 300; // 300 queries

	string data_path = "data.db";
	string bpt_id_path = "bpt_id.db";
	string bpt_q_path = "bpt_q.db";
	string log_path = "time.log";
	string result_path = "result1.csv";

	clock_t start, end;
	time_t now;
	double duration;

	ofstream outLog;
	outLog.open(log_path, ios::app | ios::out);

	ofstream outFile;
	outFile.open(result_path, ios::app | ios::out);

	Generator generator;
	generator.load_rawdata();

	for (int i = 1; i <= 10; i++) {
		size_t number = i * 1000000; // 1M -> 10M
		for (double rho = 0.01; rho <= 0.1; rho += 0.01) {
			outLog << "\nexperiment with " << number << " tuples\n";

			now = time(0);
			outLog << ctime(&now) << "start to generate data.\n";
			start = clock();
			generator.gen_data(data_path.c_str(), number, rho);
			end = clock();
			duration = (double)(end - start) / CLOCKS_PER_SEC;
			cout << "time consumption of data generation: " << duration << "s." << endl;
			now = time(0);
			outLog << ctime(&now) << "complete data generation.\n";
			outLog << "time consumption of data generation: " << duration << "s." << endl;

			now = time(0);
			outLog << ctime(&now) << "start to build B+tree.\n";
			start = clock();
			bplus_tree bpt_q(bpt_q_path.c_str(), true);
			bpt_q.build_q(data_path.c_str(), number);

			bplus_tree bpt_id(bpt_id_path.c_str(), true);
			bpt_id.build_id(data_path.c_str(), number);
			end = clock();
			duration = (double)(end - start) / CLOCKS_PER_SEC;
			cout << "time consumption of building B+tree: " << duration << "s." << endl;
			now = time(0);
			outLog << ctime(&now) << "complete B+tree construction.\n";
			outLog << "time consumption of building B+tree: " << duration << "s." << endl;

			now = time(0);
			outLog << ctime(&now) << "start to build graph.\n";
			start = clock();
			graph_t graph(data_path.c_str(), number);
			end = clock();
			duration = (double)(end - start) / CLOCKS_PER_SEC;
			cout << "time consumption of building graph: " << duration << "s." << endl;
			now = time(0);
			outLog << ctime(&now) << "complete graph construction.\n";
			outLog << "time consumption of building graph: " << duration << "s." << endl;
			outLog << "graph infomation: node size: " << graph.get_node_size()
				<< ", edge size: " << graph.get_edge_size()
				<< ", max degree: " << graph.get_max_degree()
				<< ", min degree: " << graph.get_min_degree() << endl;

			size_t sample_threshold = 4 * epsilon * epsilon;

			double sum = 0, sum_online = 0, min = 100000, max = 0, min_online = 100000, max_online = 0;
			double sum_sp = 0, sum_online_sp = 0, min_sp = 100000, max_sp = 0, min_online_sp = 100000, max_online_sp = 0;
			for (size_t i = 0; i < round; ++i) {
				// cout << i << endl;
				double left = (rand() % number / 4) / 10;
				double right = left + number / (10 * (rand() % 3 + 2)); // number / 10 - (rand() % number / 4) / 10;
				if (sizeof(data_t) == sizeof(dblp_t)) {
					left = (rand() % number / 4);
					right = left + number / (rand() % 3 + 2);
				}
				// cout << left << " "  << right << endl;
				oracle oracle(bpt_id_path.c_str(), bpt_q_path.c_str(), left, right);
				// cout << graph.vertexcover(sample_threshold) << endl;

				start = clock();
				graph.subgraph_vertexcover(sample_threshold, oracle);
				end = clock();
				duration = (double)(end - start) / CLOCKS_PER_SEC;
				// oracle.show();
				sum += duration;
				if (duration > max) max = duration;
				if (duration < min) min = duration;

				start = clock();
				graph.subgraph_vertexcover_sp(sample_threshold, oracle);
				end = clock();
				duration = (double)(end - start) / CLOCKS_PER_SEC;
				// oracle.show();
				sum_sp += duration;
				if (duration > max_sp) max_sp = duration;
				if (duration < min_sp) min_sp = duration;

				start = clock();
				graph.subgraph_vertexcover_online(sample_threshold, oracle);
				end = clock();
				duration = (double)(end - start) / CLOCKS_PER_SEC;
				// oracle.show();
				sum_online += duration;
				if (duration > max_online) max_online = duration;
				if (duration < min_online) min_online = duration;

				start = clock();
				graph.subgraph_vertexcover_online_sp(sample_threshold, oracle);
				end = clock();
				duration = (double)(end - start) / CLOCKS_PER_SEC;
				// oracle.show();
				sum_online_sp += duration;
				if (duration > max_online_sp) max_online_sp = duration;
				if (duration < min_online_sp) min_online_sp = duration;
			}
			outFile << number << "," << rho << ","
				<< sum / round << "," << sum_online / round << "," << sum_sp / round << "," << sum_online_sp / round << ","
				<< max << "," << max_online << "," << max_sp << "," << max_online_sp << ","
				<< min << "," << min_online << "," << min_sp << "," << min_online_sp << endl;
		}
		now = time(0);
		outLog << ctime(&now) << "complete experiments.\n";
	}

	outFile.close();

	outLog.close();


	return 0;
}