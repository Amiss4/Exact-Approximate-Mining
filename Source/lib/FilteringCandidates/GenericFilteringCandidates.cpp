#include "../Structures.h"
#include "./FSSEFiltering.h"
#include "./FilteringMergedFSSEPatterns.h"

std::vector<std::vector<FSSEPatternOccurenceFiltered>> FilterFSSE(std::vector<std::vector<MergedFSSEPatternOccurence>> temporalPatterns, int support, int maxSize, int volume, float alpha = 0.5, float beta = 0.5)
{

    return FilterCandidates(temporalPatterns, support, maxSize, volume, alpha, beta);
}

// std::vector<std::vector<FSSEPatternOccurenceFiltered>> FilterMergedFSSE(std::vector<std::vector<MergedFSSEPatternOccurence>> temporalPatterns, int support)
// {

//     return FilterMergedCandidates(temporalPatterns, support);
// }