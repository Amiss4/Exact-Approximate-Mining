# Exact-Approximate Mining

Article submitted to [ACM Transactions on Knowledge Discovery from Data](https://dl.acm.org/journal/tkdd).

## Abstract

Graph  pattern mining is a challenging research domain due to the NP-Completeness of subgraph matching and the combinatorial explosion of the search spaces. When mining dynamic attributed graph, the data structure itself, namely evolution in attribute and topology, adds supplementary complexity to the mining process. Distributed and approximation solutions have been proposed to improve the efficiency of graph mining process. However, there is a lack of efficient solutions for mining dynamic attributed graphs. To answer this issue, we propose a new strategy: Exact-Approximate Dynamic Attributed Graph Pattern Mining Strategy. By this strategy, we aim to reduce the algorithm complexity itself and intermediate and final result search space while delivering exact results. We also propose a new way to use constraints in order to both reduce the calculation cost and to facilitate constraints' setting by users. This new strategy demonstrates its capacity to mine complex patterns such as frequent sequential subgraph evolutions in large dynamic attributed graphs. Indeed, It also reduces the computational impact of two graph parameters, number of attributes and number of timestamps, from exponential to linear. 


## Technical environnement

To execute this code, you need : 

+ A c++ envrionnement using g++
+ Openmp installed 

To compile the code, run the following command : 

```bash
/usr/bin/g++ -fdiagnostics-color=always -g $(find /your/path/to/Source -type f -iregex .*cpp) -fopenmp -o your/path/to/Source/main
```

Then execute *Source.out*.
