#ifndef UTIL
#define UTIL

#include <vector>
#include <unordered_map>
#include <iostream>
#include <string>

#include "point.hpp"

class configuration{
  private:
    int clusterCount;

    int hashFuncCount;
    int hashTableCount;

    int HCDimension;
    int HCMaxPointChecks;
    int HCMaxVertexChecks;

    int maxIterations;

    std::string initialise;
    std::string assign;
    std::string update;

    std::string metric;

  public:
    configuration();

    bool clusterConf();
    /*
    * Check if configuration is sufficient for clustering
    */

    void print();

    void setClusterCount(int val){clusterCount = val;};
    void setHashFuncCount(int val){hashFuncCount = val;};
    void setHashTableCount(int val){hashTableCount = val;};
    void setHCDimension(int val){HCDimension = val;};
    void setHCMaxPointChecks(int val){HCMaxPointChecks = val;};
    void setHCMaxVertexChecks(int val){HCMaxVertexChecks = val;};
    void setMaxIterations(int val){maxIterations = val;};
    void setInitialise(std::string str){initialise = str;};
    void setAssign(std::string str){assign = str;};
    void setUpdate(std::string str){update = str;};
    void setMetric(std::string str){metric = str;};

    int getClusterCount(){return clusterCount;};
    int getHashFuncCount(){return hashFuncCount;};
    int getHashTableCount(){return hashTableCount;};
    int getHCDimension(){return HCDimension;};
    int getHCMaxPointChecks(){return HCMaxPointChecks;};
    int getHCMaxVertexChecks(){return HCMaxVertexChecks;};
    int getMaxIterations(){return maxIterations;};
    std::string getInitialise(){return initialise;};
    std::string getAssign(){return assign;};
    std::string getUpdate(){return update;};
    std::string getMetric(){return metric;};
};

class tweet{
  public:
    int tweetId;
    int userId;
    std::vector<std::string> words;
};

float distance(point p1, point p2, std::string metric);

point *get_true_nn(point q, double &minDist, std::vector<point> *points, std::string metric);
/*
* Returns the deterministic nearest neighbor of @q from point set @points and
* stores the distance between them in @minDist.
*/

void readInputFile(std::string inputFileName, std::vector<point> &points, int &dim, std::string &metric);
void readInputFile(std::string inputFileName, std::vector<point> &points, int &dim);
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

void readLexiconFile(std::string lexiconFileName, std::unordered_map<std::string, float> &lexicon);

void readCoinsFile(std::string coinsFileName, std::vector<std::string> &coins, std::unordered_map<std::string, int> &coinLexicon);

void readTweetsFile(std::string tweetsFileName, std::vector<tweet> &tweets);

#endif
