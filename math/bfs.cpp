#include "bfs.h"

namespace BreadthFirstSearch {

std::unordered_map<int, int> createMap(int radius, std::vector<int> directions)
{
    std::vector<int> frontier;
    frontier.push_back(0);
    std::unordered_map<int, int> cameFrom;
    cameFrom[0] = 0; // do we need this?
    while ( ! frontier.empty())
    {
        int current = frontier.front();
        frontier.erase(frontier.begin());
        std::vector<int>::iterator it = directions.begin();
        for (; it != directions.end(); it++)
        {
            int next = current + (*it);
            if (std::abs(next) > radius)
                continue;
            if (cameFrom.find(next) == cameFrom.end()) // next not in cameFrom
            {
                frontier.push_back(next);
                cameFrom[next] = current;
            }
        }
    }
    return cameFrom;
}

std::vector<int> getPath(std::unordered_map<int, int> map, int goal)
{
    int current = goal;
    std::vector<int> path;
    while (current != 0) // 0 is the starting point. To change, just shift
    {
        path.push_back(current);
        current = map[current];
    }
    path.push_back(0);
    return path;
}

} // namespace
