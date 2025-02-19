#include "../Structures.h"

std::vector<std::vector<FSSEPatternOccurence>> FSSEExtraction(GraphReadingResults grr, int volume, std::vector<int> selected_attribute);
std::vector<std::vector<MergedFSSEPatternOccurence>> OneAttFSSEExtraction(GraphReadingResults grr, int volume, std::vector<int> selected_attribute, int support, int threshold);