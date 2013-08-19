graph_bsp_simulator
===================

Simulator of Graph BSP

Use 'make' to build the executable.

Vertices in a graph with N vertices are numbered from 0 to N-1. An input file
is a text file with N lines. Each line has one or more integers. The first
integer, k, in line i is the number of neighbors of vertex i. The rest of the
line has k integers, the indexes of the neighboring vertices of vertex i. Graphs
are directed.

A partitioning has P partitions, numbered from 0 to P-1. A partitioning input
file is a text file with N lines. Each line has two or more integers. The first
integer, k, in line i is the number of partitions that vertex i belongs to. The
rest of the line hs k integers, the indexes of the partitions that vertex i
belongs to.

A query source file is a text file with an integer per line. This integer is
between 0 and N-1, both included. The integer in the n-th line is the index of
the vertex where query number n will start.

The output file has one line of text for each step of each query executed. Each
line contains the following information for each partition of the graph:
  The amount of load for computers in this partition with cached data.
  The amount of load for computers in this partition with uncached data.
  Amount of network communication sent from this partition.
  Amount of network communication received from this partition.
  Amount of network communication used in this partition for synchronzation.
