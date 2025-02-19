#include <vector>
#include "../Structures.h"


std::vector<std::vector<std::vector<int>>> CreateCandidateSpace(std::vector<std::vector<int>> liAtt, int vol);
FSSEPatternOccurence ExtractSize1Patterns(Graph g, std::vector<std::vector<int>> pattern, std::vector<int> selectedAtt);