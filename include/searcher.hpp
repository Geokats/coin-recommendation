#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>

#include "point.hpp"
#include "hash_table.hpp"

class searcher{
  protected:
    std::string metric;
    int dim;
    std::vector<point> *points;

  public:
    virtual point *nn(point q, double &minDist) = 0;
    virtual std::unordered_set<point*> rnn(point q, double r) = 0;
};

class lsh : public searcher{
  private:
    int k;
    int L;

    std::vector<hash_table*> tables;

  public:
    lsh(int k, int L, int dim, std::string metric, std::vector<point> *points);
    ~lsh();

    point *nn(point q, double &minDist);
    std::unordered_set<point*> rnn(point q, double r);
};

class hypercube : public searcher{
  private:
    int k;
    int M;
    int p;

    hasher *hashFunc;
    std::vector<std::vector<point*>*> vertices;
    std::vector<std::unordered_map<int, int>*> f;

    int getVertex(point p);

  public:
    hypercube(int k, int M, int p, int dim, std::string metric, std::vector<point> *points);
    ~hypercube();

    point *nn(point q, double &minDist);
    std::unordered_set<point*> rnn(point q, double r);
};
