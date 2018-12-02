#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>

#include "pointTest.hpp"

int main(int argc, char const *argv[]){
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(new CppUnit::TestCaller<pointTest>("testEqual", &pointTest::testEqual));;
  runner.addTest(new CppUnit::TestCaller<pointTest>("testEuclDist", &pointTest::testEuclDist));;
  runner.addTest(new CppUnit::TestCaller<pointTest>("testCosDist", &pointTest::testCosDist));;
  runner.addTest(new CppUnit::TestCaller<pointTest>("testProduct", &pointTest::testProduct));;
  runner.addTest(new CppUnit::TestCaller<pointTest>("testSum", &pointTest::testSum));;

  runner.run();
  return 0;
}
