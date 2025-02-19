#include <sstream>
#include <iostream>
#include <string>
#include "./FSSE.h"
#include "./OneAttFSSE.h"
#include "../Structures.h"
#include "omp.h"
#define MAX 16

std::vector<std::vector<FSSEPatternOccurence>> FSSEExtraction(GraphReadingResults grr, int volume, std::vector<int> selected_attribute)
{

    std::vector<std::vector<std::vector<int>>> compconnect = CreateCandidateSpace(grr.attvalues, volume);
    std::vector<std::vector<FSSEPatternOccurence>> temporalOneSizedOccurence;
    for (int time = 0; time < grr.gt.size(); time++)
    {
        std::vector<FSSEPatternOccurence> oneSizedOccurence(compconnect.size());

#pragma omp parallel for schedule(dynamic) num_threads(MAX)
        for (int pattern_number = 0; pattern_number < compconnect.size(); pattern_number++)
        {
            std::cout << "Time : " << time + 1 << " / " << grr.gt.size() << " -- Pattern : " << pattern_number + 1 << " / " << compconnect.size() << std::endl;

            auto occurences = ExtractSize1Patterns(grr.gt[time], compconnect[pattern_number], selected_attribute);
#pragma omp critical
            oneSizedOccurence.push_back(occurences);
        }
        temporalOneSizedOccurence.push_back(oneSizedOccurence);
    }
    // std::cout << "Graphs OK" << std::endl;

    return temporalOneSizedOccurence;
}

template <typename t>
void move(std::vector<t> &v, size_t oldIndex, size_t newIndex)
{
    if (oldIndex > newIndex)
        std::rotate(v.rend() - oldIndex - 1, v.rend() - oldIndex, v.rend() - newIndex);
    else
        std::rotate(v.begin() + oldIndex, v.begin() + oldIndex + 1, v.begin() + newIndex + 1);
}

void ChangeConnValuesBack(std::vector<std::vector<int>> inputVect, int changeIndex)
{
    // std::vector<std::vector<int>> retValue;
    for (int i = 0; i < inputVect.size(); i++)
    {
        move(inputVect[i], 0, changeIndex);
    }
}

std::vector<std::vector<FSSEPatternOccurence>> FilterMostFrequentOccurences(std::vector<std::vector<FSSEPatternOccurence>> patterns, int minThreshold, float percentage)
{
    for (int i = 0; i < patterns.size(); i++)
    {
        std::sort(patterns[i].begin(), patterns[i].end(),
                  [](const FSSEPatternOccurence a, const FSSEPatternOccurence b)
                  { return a.nbOccurences > b.nbOccurences; });
        if (minThreshold < patterns[i].size())
        {
            if (minThreshold > percentage * patterns[i].size())
            {
                patterns[i] = std::vector<FSSEPatternOccurence>(patterns[i].begin(), patterns[i].begin() + minThreshold);
            }
            else
            {
                patterns[i] = std::vector<FSSEPatternOccurence>(patterns[i].begin(), patterns[i].begin() + (int)(percentage * patterns[i].size()));
            }
        }
    }

    return patterns;
}

std::vector<std::vector<MergedFSSEPatternOccurence>> OneAttFSSEExtraction(GraphReadingResults grr, int volume, std::vector<int> selected_attribute, int support, int threshold)
{
    std::vector<std::vector<std::vector<FSSEPatternOccurence>>> allTemporalOneSizedOccurence;
    std::vector<std::vector<std::vector<int>>> globalComptConnect;
    for (int att = 0; att < selected_attribute.size(); att++)
    {
        std::vector<std::vector<int>> compconnect = OneAttCreateCandidateSpace(grr.attvalues[att], volume);
        globalComptConnect.push_back(compconnect);
        std::vector<std::vector<FSSEPatternOccurence>> temporalOneSizedOccurence;
        for (int time = 0; time < grr.gt.size(); time++)
        {
            std::vector<FSSEPatternOccurence> oneSizedOccurence;

#pragma omp parallel for schedule(dynamic) num_threads(MAX)
            for (int pattern_number = 0; pattern_number < compconnect.size(); pattern_number++)
            {
                std::cout << "Time : " << time + 1 << " / " << grr.gt.size() << " -- Pattern : " << pattern_number + 1 << " / " << compconnect.size() << std::endl;

                auto occurences = OneAttExtractSize1Patterns(grr.gt[time], compconnect[pattern_number], selected_attribute[att]);
                if (occurences.nbOccurences >= 2)
                {
#pragma omp critical
                    oneSizedOccurence.push_back(occurences);
                }
            }
            temporalOneSizedOccurence.push_back(oneSizedOccurence);
        }
        temporalOneSizedOccurence = FilterMostFrequentOccurences(temporalOneSizedOccurence, 10, 0.1);
        allTemporalOneSizedOccurence.push_back(temporalOneSizedOccurence);
    }
    int count = 0;
    for (auto a1 : allTemporalOneSizedOccurence)
    {
        for (auto a2 : a1)
        {
            for (auto el : a2)
            {
                std::cout << "Nb El : " << el.nbOccurences << std::endl;
            }
        }
    }
    std::cout << "Graphs OK" << std::endl;
    // int changeIndex = 3;
    // for (auto it = allTemporalOneSizedOccurence.begin() + changeIndex; it != allTemporalOneSizedOccurence.end(); ++it)
    // {
    //     auto x = *it; // or std::move(*it)
    //     allTemporalOneSizedOccurence.erase(it);
    //     allTemporalOneSizedOccurence.insert(allTemporalOneSizedOccurence.begin(), x /* or std::move(x) */);
    //     break;
    // }
    auto results = MergePatterns(allTemporalOneSizedOccurence, globalComptConnect, volume, threshold);
    // for (auto vect : results)
    // {
    //     for (auto occ : vect)
    //     {
    //         ChangeConnValuesBack(occ.occurences.connectedValues, changeIndex);
    //     }
    // }
    return results;
}