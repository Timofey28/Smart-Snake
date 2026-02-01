#include "algorithm.h"
using namespace std;

vector<int> Algorithm::FindShortestPath(const vector<vector<int>>& nodes, int startingNode, int endingNode)
{
    vector<int> minimumDistances(nodes.size(), -1);
    minimumDistances[startingNode] = 0;
    vector<vector<int>> minimumPaths(nodes.size());
    minimumPaths[startingNode] = {};
    vector<bool> passed(nodes.size(), 0);
    passed[startingNode] = 1;

    queue<int> traversalOrder;
    traversalOrder.push(startingNode);

    int currentNode;
    while (!traversalOrder.empty()) {
        currentNode = traversalOrder.front();
        traversalOrder.pop();

        for (int i = 0; i < nodes[currentNode].size(); ++i) {
            int nodeToTravelTo = nodes[currentNode][i];
            if (!passed[nodeToTravelTo]) {
                passed[nodeToTravelTo] = 1;
                traversalOrder.push(nodeToTravelTo);
                if (minimumPaths[nodeToTravelTo].empty() || minimumDistances[currentNode] + 1 < minimumDistances[nodeToTravelTo]) {
                    minimumDistances[nodeToTravelTo] = minimumDistances[currentNode] + 1;
                    minimumPaths[nodeToTravelTo] = minimumPaths[currentNode];
                    minimumPaths[nodeToTravelTo].push_back(nodeToTravelTo);
                    if (nodeToTravelTo == endingNode) return minimumPaths[endingNode];
                }
            }
        }
    }

    return {};
}
