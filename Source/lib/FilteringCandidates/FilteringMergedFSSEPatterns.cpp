#include "../Structures.h"
#include "omp.h"
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

// float GetMeanSize(std::vector<std::vector<int>> nodeOccurences){
//     float val = 0.0; 
//     for(auto vec : nodeOccurences){
//         val += vec.size();
//     }
//     return val/float(nodeOccurences.size());
// }


std::vector<std::vector<FSSEPatternOccurenceFiltered>> FilterMergedCandidates(std::vector<std::vector<MergedFSSEPatternOccurence>> temporalPatterns, int support)
{
    std::vector<std::vector<FSSEPatternOccurenceFiltered>> filteredPattern;
    for (int time = 0; time < temporalPatterns.size(); time++)
    {
        std::vector<FSSEPatternOccurenceFiltered> currentPatterns;
        for (int patternNb = 0; patternNb < temporalPatterns[time].size(); patternNb++)
        {
            if (temporalPatterns[time][patternNb].nbOccurences >= support)
            {
                FSSEPatternOccurenceFiltered tmpFsseOccFiltered;
                tmpFsseOccFiltered.patOccurence = temporalPatterns[time][patternNb];
                tmpFsseOccFiltered.respectSup = temporalPatterns[time][patternNb].nbOccurences >= support;
                // tmpFsseOccFiltered.meanSize = GetMeanSize(temporalPatterns[time][patternNb].nodeOccurences);
                std::cout << "Occurences : " << temporalPatterns[time][patternNb].nbOccurences << std::endl;
                currentPatterns.push_back(tmpFsseOccFiltered);
            }
        }
        filteredPattern.push_back(currentPatterns);
    }
    return filteredPattern;
}