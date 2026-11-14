#ifndef UTIL_RATIONAL_SELF_DIVISION_TEST_H
#define UTIL_RATIONAL_SELF_DIVISION_TEST_H
#include <util/math/Rational.h>
#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

class RationalSelfDivisionTest : public UnitTest
{
public:
   void testSelfDivision()
   {
      printMethod(TEST_FUNC);
      for (int p : {2, -2, 7, -7}) {
         for (int q : {1, 3, 5}) {
            Util::Rational value(p, q);
            Util::Rational& result = (value /= value);
            TEST_ASSERT(&result == &value);
            TEST_ASSERT(value.num() == 1 && value.den() == 1);
         }
      }
   }
   void testDistinctOperands()
   {
      printMethod(TEST_FUNC);
      Util::Rational value(2,3), divisor(-7,5), saved(divisor);
      Util::Rational expected = value / divisor;
      value /= divisor;
      TEST_ASSERT(value == expected && divisor == saved);
      value = 0;
      value /= divisor;
      TEST_ASSERT(value.num() == 0 && value.den() == 1);
      value = Util::Rational(2,3);
      value /= -2;
      TEST_ASSERT(value == Util::Rational(-1,3));
   }
};
TEST_BEGIN(RationalSelfDivisionTest)
TEST_ADD(RationalSelfDivisionTest, testSelfDivision)
TEST_ADD(RationalSelfDivisionTest, testDistinctOperands)
TEST_END(RationalSelfDivisionTest)
#endif
