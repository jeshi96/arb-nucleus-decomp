# Shared-Memory Parallel Nucleus Decomposition

Organization
--------

This repository contains code for our shared-memory parallel (r, s)
nucleus decomposition algorithms. Note
that the repository uses the
[Graph-Based Benchmark Suite (GBBS)](https://github.com/ParAlg/gbbs)
for parallel primitives and benchmarks.

The `benchmarks/NucleusDecomposition/` directory contains the main executable, 
`NucleusDecomposition_main`.


Compilation
--------

Compiler:
* g++ &gt;= 7.4.0 with pthread support for the [ParlayLib](https://github.com/cmuparlay/parlaylib) scheduler
* g++ &gt;= 7.4.0 with support for Cilk Plus


Build system:
* [Bazel](https:://bazel.build) 2.0.0
* Make --- our primary build system is Bazel, but we also provide Makefiles
  for those who wish to run benchmarks without installing Bazel.

To build, navigate to the `benchmarks/NucleusDecomposition/` directory, and run:
```sh
# For Bazel:
$ bazel build :NucleusDecomposition_main

# For Make:
$ make
```

Most optionality from [GBBS](https://github.com/ParAlg/gbbs)
applies. In particular, to compile benchmarks for graphs with
more than 2^32 edges, the `LONG` command-line parameter should be set.

The default compilation is the ParlayLib scheduler (Homegrown).
To compile with the Cilk Plus scheduler instead of the Homegrown scheduler, use
the Bazel configuration `--config=cilk`. To compile using OpenMP instead, use
the Bazel configuration `--config=openmp`. To compile serially instead, use the
Bazel configuration `--config=serial`. (For the Makefiles, instead set the
environment variables `CILK`, `OPENMP`, or `SERIAL` respectively.)

Note that the default compilation mode in bazel is to build optimized binaries
(stripped of debug symbols). You can compile debug binaries by supplying `-c
dbg` to the bazel build command.

The following commands cleans the directory:
```sh
# For Bazel:
$ bazel clean  # removes all executables

# For Make:
$ make clean  # removes executables for the current directory
```

Graph Format
-------

The applications take as input the adjacency graph format used by
[GBBS](https://github.com/ParAlg/gbbs).

The adjacency graph format starts with a sequence of offsets one for each
vertex, followed by a sequence of directed edges ordered by their source vertex.
The offset for a vertex i refers to the location of the start of a contiguous
block of out edges for vertex i in the sequence of edges. The block continues
until the offset of the next vertex, or the end if i is the last vertex. All
vertices and offsets are 0 based and represented in decimal. The specific format
is as follows:

```
AdjacencyGraph
<n>
<m>
<o0>
<o1>
...
<o(n-1)>
<e0>
<e1>
...
<e(m-1)>
```

This file is represented as plain text.

**Using SNAP Graphs**

Graphs from the [SNAP dataset
collection](https://snap.stanford.edu/data/index.html) are used in our experiments. 
We use a tool from the [GBBS](https://github.com/ParAlg/gbbs) 
that converts the most common SNAP
graph format to the adjacency graph format that GBBS accepts. Usage example:
```sh
# Download a graph from the SNAP collection.
wget https://snap.stanford.edu/data/wiki-Vote.txt.gz
gzip --decompress ${PWD}/wiki-Vote.txt.gz
# Run the SNAP-to-adjacency-graph converter.
# Run with Bazel:
bazel run //utils:snap_converter -- -s -i ${PWD}/wiki-Vote.txt -o <output file>
# Or run with Make:
cd utils
make snap_converter
./snap_converter -s -i <input file> -o <output file>
```


Running Code
-------
The applications take the input graph as input, as well as flags to specify
the parameters of the (r, s) nucleus decomposition algorithm and desired 
optimizations. Note that the "-s" flag must be set to indicate a symmetric 
(undirected) graph, and the "-rounds 1" argument must be passed in.

The options for arguments are:
* `--r_clique` followed by an integer specifying r
* `--s_clique` followed by an integer specifying s
* `--numberOfLevels` followed by a string, which must be `ONE_LEVEL`, `TWO_LEVEL`, or `MULTI_LEVEL`
* `--numberOfMultiLevels` followed by an integer which must be less than or equal to r, specifying 
the number of levels desired if the `MULTI_LEVEL` option is chosen (this argument is ignored 
if `ONE_LEVEL` or `TWO_LEVEL` are chosen)
* `--inverseIndexMap` followed by a string, which must be `BINARY_SEARCH` or `STORED_POINTERS`
* `--contiguousSpace`, which specifies that contiguous space should be used for the 
parallel hash table T if set (this argument is ignored if `ONE_LEVEL` or `STORED_POINTERS`
are chosen, both of which require contiguous space)
* `--relabel`, which specifies that the graph should be relabeled if set
* `--updateAggregation` followed by a string, which must be `SIMPLE_ARRAY`, 
`LIST_BUFFER`, or `HASH_TABLE`

**Example Usage**

The main executable is `NucleusDecomposition_main` in the `benchmarks/NucleusDecomposition/` directory.

After navigating to the `benchmarks/NucleusDecomposition/` directory, a template command is:

```sh
# For Bazel:
$ bazel run :NucleusDecomposition_main -- -s -rounds 1 --r_clique 3 --s_clique 4 --numberOfLevels TWO_LEVEL --inverseIndexMap STORED_POINTERS --relabel --updateAggregation LIST_BUFFER </path/to/input/graph>

# For Make:
$ ./NucleusDecomposition -s -rounds 1 --r_clique 3 --s_clique 4 --numberOfLevels TWO_LEVEL --inverseIndexMap STORED_POINTERS --relabel --updateAggregation LIST_BUFFER </path/to/input/graph>
```
