#include "../Structures.h"
#include "omp.h"
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#ifndef COMMANDS_H_
#define COMMANDS_H_
float GetMeanSize(std::vector<std::vector<int>> nodeOccurences)
{
    float val = 0.0;
    for (auto vec : nodeOccurences)
    {
        val += vec.size();
    }
    return val / float(nodeOccurences.size());
}

float GetMetric(int nbOccurences, int maxOccurences, float meanSize, int maxSize, int volume, float alpha, float beta)
{
    float m = 0;
    m = alpha * (float(nbOccurences) / float(maxOccurences)) + beta * ((float(maxSize) - meanSize) / (float(maxSize) - float(volume)));

    return m;
}
#endif /* COMMANDS_H_ */
std::vector<std::vector<FSSEPatternOccurenceFiltered>> FilterCandidates(std::vector<std::vector<MergedFSSEPatternOccurence>> temporalPatterns, int support, int maxSize, int volume, float alpha = 0.5, float beta = 0.5)
{
    int maxOccurences = 0;
    std::vector<std::vector<FSSEPatternOccurenceFiltered>> filteredPattern;
    for (int time = 0; time < temporalPatterns.size(); time++)
    {
        std::vector<FSSEPatternOccurenceFiltered> currentPatterns;
        for (int patternNb = 0; patternNb < temporalPatterns[time].size(); patternNb++)
        {
            if (temporalPatterns[time][patternNb].nbOccurences >= 2)
            {
                FSSEPatternOccurenceFiltered tmpFsseOccFiltered;
                tmpFsseOccFiltered.patOccurence = temporalPatterns[time][patternNb];
                tmpFsseOccFiltered.respectSup = temporalPatterns[time][patternNb].nbOccurences >= support;
                tmpFsseOccFiltered.meanSize = GetMeanSize(temporalPatterns[time][patternNb].nodeOccurences);
                if (temporalPatterns[time][patternNb].nbOccurences > maxOccurences)
                {
                    maxOccurences = temporalPatterns[time][patternNb].nbOccurences;
                }

                // if(temporalPatterns[time][patternNb].nbOccurences >= support){
                //     std::cout << "test"<< std::endl;
                // }
                std::cout << "Occurences : " << temporalPatterns[time][patternNb].nbOccurences << std::endl;
                currentPatterns.push_back(tmpFsseOccFiltered);
            }
        }
        filteredPattern.push_back(currentPatterns);
    }

    for (int time = 0; time < filteredPattern.size(); time++)
    {
        for (int patternNb = 0; patternNb < filteredPattern[time].size(); patternNb++)
        {
            filteredPattern[time][patternNb].relevancyMetric = GetMetric(filteredPattern[time][patternNb].patOccurence.nbOccurences, maxOccurences, filteredPattern[time][patternNb].meanSize, maxSize, volume, alpha, beta);
        }
    }

    for (int time = 0; time < filteredPattern.size(); time++)
    {
        std::sort(filteredPattern[time].begin(), filteredPattern[time].end(),
                  [](const FSSEPatternOccurenceFiltered a, const FSSEPatternOccurenceFiltered b)
                  { return a.relevancyMetric > b.relevancyMetric; });
    }
    return filteredPattern;
}