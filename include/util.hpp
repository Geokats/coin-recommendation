#ifndef UTIL
#define UTIL

#include <vector>
#include <iostream>

#include "point.hpp"

point *get_true_nn(point q, double &minDist, std::vector<point> *points);
/*
* Returns the deterministic nearest neighbor of @q from point set @points and
* stores the distance between them in @minDist.
*/

void readInputFile(std::string inputFileName, std::vector<point> &points, int &dim, std::string &metric);
/*
* Opens file @inputFileName, reads the points and the metric within it and saves
* them in @points and @metric. Also saves the dimension of the points in
* variable @dim. If the dimensions of the points are inconsistent the value -1
* is stored in @dim instead. After being read, the file is closed.
*/

void readQueryFile(std::string queryFileName, std::vector<point> &queries, int &dim, double &radius);
/*
* Opens file @queryFileName, reads the points and the radius within it and saves
* them in @queries and @radius. Also saves the dimension of the points in
* variable @dim. If the dimensions of the points are inconsistent the value -1
* is stored in @dim instead. After being read, the file is closed.
*/

#endif
