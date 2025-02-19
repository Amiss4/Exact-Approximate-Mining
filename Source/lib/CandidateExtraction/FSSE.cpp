#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <boost/range/irange.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/config.hpp>
#include "omp.h"
#include "../Structures.h"

bool TestNodeAttribute(std::vector<int> attributes, std::vector<int> nodePattern, std::vector<int> selectedAtt)
{
    for (int node_index = 0; node_index < nodePattern.size(); node_index++)
    {
        if (attributes[selectedAtt[node_index]] != nodePattern[node_index])
        {
            return false;
        }
    }
    return true;
}

int NeighboursFiltering(int i, Graph g, std::vector<int> nodePattern, std::vector<int> selectedAtt)
{
    auto neighbours = adjacent_vertices(i, g); 
    for (size_t vd : boost::make_iterator_range(neighbours)) // explore all neighbours of vertices
    {
        if (TestNodeAttribute(g[vd].Attribute, nodePattern, selectedAtt))
        {
            return int(vd);
        }
    }
    return -1;
}

FSSEPatternOccurence ExtractSize1Patterns(Graph g, std::vector<std::vector<int>> pattern, std::vector<int> selectedAtt)
{

    FSSEPatternOccurence FPO;
    boost::graph_traits<Graph>::vertex_iterator i, end; // Create iteration on vertices
    std::vector<std::vector<int>> occ;                  // Final std::vector
    std::vector<int> vertexvis;
    for (std::tie(i, end) = vertices(g); i != end; ++i) // Explore all vertices of graph
    {
        std::vector<int> comb;
        bool firstNodeIsOk = TestNodeAttribute(g[*i].Attribute, pattern[0], selectedAtt);
        bool patternExists = true;
        if (firstNodeIsOk)
        {

            int currentNodeIndex = int(*i);
            comb.push_back(currentNodeIndex);

            for (int v = 1; v < pattern.size(); v++)
            {

                int nextVertex = NeighboursFiltering(currentNodeIndex, g, pattern[v], selectedAtt);
                if (nextVertex != -1 && nextVertex > currentNodeIndex)
                {
                    currentNodeIndex = nextVertex;
                    comb.push_back(currentNodeIndex);
                }
                else
                {
                    patternExists = false;
                    break;
                }
            }
            if (patternExists)
            {
                occ.push_back(comb);
            }
        }
    }
    FPO.nbOccurences = occ.size();
    FPO.nodeOccurences = occ;
    FPO.connectedValues = pattern;
    return FPO;
}

std::vector<std::vector<std::vector<int>>> CartesianProductTripleDouble(std::vector<std::vector<std::vector<int>>> A, std::vector<std::vector<int>> B)
{
    std::vector<std::vector<std::vector<int>>> output;
    for (auto a : A)
    {
        for (auto b : B)
        {
            std::vector<std::vector<int>> v;
            v.insert(v.end(), a.begin(), a.end());
            v.push_back(b);
            output.push_back(v);
        }
    }
    return output;
}

std::vector<std::vector<int>> CartesianProductDoubleSimple(std::vector<std::vector<int>> A, std::vector<int> B)
{
    std::vector<std::vector<int>> output;
    for (auto a : A)
    {
        for (auto b : B)
        {
            std::vector<int> v;
            v.insert(v.end(), a.begin(), a.end());
            v.push_back(b);
            output.push_back(v);
        }
    }
    return output;
}

std::vector<std::vector<std::vector<int>>> CreateCandidateSpace(std::vector<std::vector<int>> liAtt, int vol)
{
    std::vector<std::vector<int>> combatt;
    if (combatt.size() == 0)
    {
        for (auto i : liAtt[0])
            combatt.push_back({i});
    }
    for (int i = 1; i < liAtt.size(); i++)
    {
        combatt = CartesianProductDoubleSimple(combatt, liAtt[i]);
    }

    std::vector<std::vector<std::vector<int>>> allCombs;
    if (allCombs.size() == 0)
    {
        for (auto i : combatt)
            allCombs.push_back({i});
    }

    for (int i = 1; i < vol; i++)
    {
        allCombs = CartesianProductTripleDouble(allCombs, combatt);
    }
    //std::cout << "Possible patterns : " << allCombs.size() << std::endl;

    return allCombs;
}
