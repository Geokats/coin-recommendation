#ifndef UTIL
#define UTIL

#include <vector>
#include <iostream>

#include "point.hpp"

class configuration{
  private:
    int clusterCount;
    int hashFuncCount;
    int hashTableCount;

  public:
    configuration();

    bool clusterConf();
    /*
    * Check configuration is sufficient for clustering
    */

    void setClusterCount(int val){clusterCount = val;};
    void setHashFuncCount(int val){hashFuncCount = val;};
    void setHashTableCount(int val){hashTableCount = val;};

    int getClusterCount(){return clusterCount;};
    int getHashFuncCount(){return hashFuncCount;};
    int getHashTableCount(){return hashTableCount;};
};

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

void readConfigFile(std::string configFileName, configuration &conf);

#endif
