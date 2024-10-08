#pragma once

#include <vector>


class Algorithm
{
public:
    static std::vector<int> FindShortestPath(
        std::vector<std::vector<int>>& nodes,
        int startingNode,
        int endingNode
    );
};
