#ifndef POINT

#include <vector>
#include <string>

class point{
private:
  std::string name;
  std::vector<int> vals;

public:
  point(std::string line);
  // virtual ~point();

  void print();
  int dim();
  std::string getName();
  double distance(point p);
};

#define POINT
#endif
