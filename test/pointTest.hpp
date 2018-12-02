#include <cppunit/TestFixture.h>
#include "point.hpp"

class pointTest : public CppUnit::TestFixture{
  private:
    point *p1, *p2, *p3;

  public:
    void setUp();
    void tearDown();

    void testEqual();
    void testEuclDist();
    void testCosDist();
    void testProduct();
    void testSum();
};
