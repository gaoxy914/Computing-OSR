#pragma once

#include <iostream>

extern "C" {
#include "glpk.h"
}

#include "graph.h"

int lp_mvc(graph_t& graph);