#include <vector>
#include <boost/range/irange.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/config.hpp>

#ifndef STRUCTURES_H
#define STRUCTURES_H
#define MAX 16

struct Vertex
{
    std::vector<int> Attribute; // matrix of each value's attribute Attribute[0] = a1, Attribute[1] = a2, etc etc...
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, Vertex> Graph;

struct ProcessedGraph
{
    std::vector<std::vector<std::vector<std::vector<int>>>> occurences; // list of list of vertices
    // std::vector<std::vector<int>> combconnect;        // list of potentiel patterns
};
struct Result
{
    int count;                                                  // occurence number of a given pattern
    std::vector<std::vector<std::vector<int>>> connectedValues; // pattern
    std::vector<std::vector<int>> nodes;                        // list of list of vertices
    float relevancyMetric;
};

struct GraphReadingResults
{
    std::vector<std::vector<int>> attvalues;
    std::vector<Graph> gt;
};

struct TimeOccurences
{
    std::map<std::pair<int, int>, int> occurencesInTime; // map of std::pair<attribute, value>, occurence
};

struct FSSEPatternOccurence
{
    std::vector<std::vector<int>> connectedValues;
    std::vector<std::vector<int>> nodeOccurences;
    int nbOccurences;
};

struct MergedFSSEPatternOccurence
{
    std::vector<std::vector<int>> connectedValues;
    std::vector<std::vector<int>> nodeOccurences;
    int nbOccurences;
};

struct FSSEPatternOccurenceFiltered
{
    MergedFSSEPatternOccurence patOccurence;
    bool respectSup;
    float meanSize;
    float relevancyMetric;
};

struct ExtensionResults
{
    std::vector<int> count;                                        // Temporal patterns
    std::vector<std::vector<std::vector<std::vector<int>>>> nodes; // Occurences defined with vertices
    std::vector<std::vector<std::vector<int>>> sequence;           // count of pattern
};
struct GraphWithOccurence
{
    Graph g;           // Graph
    TimeOccurences to; // Occurences in time
};

struct FileResults
{
    GraphWithOccurence gwo;
    int attNb;
    std::vector<std::set<int>> attValueNb;
};

struct temporaryTimeComintatiionResult
{
    std::vector<int> previousFirstItem;
    std::vector<std::vector<int>> results;
};

#endif