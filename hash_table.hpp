#ifndef HASH_TABLE
#define HASH_TABLE

#include <vector>
#include <string>

#include "point.hpp"
#include "hasher.hpp"

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
