#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include "../Structures.h"

int ElementInCommon(std::vector<int> vec1, std::vector<int> vec2)
{
    int count = 0;
    for (auto el1 : vec1)
    {
        for (auto el2 : vec2)
        {
            if (el1 == el2)
            {
                count++;
                break;
            }
        }
    }
    return count;
}

std::vector<std::vector<int>> NodeOccurencesIntersection(std::vector<std::vector<int>> nodes1, std::vector<std::vector<int>> nodes2, int mincom)
{
    std::vector<std::vector<int>> intersection;
    for (auto vec1 : nodes1)
    {
        for (auto vec2 : nodes2)
        {
            if (ElementInCommon(vec1, vec2) >= mincom)
            {
                intersection.push_back(vec2);
                break;
            }
        }
    }
    return intersection;
}

Result FSSEPatternOccurenceFilteredComparison(FSSEPatternOccurenceFiltered pattern1, FSSEPatternOccurenceFiltered pattern2, int mincom)
{
    Result r;
    r.connectedValues.push_back(pattern1.patOccurence.connectedValues);
    r.connectedValues.push_back(pattern2.patOccurence.connectedValues);
    r.nodes = NodeOccurencesIntersection(pattern1.patOccurence.nodeOccurences, pattern2.patOccurence.nodeOccurences, mincom);
    r.count = r.nodes.size();
    return r;
}

Result ResToFSSEPatternOccurenceFilteredComparison(Result pattern1, FSSEPatternOccurenceFiltered pattern2, int mincom)
{
    Result r;
    for (auto vec : pattern1.connectedValues)
        r.connectedValues.push_back(vec);
    r.connectedValues.push_back(pattern2.patOccurence.connectedValues);
    r.nodes = NodeOccurencesIntersection(pattern1.nodes, pattern2.patOccurence.nodeOccurences, mincom);
    r.count = r.nodes.size();
    return r;
}

float GetMetricExt(int nbOccurences, int maxOccurences, float meanSize, int maxSize, int volume, float alpha, float beta)
{
    float m = 0;
    m = alpha * (float(nbOccurences) / float(maxOccurences)) + beta * ((float(maxSize) - meanSize) / (float(maxSize) - float(volume)));

    return m;
}

float GetMeanSizeExt(std::vector<std::vector<int>> nodeOccurences)
{
    float val = 0.0;
    for (auto vec : nodeOccurences)
    {
        val += vec.size();
    }
    return val / float(nodeOccurences.size());
}

std::vector<Result> ExtensionPattPatt(std::vector<FSSEPatternOccurenceFiltered> occ1, std::vector<FSSEPatternOccurenceFiltered> occ2, int mincom, int maxSize, int volume, float alpha, float beta)
{
    std::vector<Result> results;
    int maxOcc = 0;
#pragma omp parallel for ordered schedule(dynamic) num_threads(MAX)
    for (auto pattern1 : occ1)
    {
        for (auto pattern2 : occ2)
        {
            Result r = FSSEPatternOccurenceFilteredComparison(pattern1, pattern2, mincom);
            if (r.count >= 2)
            {
#pragma omp critical
                results.push_back(r);
            }
            if (r.count > maxOcc)
            {
#pragma omp critical

                maxOcc = r.count;
            }
        }
    }
    std::cout << float(results.size() * 100) / float(occ1.size() * occ2.size()) << "\% of extensions found (" << results.size() << " elements)" << std::endl;
    for (auto r : results)
    {
        float tmpMeanSize = GetMeanSizeExt(r.nodes);
        r.relevancyMetric = GetMetricExt(r.count, maxOcc, tmpMeanSize, maxSize, volume, alpha, beta);
    }
    return results;
}

std::vector<Result> ExtensionResPatt(std::vector<Result> occ1, std::vector<FSSEPatternOccurenceFiltered> occ2, int mincom, int maxSize, int volume, float alpha, float beta)
{
    std::vector<Result> results;
    int maxOcc = 0;
#pragma omp parallel for ordered schedule(dynamic) num_threads(MAX)
    for (auto pattern1 : occ1)
    {
        for (auto pattern2 : occ2)
        {
            Result r = ResToFSSEPatternOccurenceFilteredComparison(pattern1, pattern2, mincom);
            if (r.count >= 2)
            {
#pragma omp critical
                results.push_back(r);
            }
            if (r.count > maxOcc)
            {
#pragma omp critical

                maxOcc = r.count;
            }
        }
    }
    std::cout << float(results.size() * 100) / float(occ1.size() * occ2.size()) << "\% of extensions found (" << results.size() << " elements)" << std::endl;
    for (auto r : results)
    {
        float tmpMeanSize = GetMeanSizeExt(r.nodes);
        r.relevancyMetric = GetMetricExt(r.count, maxOcc, tmpMeanSize, maxSize, volume, alpha, beta);
    }
    return results;
}

