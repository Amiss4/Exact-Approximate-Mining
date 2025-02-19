#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
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
#define MAX 16

bool OneAttTestNodeAttribute(std::vector<int> attributes, int nodePattern, int selectedAtt)
{
    // for (int node_index = 0; node_index < nodePattern.size(); node_index++)
    // {
    if (attributes[selectedAtt] != nodePattern)
    {
        return false;
    }
    // }
    return true;
}

int OneAttNeighboursFiltering(int i, Graph g, int nodePattern, int selectedAtt)
{
    auto neighbours = adjacent_vertices(i, g);
    for (size_t vd : boost::make_iterator_range(neighbours)) // explore all neighbours of vertices
    {
        if (OneAttTestNodeAttribute(g[vd].Attribute, nodePattern, selectedAtt))
        {
            return int(vd);
        }
    }
    return -1;
}

FSSEPatternOccurence OneAttExtractSize1Patterns(Graph g, std::vector<int> pattern, int selectedAtt)
{

    FSSEPatternOccurence FPO;
    boost::graph_traits<Graph>::vertex_iterator i, end; // Create iteration on vertices
    std::vector<std::vector<int>> occ;                  // Final std::vector
    std::vector<int> vertexvis;
    for (std::tie(i, end) = vertices(g); i != end; ++i) // Explore all vertices of graph
    {
        std::vector<int> comb;
        bool firstNodeIsOk = OneAttTestNodeAttribute(g[*i].Attribute, pattern[0], selectedAtt);
        if (firstNodeIsOk)
        {

            int currentNodeIndex = int(*i);
            comb.push_back(currentNodeIndex);

            for (int v = 1; v < pattern.size(); v++)
            {
                auto neighbours = adjacent_vertices(currentNodeIndex, g);
                for (size_t vd : boost::make_iterator_range(neighbours)) // explore all neighbours of vertices
                {
                    int nextVertex = int(vd);
                    // if (nextVertex != -1 && std::find(comb.begin(), comb.end(), nextVertex) == comb.end() && OneAttTestNodeAttribute(g[vd].Attribute, pattern[v], selectedAtt))
                    if (nextVertex != -1 && nextVertex > currentNodeIndex && OneAttTestNodeAttribute(g[vd].Attribute, pattern[v], selectedAtt))
                    {
                        currentNodeIndex = nextVertex;
                        comb.push_back(currentNodeIndex);
                        break;
                    }
                }
            }
            if (comb.size() == pattern.size())
            {
                occ.push_back(comb);
            }
        }
    }
    FPO.nbOccurences = occ.size();
    FPO.nodeOccurences = occ;
    FPO.connectedValues.push_back(pattern);
    return FPO;
}

std::vector<std::vector<int>> OneAttCartesianProductDoubleSimple(std::vector<std::vector<int>> A, std::vector<int> B)
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
std::vector<std::vector<int>> OneAttCreateCandidateSpace(std::vector<int> liAtt, int vol)
{
    std::vector<std::vector<int>> combatt;
    if (combatt.size() == 0)
    {
        for (auto i : liAtt)
            combatt.push_back({i});
    }
    for (int i = 1; i < vol; i++)
    {
        combatt = OneAttCartesianProductDoubleSimple(combatt, liAtt);
    }

    // std::cout << "Possible patterns : " << combatt.size() << std::endl;

    return combatt;
}

bool OccurenceEquality(std::vector<int> p1, std::vector<int> p2, int volume)
{
    int count = 0;
    for (auto el2 : p2)
    {
        for (auto el1 : p1)
        {
            if (el1 == el2)
            {
                count++;
                if (count > (volume - 1))
                    return true;
                break;
            }
        }
    }

    return count >= (volume - 1);
}

