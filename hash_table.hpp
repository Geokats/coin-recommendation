#ifndef HASH_TABLE
#define HASH_TABLE

#include <vector>
#include <string>

#include "point.hpp"

class hasher{
  protected:
    int tableSize;
    int k;
    int dim;
  public:
    virtual int hash(point p) = 0;
};

class eucl_hash : public hasher{
  private:
    std::vector<point> v;
    std::vector<double> t;
    const static unsigned int w = 4;

    std::vector<int> r;

  public:
    eucl_hash(int tableSize, int k, int dim);

    int hash(point p);
};

class hash_table{
  private:
    int tableSize;
    std::vector<std::vector<point*>*> buckets;
    hasher *hashFunc;

  public:
    hash_table(int k, int dim, std::vector<point> *points, std::string metric);
    ~hash_table();

    std::vector<point*> *getBucket(point p);
};

#endif
