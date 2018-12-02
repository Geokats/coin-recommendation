#include "pointTest.hpp"

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <vector>
#include <random>

#include "point.hpp"

using namespace std;

void pointTest::setUp(){
  std::default_random_engine generator;
  std::uniform_real_distribution<double> distribution(0.0,1.0);

  vector<double> vals1, vals2;
  //Generate random vector of 20 reals
  for(int i = 0; i < 20; i++){
    double v = distribution(generator);
    vals1.push_back(v);
  }
  //Generate random vector of 20 reals
  for(int i = 0; i < 20; i++){
    double v = distribution(generator);
    vals2.push_back(v + 1.0);
  }

  p1 = new point("p1", vals1);
  p2 = new point("p2", vals1);
  p3 = new point("p3", vals2);
}

void pointTest::tearDown(){
  delete p1;
  delete p2;
  delete p3;
}

void pointTest::testEqual(){
  CPPUNIT_ASSERT(p1->equal(*p1));
  CPPUNIT_ASSERT(p1->equal(*p2));
  CPPUNIT_ASSERT(p2->equal(*p1));

  CPPUNIT_ASSERT(! p1->equal(*p3));
  CPPUNIT_ASSERT(! p3->equal(*p1));
}

void pointTest::testEuclDist(){
  CPPUNIT_ASSERT(p1->euclDist(*p1) == 0);
  CPPUNIT_ASSERT(p1->euclDist(*p2) == 0);
  CPPUNIT_ASSERT(p2->euclDist(*p1) == 0);

  CPPUNIT_ASSERT(p1->euclDist(*p3) != 0);
  CPPUNIT_ASSERT(p3->euclDist(*p1) != 0);

  CPPUNIT_ASSERT(p3->euclDist(*p1) == p1->euclDist(*p3));
}

void pointTest::testCosDist(){
  CPPUNIT_ASSERT(p1->cosDist(*p1) == 1);
  CPPUNIT_ASSERT(p1->cosDist(*p2) == 1);
  CPPUNIT_ASSERT(p2->cosDist(*p1) == 1);

  CPPUNIT_ASSERT(p1->cosDist(*p3) != 1);
  CPPUNIT_ASSERT(p3->cosDist(*p1) != 1);

  CPPUNIT_ASSERT(p3->cosDist(*p1) == p1->cosDist(*p3));
}

void pointTest::testProduct(){
  CPPUNIT_ASSERT(p3->product(*p1) == p1->product(*p3));
}

void pointTest::testSum(){
  point sum1 = p3->sum(*p1);
  point sum2 = p1->sum(*p3);
  CPPUNIT_ASSERT(sum1.equal(sum2));
}
