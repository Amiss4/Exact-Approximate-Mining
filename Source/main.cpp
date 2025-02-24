#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <regex>
#include <stdexcept>
#include <fstream>
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
#include "./lib/Structures.h"
#include "./lib/CandidateExtraction/GenericCandidateExtractor.h"
#include "./lib/FilteringCandidates/GenericFilteringCandidates.h"
#include "./lib/Extension/GenericExtension.h"
#include "./lib/Extension/FSSEExtension.h"

FileResults read_graph(std::string edges_filename, std::string att_filename)
{

    /// <summary>
    /// Create combination attributes of size k
    /// </summary>
    /// <param name="edges_filename"> set of attributes
    /// <param name="att_filename"> set of attribute values
    /// <param name="de"> combination size
    /// <returns> the graph with edges and attributes + occurences of attribute values
    FileResults fr;
    GraphWithOccurence gwo;
    std::string line_edges, line_att;
    int nb_line_att = 0;
    std::ifstream file_edges, file_attributes, file_label;
    file_attributes.open(att_filename);
    std::vector<std::vector<int>> temp_val_att;
    std::vector<int> size;
    if (file_attributes.is_open()) // Check if the file is open
    {
        std::cout << "File " + att_filename + " opened successfully" << std::endl;
    }
    std::vector<std::string> Att;
    while (file_attributes.good())                    // We start to read the file
    {                                                 // Remark: using the loop while(file.good()) will make a read until a line past the last non empty line
        getline(file_attributes, line_att);           // Read line by line.
        split(Att, line_att, boost::is_any_of("\t")); // Splits the line into a std::vector containing all strings of a text line based on their separation (in this case "\t")
        size.push_back(Att.size());
        if (Att.size() > 1) // Condition for reading non empty lines. This may need further modification if we have a single attribute per line
        {
            add_vertex(gwo.g); // Create a vertex
            // In the following loop we write the attributes
            std::vector<int> att_int;
            for (int i = 0; i < Att.size(); i++) // We use .size()-1 as the split function creates a std::vector of size n + 1
                                                 // (this can be set up according to the nature of the file we read)
            {
                std::pair<int, int> attValue(i, atoi(Att[i].c_str()));
                att_int.push_back(atoi(Att[i].c_str()));
                // fr
                int att;
                // Transform strings into integers
                std::stringstream(std::string(Att[i])) >> att;

                // Occurences getter
                gwo.g[nb_line_att].Attribute.push_back(att);
                if (gwo.to.occurencesInTime.find(attValue) == gwo.to.occurencesInTime.end())
                {
                    gwo.to.occurencesInTime.insert(std::pair<std::pair<int, int>, int>(attValue, 1)); //
                }

                else
                {
                    // This routine will increment the value if the key already exists. If it doesn't exist it will create it for you
                    gwo.to.occurencesInTime[attValue]++;
                }
            }
            temp_val_att.push_back(att_int);
            nb_line_att++; // nb_line_att refers to the index of a vertex in the graph
        }
    }
    file_attributes.close(); // Close file
    for (auto it = gwo.to.occurencesInTime.cbegin(); it != gwo.to.occurencesInTime.cend(); ++it)
    {
        std::cout << "Attribut : " << it->first.first << ", Value : " << it->first.second << ", Count : " << it->second << "\n";
    }
    file_edges.open(edges_filename);
    if (file_edges.is_open())
    {
        std::cout << "File " + edges_filename + " opened successfully" << std::endl;
    }
    std::regex regEdges("([0-9]+)[^0-9]*([0-9]+)");
    std::vector<std::string> Edges;
    while (file_edges.good())
    {
        getline(file_edges, line_edges); // Read line
        // split(Edges, line_edges, is_any_of("	"));
        std::smatch m;
        bool b = std::regex_search(line_edges, m, regEdges);
        if (b)
        { // Check if the line is not empty (otherwise the size would be 1)
            int ledge, redge;
            // Transform strings into integers
            std::stringstream(m[1]) >> ledge;
            std::stringstream(m[2]) >> redge;
            add_edge(ledge, redge, gwo.g); // Add edges
        }
    }
    file_edges.close();
    // FileResults fr;
    fr.gwo = gwo;
    fr.attNb = *max_element(std::begin(size), std::end(size));
    std::vector<std::set<int>> att_values;
    for (int i = 0; i < fr.attNb; i++)
    {
        std::set<int> tmp_set;
        for (auto v : temp_val_att)
        {
            tmp_set.insert(v[i]);
        }
        att_values.push_back(tmp_set);
    }
    fr.attValueNb = att_values;
    return fr;
}

GraphReadingResults getGraphData(std::string dataset, int volume, int nbTime, std::vector<int> selected_attribute)
{
    std::vector<Graph> gt;
    std::vector<TimeOccurences> mapOccurences;
    std::vector<std::vector<std::vector<std::vector<int>>>> result;
    std::vector<int> tmp_size;
    std::vector<std::vector<std::set<int>>> tmp_att_values;

    // Import DATA
    for (int i = 0; i < nbTime; i++)
    {
        FileResults fr;
        std::stringstream stmp2;
        std::stringstream stmp;
        stmp2 << dataset << "/attributes/T" << std::to_string(i) << "attributes";
        stmp << dataset << "/edges/T" << std::to_string(i) << "edges";
        fr = read_graph(stmp.str(), stmp2.str());
        tmp_size.push_back(fr.attNb);
        tmp_att_values.push_back(fr.attValueNb);
        gt.push_back(fr.gwo.g);
        mapOccurences.push_back(fr.gwo.to); // <(attribut, valeur),  nb>
    }
    int size = *max_element(std::begin(tmp_size), std::end(tmp_size));
    tmp_size = std::vector<int>();

    std::vector<std::set<int>> att_values;

    for (int i = 0; i < size; i++)
    {
        std::set<int> tmp;
        att_values.push_back(tmp);
    }

    for (auto g : tmp_att_values)
    {
        std::set<int> tmp_set;

        for (int i = 0; i < g.size(); i++)
        {
            for (auto it = g[i].begin(); it != g[i].end(); it++)
            {
                att_values[i].insert(*it);
            }
        }
    }

    GraphReadingResults grr;
    std::vector<std::vector<int>> filtered_att;
    for (auto i : selected_attribute)
    {
        std::vector<int> v(att_values[i].begin(), att_values[i].end());
        filtered_att.push_back(v);
    }
    grr.attvalues = filtered_att;
    grr.gt = gt;
    return grr;
}

