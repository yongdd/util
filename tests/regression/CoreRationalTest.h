#ifndef CORE_RATIONAL_TEST_H
#define CORE_RATIONAL_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

#include <util/math/Rational.h>

#include <sstream>
#include <string>

using namespace Util;

/*
* A Rational read from a stream must be stored in reduced form, as the
* constructors and the arithmetic operators store it: equality compares
* numerators and denominators, so '2/4' read unreduced was not equal to
* 1/2, and a group file with such a translation was rejected.
*/
class CoreRationalTest : public UnitTest 
{

public:

   void setUp()
   {}

   void tearDown()
   {}

   void testReadReduced()
   {
      printMethod(TEST_FUNC);
      checkRead("2/4", 1, 2);
      checkRead("0/2", 0, 1);
      checkRead("2/6", 1, 3);
      checkRead("-2/4", -1, 2);
      checkRead("6/3", 2, 1);
      checkRead("3/4", 3, 4);
      checkRead("5", 5, 1);
   }

private:

   // Read a Rational from text and compare it with num/den
   void checkRead(std::string const & text, int num, int den)
   {
      std::istringstream in(text);
      Rational r;
      in >> r;
      TEST_ASSERT(r.num() == num);
      TEST_ASSERT(r.den() == den);
      TEST_ASSERT(r == Rational(num, den));
   }

};

TEST_BEGIN(CoreRationalTest)
TEST_ADD(CoreRationalTest, testReadReduced)
TEST_END(CoreRationalTest)

#endif
