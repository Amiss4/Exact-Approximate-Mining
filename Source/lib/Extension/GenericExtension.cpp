#include <vector>
#include "./FSSEExtension.h"
#include "../Structures.h"

void GenericExtension(std::vector<std::vector<FSSEPatternOccurenceFiltered>> size1Results, int mincom, int maxSize, int timeCombination,  std::string file_path, int volume, float alpha, float beta, int threshold)
{

    std::vector<std::vector<FSSEPatternOccurenceFiltered>> tmp;
    PatternExtension(size1Results, mincom, maxSize, timeCombination,  file_path,  volume,  alpha,  beta, threshold);
}