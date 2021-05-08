#ifndef BFS_H
#define BFS_H
/*
 * This is an implementation of Breadth-first search (BFS) algorithm
 * The function createMap is a 1-dimension version. It gets a radius as input as well as
 * a vector of valid directions and generates a map with these directions. This is only in
 * 1D and the directions can be integers, such as the vector {-3, 3, -4, 4, -7, 7}. Then
 * it will retrieve a map containing, for every int from 0 to radius, one of the shortest
 * paths to 0 using the directions given.
 *
 * The function getPath retrieves the path given the map and the int to go to starting from 0.
 * */
#include <vector>
#include <unordered_map>

namespace BreadthFirstSearch {

std::unordered_map<int, int> createMap(int radius, std::vector<int> directions);
std::vector<int> getPath(std::unordered_map<int, int> map, int goal);

}

#endif // BFS_H
