#ifndef POINT

#include <vector>
#include <string>

class point{
private:
  std::string name;
  std::vector<double> vals;

public:
  point(std::string name, int dim, int min, int max);
  point(std::string line);

  void print();
  int dim();
  std::string getName();
  double distance(point p);
};

#define POINT
#endif
