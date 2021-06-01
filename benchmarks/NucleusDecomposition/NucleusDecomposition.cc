#include "NucleusDecomposition.h"
#include "benchmarks/KTruss/KTruss.h"
#include <math.h>
#include <fstream>

namespace gbbs {


long strToTableType(std::string tt_str, std::string inverse_index_map_str) {
  if (tt_str == "ONE_LEVEL") return 1;
  else if (tt_str == "TWO_LEVEL") {
    if (inverse_index_map_str == "BINARY_SEARCH") return 2;
    else if (inverse_index_map_str == "STORED_POINTERS") return 5;
    ABORT("Unexpected inverse index map (str): " << inverse_index_map_str);
  }
  else if (tt_str == "MULTI_LEVEL") {
    if (inverse_index_map_str == "BINARY_SEARCH") return 3;
    else if (inverse_index_map_str == "STORED_POINTERS") return 4;
    ABORT("Unexpected inverse index map (str): " << inverse_index_map_str);
  }
  ABORT("Unexpected number of levels (str): " << tt_str);
}

long strToUpdateAggregation(std::string update_agg_str) {
  if (update_agg_str == "SIMPLE_ARRAY") return 0;
  else if (update_agg_str == "LIST_BUFFER") return 1;
  else if (update_agg_str == "HASH_TABLE") return 2;
  ABORT("Unexpected update aggregation (str): " << update_agg_str);
}


template <class Graph>
double AppNucleusDecomposition_runner(Graph& GA, commandLine P) {
  auto tt_str = P.getOptionValue("--numberOfLevels", "");
  long num_multi_levels = P.getOptionLongValue("--numberOfMultiLevels", 2);
  auto inverse_index_map_str = P.getOptionValue("--inverseIndexMap", "");
  auto update_agg_str = P.getOptionValue("--updateAggregation", "");
  bool relabel = P.getOptionValue("--relabel"); // for true, relabel graph
  bool contiguous_space = P.getOptionValue("--contiguousSpace"); // for true, contiguous space
  long r = P.getOptionLongValue("--rClique", 3); // k as in k-cliques
  long ss = P.getOptionLongValue("--sClique", 4); // k as in k-cliques

  long table_type = 5;
  long num_levels = 2;
  if (tt_str == "") {
    table_type = P.getOptionLongValue("-tt", 3); // 1 = 1 lvl, 2 = 2 lvls, 3 = multi
    num_levels = P.getOptionLongValue("-nl", 2); // only for multi, # levels
  } else {
    table_type = strToTableType(tt_str, inverse_index_map_str);
    num_levels = num_multi_levels;
  }

  long efficient = 1;
  if (update_agg_str == "") {
    efficient = P.getOptionLongValue("-efficient", 1); // for list buffer
  } else {
    efficient = strToUpdateAggregation(update_agg_str);
  }

  // Internal options
  bool verify = P.getOptionValue("-verify"); // for testing
  bool use_compress = P.getOptionValue("-compress"); //only for 2, 3
  bool output_size = P.getOptionValue("-output_size");

  std::cout << "### Application: Nucleus Decomposition" << std::endl;
  std::cout << "### Graph: " << P.getArgument(0) << std::endl;
  std::cout << "### Threads: " << num_workers() << std::endl;
  std::cout << "### n: " << GA.n << std::endl;
  std::cout << "### m: " << GA.m << std::endl;
  std::cout << "### ------------------------------------" << std::endl;

  assert(P.getOption("-s"));

  timer t; t.start();

  if (r == 2 && ss == 3 && table_type == 5 && efficient == 2) {
    KTruss_ht(GA, 16);
  } else {
    NucleusDecomposition(GA, r, ss, table_type, num_levels, relabel, contiguous_space, verify, efficient, use_compress, output_size);
  }

  double tt = t.stop();

  std::cout << "### Running Time: " << tt << std::endl;

  // Require solely one round
  exit(0);

  return tt;
}
}
generate_symmetric_main(AppNucleusDecomposition_runner, false);
