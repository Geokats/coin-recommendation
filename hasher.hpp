#ifndef HASHER
#define HASHER

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

class cos_hash : public hasher{
  private:
    std::vector<point*> r;

  public:
    cos_hash(int tableSize, int k, int dim);
    ~cos_hash();

    int hash(point p);
};

#endif
