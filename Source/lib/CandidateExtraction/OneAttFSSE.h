#include <vector>

std::vector<std::vector<int>> OneAttCreateCandidateSpace(std::vector<int> liAtt, int vol);
FSSEPatternOccurence OneAttExtractSize1Patterns(Graph g, std::vector<int> pattern, int selectedAtt);
std::vector<std::vector<MergedFSSEPatternOccurence>> MergePatterns(std::vector<std::vector<std::vector<FSSEPatternOccurence>>> p, std::vector<std::vector<std::vector<int>>> globalCompConnect, int volume, int threshold);