void WriteResultsInFile(std::vector<Result> results, std::string path, bool openAsAppend, std::vector<int> current_time)
{
    std::stringstream time;
    for (int i = 0; i < current_time.size() - 1; i++)
    {
        time << current_time[i] << ", ";
    }

    time << std::to_string(current_time[current_time.size() - 1]);
    std::ofstream outfile;                                            // declaration of file pointer named outfile
    outfile.open(path, openAsAppend ? std::ios::app : std::ios::out); // opens file named "filename" for output

    for (auto res : results)
    {
        std::stringstream current_line;
        current_line << time.str() << "\t_\t" << "Pattern : <";
        for (auto globalConVal : res.connectedValues)
        {
            current_line << "<";
            for (auto vec : globalConVal)
            {
                current_line << "<";
                for (int i = 0; i < vec.size() - 1; i++)
                {
                    current_line << std::to_string(vec[i]) << ", ";
                }
                current_line << std::to_string(vec[vec.size() - 1]) << ">, ";
            }
            current_line << ">, ";
        }

        current_line << "\t_\tFrequency : " << res.count;

        // write frequence

        // write last set of nodes
        current_line << "\t_\t" << "Occurences : <";
        for (auto occ : res.nodes)
        {
            current_line << "<";
            for (auto node : occ)
            {
                current_line << node << ", ";
            }
            current_line << ">";
        }
        outfile << current_line.str() << std::endl;
    }
    outfile.close();
}

void ExtensionOrganizer(std::vector<std::vector<FSSEPatternOccurenceFiltered>> patterns, int mincom, int maxSize, std::vector<int> timeComb, std::string file_path, int volume, float alpha, float beta, int threshold)
{
    std::stringstream fp;
    // file_path << "../output/one_att/results_";
    fp << file_path;
    for (int i = 0; i < timeComb.size() - 1; i++)
        fp << timeComb[i] << "_";
    fp << timeComb[timeComb.size() - 1] << ".txt";
    std::vector<int> current_time;
    std::vector<Result> results = ExtensionPattPatt(patterns[timeComb[0]], patterns[timeComb[1]], mincom, maxSize, volume, alpha, beta);
    std::sort(results.begin(), results.end(),
              [](const Result a, const Result b)
              { return a.relevancyMetric > b.relevancyMetric; });
    results = results.size() > threshold ? std::vector<Result>(results.begin(), results.begin() + threshold) : results;

    current_time.push_back(timeComb[0]);
    current_time.push_back(timeComb[1]);
    WriteResultsInFile(results, fp.str(), false, current_time);

    for (int t = 2; t < timeComb.size() && results.size() != 0; t++)
    {
        results = ExtensionResPatt(results, patterns[timeComb[t]], mincom, maxSize, volume, alpha, beta);
        std::sort(results.begin(), results.end(),
                  [](const Result a, const Result b)
                  { return a.relevancyMetric > b.relevancyMetric; });
        results = results.size() > threshold ? std::vector<Result>(results.begin(), results.begin() + threshold) : results;
        current_time.push_back(timeComb[t]);
        if (results.size() == 0)
            break;
        WriteResultsInFile(results, fp.str(), true, current_time);
    }
}

std::vector<std::vector<int>> GenerateTimeComn(int times)
{
    std::vector<std::vector<int>> timeComb;

    for (int i = 0; i < times; i++)
    {
        std::vector<int> currentComb;
        for (int j = i; j < times; j++)
        {
            currentComb.push_back(j);
        }
        timeComb.push_back(currentComb);
    }

    return timeComb;
}

void PatternExtension(std::vector<std::vector<FSSEPatternOccurenceFiltered>> filteredPatterns, int mincom, int supp, int time, std::string file_path, int volume, float alpha, float beta, int threshold)
{

    auto timeComb = GenerateTimeComn(time);

    for (int tc = 0; tc < timeComb.size(); tc++)
    {
        if (timeComb[tc].size() > 1)
        {
            ExtensionOrganizer(filteredPatterns, mincom, supp, timeComb[tc], file_path, volume, alpha, beta, threshold);
        }
    }
}