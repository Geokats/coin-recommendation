#ifndef UTIL
#define UTIL

#include <vector>
#include <iostream>
#include "point.hpp"

point *get_true_nn(point q, double &minDist, std::vector<point> *points);

void readInputFile(std::string inputFileName, std::vector<point> &points, int &dim, std::string &metric);
void readQueryFile(std::string queryFileName, std::vector<point> &queries, int &dim, double &radius);

#endif
