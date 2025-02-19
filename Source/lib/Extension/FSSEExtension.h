#include <vector>
#include "../Structures.h"
std::vector<std::vector<int>> GenerateTimeCombinations(int numberOfTime);
void PatternExtension(std::vector<std::vector<FSSEPatternOccurenceFiltered>>, int mincom, int supp, int timeCombination,  std::string file_path, int volume, float alpha, float beta, int threshold);