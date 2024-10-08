#include "algorithm.h"

#include <iostream>
#include <vector>
#include <queue>

using namespace std;


vector<int> Algorithm::FindShortestPath(vector<vector<int>>& nodes, int startingNode, int endingNode)
{
    vector<bool> passed(nodes.size(), 0);
    vector<bool> noted(nodes.size(), 0);
    vector<int> minimumLengths(nodes.size(), -1);
    minimumLengths[startingNode] = 0;
    vector<vector<int>> minimumPaths(nodes.size());
    minimumPaths[startingNode] = {startingNode};

    queue<int> traversalOrder;
    traversalOrder.push(startingNode);
    int currentNode;
    int cycle = 1;

    while (!traversalOrder.empty()) {
        currentNode = traversalOrder.front();
        if (currentNode == endingNode) break;
        traversalOrder.pop();
        passed[currentNode] = 1;

        for (int i = 0; i < nodes[currentNode].size(); ++i) {
            int nodeToTravelTo = nodes[currentNode][i];
            if (!passed[nodeToTravelTo]) {
                if (!noted[nodeToTravelTo]) {
                    traversalOrder.push(nodeToTravelTo);
                    noted[nodeToTravelTo] = 1;
                }
                if (minimumLengths[nodeToTravelTo] == -1 || minimumLengths[currentNode] + 1 < minimumLengths[nodeToTravelTo]) {
                    minimumLengths[nodeToTravelTo] = minimumLengths[currentNode] + 1;
                    minimumPaths[nodeToTravelTo] = minimumPaths[currentNode];
                    minimumPaths[nodeToTravelTo].push_back(nodeToTravelTo);
                }
            }
        }
    }

    return minimumPaths[endingNode];
}
