#ifndef UTIL_CARDINAL_B_SPLINE_CONSTANT_TEST_H
#define UTIL_CARDINAL_B_SPLINE_CONSTANT_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>
#include <util/math/CardinalBSpline.h>
#include <util/math/Binomial.h>
#include <cmath>

class CardinalBSplineConstantTest : public UnitTest
{
public:
   void tearDown() { Util::Binomial::clear(); }

   void testDegreeZero()
   {
      printMethod(TEST_FUNC);
      Util::CardinalBSpline spline(0);
      TEST_ASSERT(spline.degree() == 0);
      TEST_ASSERT(spline[0].size() == 1);
      for (double x : {.25, .5, .75}) {
         TEST_ASSERT(spline(x) == 1.0);
         TEST_ASSERT(spline[0](x) == 1.0);
         double sum = 0.0;
         for (int j = -2; j <= 2; ++j) sum += spline(x-j);
         TEST_ASSERT(sum == 1.0);
      }
      TEST_ASSERT(spline(-.25) == 0.0);
      TEST_ASSERT(spline(1.25) == 0.0);
   }

   void testPositiveDegreeControls()
   {
      printMethod(TEST_FUNC);
      for (int degree : {1, 3, 4}) {
         Util::CardinalBSpline spline(degree);
         for (int segment = 0; segment <= degree; ++segment) {
            for (double fraction : {.25, .5, .75}) {
               const double x = segment + fraction;
               TEST_ASSERT(std::abs(spline(x)-oracle(degree,x)) < 1e-11L);
            }
         }
         for (double x : {.25, .5, .75}) {
            double sum = 0.0;
            for (int j = -degree-2; j <= 2; ++j) sum += spline(x-j);
            TEST_ASSERT(std::abs(sum-1.0) < 1e-11);
         }
         TEST_ASSERT(spline(-.25) == 0.0);
         TEST_ASSERT(spline(degree+1.25) == 0.0);
      }
   }

private:
   // Value recurrence, independent of the polynomial integration in production.
   // All arguments are nonintegers, avoiding endpoint convention differences.
   long double oracle(int degree, long double x)
   {
      if (!degree) return x > 0 && x < 1 ? 1 : 0;
      return x/degree*oracle(degree-1,x)
           + (degree+1-x)/degree*oracle(degree-1,x-1);
   }
};
TEST_BEGIN(CardinalBSplineConstantTest)
TEST_ADD(CardinalBSplineConstantTest, testDegreeZero)
TEST_ADD(CardinalBSplineConstantTest, testPositiveDegreeControls)
TEST_END(CardinalBSplineConstantTest)
#endif
