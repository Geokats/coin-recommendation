#ifndef CLUSTER_CREATOR

#include <vector>

#include "point.hpp"

class clusterCreator{
  private:
    int k;
    std::vector<point*> *clusters;
    std::vector<point> centroids;

    void (clusterCreator::*initialise)(void);
    void randomInit();

    void (clusterCreator::*assign)(void);
    void lloydsAssign();

    void (clusterCreator::*update)(void);

    std::vector<point> *points;

  public:
    clusterCreator(std::vector<point> *points, int k);

    void makeClusters();
    std::vector<point*>* getClusters();
    std::vector<point>* getCentroids();
};

#define CLUSTER_CREATOR
#endif
