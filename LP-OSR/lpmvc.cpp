#include "lpmvc.h"

/*
 * x_i + x_j >= 1.0 for all (i, j) in E
 * x_i in {0, 0.5, 1}
 */
int lp_mvc(graph_t& graph) {
	vector<int> x;
	int m = graph.get_edge_size();
	int n = graph.get_node_size();
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
		edge_t edge = graph.get_edge(i - 1);
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
	// cout << glp_get_obj_val(lp) << endl;
	for (int i = 1; i <= n; i++) {
		// cout << glp_get_col_prim(lp, i) << endl;
		if (glp_get_col_prim(lp, i) >= .5) {
			x.push_back(i);
			// cout << i - 1 << " ";
			// cout << graph.get_node(i - 1).edges.size() << endl;
			// graph.get_node(i - 1).show();
		}
	}
		
cleanup:
	glp_delete_prob(lp);

	return x.size();
}
