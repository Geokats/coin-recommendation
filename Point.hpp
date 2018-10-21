#ifndef POINT

#include <vector>
#include <string>

class Point{
private:
  std::string name;
  std::vector<int> vals;

public:
  Point(std::string line);
  // virtual ~Point();

  void print();
  int dim();
  std::string getName();
  double distance(Point p);
};

#define POINT
#endif
