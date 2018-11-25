#ifndef CLUSTER_CREATOR

#include <vector>

#include "point.hpp"
#include "util.hpp"

class clusterCreator{
  private:
    int k;
    std::vector<point*> *clusters;
    std::vector<point> centroids;

    void (clusterCreator::*initialise)(void);
    void randomInit();

    void (clusterCreator::*assign)(void);
    void lloydsAssign();

    std::vector<point> (clusterCreator::*update)(void);
    std::vector<point> kmeansUpdate();

    std::vector<point> *points;

  public:
    clusterCreator(std::vector<point> *points, configuration conf);

    void makeClusters();
    std::vector<point*>* getClusters();
    std::vector<point>* getCentroids();

    float pointSilhouette(point p, int clusterIndex);
    float clusterSilhouette(int clusterIndex);
    std::vector<float> silhouette();
};

#define CLUSTER_CREATOR
#endif
