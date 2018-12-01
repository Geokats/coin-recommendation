#ifndef POINT

#include <vector>
#include <string>

class point{
private:
  std::string name;
  std::vector<double> vals;

public:
  point(std::string name, int dim, int mean, int dev);
  point(std::string line);
  point(std::string name, std::vector<double> vals);
  point(std::string name, int dim);
  point(const point &p);

  void print();
  void printVals(std::ostream &stream);

  int dim();
  std::string getName();

  double euclDist(point p);
  double cosDist(point p);
  double product(point p);

  point sum(point p);
  bool equal(point p);

  void add(point p);
  void div(float f);
};

#define POINT
#endif