std::vector<std::vector<int>> PatternsEquality(MergedFSSEPatternOccurence p1, FSSEPatternOccurence p2, int volume)
{
    std::vector<std::vector<int>> commonOccurences;
    for (int i = 0; i < p1.nodeOccurences.size(); i++)
    {
        for (int j = 0; j < p2.nodeOccurences.size(); j++)
        {
            if (OccurenceEquality(p1.nodeOccurences[i], p2.nodeOccurences[j], volume))
            {
                std::vector<int> tmpOccurences;
                tmpOccurences = p1.nodeOccurences[i];

                for (auto el : p2.nodeOccurences[j])
                {
                    if (std::count(tmpOccurences.begin(), tmpOccurences.end(), el) == 0)
                    {
                        tmpOccurences.push_back(el);
                    }
                }
                std::sort(tmpOccurences.begin(), tmpOccurences.end(),
                          [](const int a, const int b)
                          { return a < b; });

                commonOccurences.push_back(tmpOccurences);
                break;
            }
        }
    }
    return commonOccurences;
}

std::vector<std::vector<int>> PatternsEquality(FSSEPatternOccurence p1, FSSEPatternOccurence p2, int volume)
{
    std::vector<std::vector<int>> commonOccurences;
    for (int i = 0; i < p1.nodeOccurences.size(); i++)
    {
        for (int j = 0; j < p2.nodeOccurences.size(); j++)
        {
            if (OccurenceEquality(p1.nodeOccurences[i], p2.nodeOccurences[j], volume))
            {
                std::vector<int> tmpOccurences;
                tmpOccurences = p1.nodeOccurences[i];

                for (auto el : p2.nodeOccurences[j])
                {
                    if (std::count(tmpOccurences.begin(), tmpOccurences.end(), el) == 0)
                    {
                        tmpOccurences.push_back(el);
                    }
                }

                std::sort(tmpOccurences.begin(), tmpOccurences.end(),
                          [](const int a, const int b)
                          { return a < b; });

                commonOccurences.push_back(tmpOccurences);
                break;
            }
        }
    }
    return commonOccurences;
}

std::vector<std::vector<int>> MergeCompConnect(std::vector<int> comp1, std::vector<int> comp2)
{
    std::vector<std::vector<int>> mergedComp;
    for (int i = 0; i < comp1.size(); i++)
    {
        mergedComp.push_back({comp1[i], comp2[i]});
    }
    return mergedComp;
}

std::vector<std::vector<int>> MergeCompConnect(std::vector<std::vector<int>> comp1, std::vector<int> comp2)
{
    std::vector<std::vector<int>> mergedComp;
    for (int i = 0; i < comp1.size(); i++)
    {
        mergedComp.push_back(comp1[i]);
    }
    mergedComp.push_back(comp2);
    return mergedComp;
}

std::vector<std::vector<MergedFSSEPatternOccurence>> MergeComplexPatterns(std::vector<std::vector<MergedFSSEPatternOccurence>> firstList, std::vector<std::vector<FSSEPatternOccurence>> secondList, std::vector<std::vector<int>> secondComp, int volume, int threshold)
{
    std::vector<std::vector<MergedFSSEPatternOccurence>> mergedPatterns;
    for (int graph = 0; graph < firstList.size(); graph++)
    {
        std::vector<MergedFSSEPatternOccurence> currentGraph;
#pragma omp parallel for ordered schedule(dynamic) num_threads(MAX)

        for (int p1 = 0; p1 < firstList[graph].size(); p1++)
        {
            for (int p2 = 0; p2 < secondList[graph].size(); p2++)
            {
                MergedFSSEPatternOccurence currentPattern;
                currentPattern.nodeOccurences = PatternsEquality(firstList[graph][p1], secondList[graph][p2], volume);
                currentPattern.nbOccurences = currentPattern.nodeOccurences.size();

                if (currentPattern.nbOccurences >= 2)
                {
#pragma omp critical
                    {
                        currentPattern.connectedValues = MergeCompConnect(firstList[graph][p1].connectedValues, secondComp[p2]);
                        currentGraph.push_back(currentPattern);
                    }
                }
            }
        }
        std::sort(currentGraph.begin(), currentGraph.end(),
                  [](const MergedFSSEPatternOccurence a, const MergedFSSEPatternOccurence b)
                  { return a.nbOccurences > b.nbOccurences; });
        currentGraph = currentGraph.size() > threshold ? std::vector<MergedFSSEPatternOccurence>(currentGraph.begin(), currentGraph.begin() + threshold) : currentGraph ;
        mergedPatterns.push_back(currentGraph);
    }
    return mergedPatterns;
}

