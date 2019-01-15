#ifndef CLUSTER_CREATOR

#include <vector>

#include "point.hpp"
#include "searcher.hpp"
#include "util.hpp"

class clusterCreator{
  private:
    configuration conf;
    int k;
    std::vector<point*> *clusters;
    std::vector<point> centroids;

    std::vector<point> (clusterCreator::*initialise)(void);
    std::vector<point> randomInit();
    std::vector<point> kmeansInit();

    std::vector<point*>* (clusterCreator::*assign)(std::vector<point> centroids);
    std::vector<point*>* lloydsAssign(std::vector<point> centroids);
    searcher *srch;
    std::vector<point*>* rangeSearchAssign(std::vector<point> centroids);

    std::vector<point> (clusterCreator::*update)(void);
    std::vector<point> kmeansUpdate();
    std::vector<point> pamUpdate();

    std::vector<point> *points;

  public:
    clusterCreator(std::vector<point> *points, configuration conf);
    ~clusterCreator();

    void makeClusters();
    std::vector<point*>* getClusters();
    std::vector<point>* getCentroids();

    float pointSilhouette(point p, int clusterIndex);
    float clusterSilhouette(int clusterIndex);
    std::vector<float> silhouette();
    float avgSilhouette();
};

#define CLUSTER_CREATOR
#endif
