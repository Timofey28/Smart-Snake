#pragma once

#include <vector>


class Algorithm
{
public:
    static std::vector<int> FindShortestPath(
        const std::vector<std::vector<int>>& nodes,
        int startingNode,
        int endingNode
    );
};