std::vector<std::vector<MergedFSSEPatternOccurence>> MergeSimplePatterns(std::vector<std::vector<FSSEPatternOccurence>> firstList, std::vector<std::vector<FSSEPatternOccurence>> secondList, std::vector<std::vector<int>> firstComp, std::vector<std::vector<int>> secondComp, int volume)
{
    std::vector<std::vector<MergedFSSEPatternOccurence>> mergedPatterns;
    for (int graph = 0; graph < firstList.size(); graph++)
    {
        std::vector<MergedFSSEPatternOccurence> currentGraph;
#pragma omp parallel for ordered schedule(dynamic) num_threads(MAX)

        for (int p1 = 0; p1 < firstList[graph].size(); p1++)
        {
            for (int p2 = 0; p2 < secondList[graph].size(); p2++)
            {
                MergedFSSEPatternOccurence currentPattern;
                currentPattern.nodeOccurences = PatternsEquality(firstList[graph][p1], secondList[graph][p2], volume);
                currentPattern.nbOccurences = currentPattern.nodeOccurences.size();

                if (currentPattern.nbOccurences > 2)
                {
#pragma omp critical
                    {

                        // std::cout << "p1 : " <<firstList[graph]p1 << " p2 : " << p2 << std::endl;

                        currentPattern.connectedValues.push_back({firstList[graph][p1].connectedValues[0]});
                        currentPattern.connectedValues.push_back({secondList[graph][p2].connectedValues[0]});
                        currentGraph.push_back(currentPattern);
                    }
                }
            }
        }
        mergedPatterns.push_back(currentGraph);
    }
    return mergedPatterns;
}

std::vector<std::vector<MergedFSSEPatternOccurence>> SizeOneMerge(std::vector<std::vector<std::vector<FSSEPatternOccurence>>> p, std::vector<std::vector<std::vector<int>>> globalCompConnect)
{
    std::vector<std::vector<MergedFSSEPatternOccurence>> mergedPatterns;
    for (int g = 0; g < p[0].size(); g++)
    {
        std::vector<MergedFSSEPatternOccurence> currentPatterns;
        for (int pattern = 0; pattern < p[0][g].size(); pattern++)
        {
            if (p[0][g][pattern].nbOccurences >= 2)
            {
                MergedFSSEPatternOccurence currentPattern;
                std::vector<std::vector<int>> currentConnectedValues;
                for (auto el : globalCompConnect[0][pattern])
                    currentConnectedValues.push_back({el});
                currentPattern.connectedValues = currentConnectedValues;
                currentPattern.nbOccurences = p[0][g][pattern].nbOccurences;
                currentPattern.nodeOccurences = p[0][g][pattern].nodeOccurences;
            }
        }
        mergedPatterns.push_back(currentPatterns);
    }
    return mergedPatterns;
}

std::vector<std::vector<MergedFSSEPatternOccurence>> MergePatterns(std::vector<std::vector<std::vector<FSSEPatternOccurence>>> p, std::vector<std::vector<std::vector<int>>> globalCompConnect, int volume, int threshold)
{
    std::vector<std::vector<MergedFSSEPatternOccurence>> mergedPatterns;
    if (p.size() == 1)
    {
        return SizeOneMerge(p, globalCompConnect);
    }
    mergedPatterns = MergeSimplePatterns(p[0], p[1], globalCompConnect[0], globalCompConnect[1], volume);

    if (p.size() == 2)
        return mergedPatterns;

    for (int i = 2; i < globalCompConnect.size(); i++)
    {
        std::cout << i << std::endl;
        mergedPatterns = MergeComplexPatterns(mergedPatterns, p[i], globalCompConnect[i], volume, threshold);
    }

    return mergedPatterns;
}