#include "../Structures.h"

std::vector<std::vector<FSSEPatternOccurenceFiltered>> FilterCandidates(std::vector<std::vector<MergedFSSEPatternOccurence>> temporalPatterns, int support, int maxSize, int volume, float alpha = 0.5, float beta = 0.5);