void WriteMergeIntermediateResults(std::vector<FSSEPatternOccurenceFiltered> results, int time, std::string file_path){

    std::stringstream currentPath; 
    currentPath << file_path << time << "mergeAttributes.txt"; 
    std::ofstream outfile;                                            // declaration of file pointer named outfile
    outfile.open(currentPath.str());
    for(auto res : results){
        std::stringstream last_node_pattern;
        last_node_pattern << "<";
        for(int i = 0; i< res.patOccurence.connectedValues.size(); i++){
            last_node_pattern << "<";
            for(int j =0; j<res.patOccurence.connectedValues[i].size(); j++){
                last_node_pattern << res.patOccurence.connectedValues[i][j] << ";";    
            }
            last_node_pattern << ">, ";
        } 
        last_node_pattern << ">";
        // std::stringstream node_to_test;
        // for(int i = 0; i< res.patOccurence.nodeOccurences.size(); i++){
        //     node_to_test << res.patOccurence.nodeOccurences[i][res.patOccurence.nodeOccurences[i].size()-2] << ";";  
        // } 
        outfile << last_node_pattern.str() << "\t" << res.patOccurence.nodeOccurences.size() << std::endl;
    }
    outfile.close();
}

int main()
{
    auto t1 = std::chrono::high_resolution_clock::now();
    int volume = 10;
    int supp = 10;
    int mincom = 2;
    int mincoh = 2;
    float alpha, beta;
    alpha = 0.2 ;
    beta = 1.0 - alpha;

    int threshold = 100; 

    std::vector<int> selected_attribute = {
        0,
        1,
        2,
        3,
        4,
        // 5,
        // 6,
        // 7,
        // 8,
        // 9,
        // 10, 
        // 11,
        // 12,
        // 13,
        // 14,
        // 15,
        // 16,
        // 17,
        // 18,
        // 19,
        // 20
        };
    int nbTime = 7;
    int maxSize = (volume - mincoh) * selected_attribute.size() + mincoh;
    std::stringstream file_path;
    std::string dataset = "../data/TrendGraphs/DBLP";
    GraphReadingResults grr = getGraphData(dataset, volume, nbTime, selected_attribute);

    // one ATT settings
    auto FSSEresults = OneAttFSSEExtraction(grr, volume, selected_attribute, supp, threshold);
    auto FilteredFSSE = FilterFSSE(FSSEresults, supp, maxSize, volume, alpha, beta);
    //     auto t2 = std::chrono::high_resolution_clock::now();
    // auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    // std::cout << "Execution time size 1 pattern: " << (ms_int.count() / 1000) << "s\n";

    // file_path << "../output/one_att/test_merge/";
    // int count = 0;
    // for (auto vec : FilteredFSSE)
    // {
    //     for (auto el : vec)
    //     {
    //         count++;
    //         std::cout << "Mean Size : " << el.meanSize << std::endl;
    //         std::cout << "Relevancy Metric : " << el.relevancyMetric << std::endl
    //                   << std::endl;
    //     }
    // }

    // std::cout << "Number of Patterns : " << count << std::endl;
    // Multi ATT settings
    // auto FSSEresults = FSSEExtraction(grr, volume, selected_attribute);
    // auto FilteredFSSE = FilterFSSEMultiAtt(FSSEresults, supp);
    // file_path << "../output/classic/results_";

    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    // std::chrono::duration<double, std::milli> ms_double = t2 - t1;

    std::cout << "Execution time size 1 pattern: " << (ms_int.count() / 1000) << "s\n";

    // int countOkPatterns = 0;
    // double adaptSupp = 0;
    // for (int time = 0; time < FSSEresults.size(); time++)
    // {
    //     int countInTime = 0;
    //     for (auto pattern : FSSEresults[time])
    //     {
    //         if (pattern.nbOccurences >= supp)
    //         {
    //             adaptSupp += pattern.nbOccurences;
    //             countInTime++;
    //             countOkPatterns++;
    //         }
    //     }
    //     std::cout << "There is " << countInTime << " patterns respecting support constraint in time number " << time << "." << std::endl;
    // }

    // std::cout << "Total : " << countOkPatterns << " patterns respecting support constraint." << std::endl;
    // std::cout << "Mean supp " << (adaptSupp / countOkPatterns) << std::endl;
    GenericExtension(FilteredFSSE, mincom, maxSize, nbTime, file_path.str(), volume, alpha, beta, threshold);
    auto t3 = std::chrono::high_resolution_clock::now();
    auto ms_int_extension = std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2);
    auto ms_int_total = std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t1);

    // std::cout << "Execution time extension : " << (ms_int_extension.count() / 1000) << "s\n";
    std::cout << "Execution time : " << (ms_int_total.count() / 1000) << "s\n";
    return 0;
}
