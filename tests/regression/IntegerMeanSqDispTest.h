#ifndef UTIL_INTEGER_MEAN_SQ_DISP_TEST_H
#define UTIL_INTEGER_MEAN_SQ_DISP_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>
#include <util/accumulators/MeanSqDispArray.h>
#include <util/containers/DArray.h>
#include <cmath>
#include <limits>
#include <sstream>

using namespace Util;

class IntegerMeanSqDispTest : public UnitTest
{
public:
   void testLargeSquares()
   {
      printMethod(TEST_FUNC);
      TEST_ASSERT(measure<int>(0, 50000) == 2500000000.0);
      TEST_ASSERT(measure<int>(0, -50000) == 2500000000.0);
      TEST_ASSERT(measure<int>(50000, 0) == 2500000000.0);
      TEST_ASSERT(measure<int>(-50000, 0) == 2500000000.0);
   }

   void testDifferenceRange()
   {
      printMethod(TEST_FUNC);
      const int low = std::numeric_limits<int>::min();
      const int high = std::numeric_limits<int>::max();
      // A long-double oracle keeps the subtraction outside integer arithmetic.
      const long double distance = (long double)high - (long double)low;
      const double expected = double(distance*distance);
      const double forward = measure<int>(low, high);
      const double reverse = measure<int>(high, low);
      TEST_ASSERT(std::isfinite(forward));
      TEST_ASSERT(forward > 0.0);
      TEST_ASSERT(forward == reverse);
      // The public output uses Dbl's default finite decimal precision.
      TEST_ASSERT(std::abs(forward/expected - 1.0) < 1.0e-8);
   }

   void testOrdinaryAndDoubleControls()
   {
      printMethod(TEST_FUNC);
      TEST_ASSERT(measure<int>(0, 40000) == 1600000000.0);
      TEST_ASSERT(measure<double>(0.0, 50000.0) == 2500000000.0);
      TEST_ASSERT(measure<int>(123, 123) == 0.0);
   }

private:
   template <typename Data>
   double measure(Data first, Data second)
   {
      MeanSqDispArray<Data> msd;
      msd.setParam(1, 4);
      DArray<Data> values;
      values.allocate(1);
      values[0] = first;
      msd.sample(values);
      values[0] = second;
      msd.sample(values);
      std::ostringstream output;
      msd.output(output);
      std::istringstream input(output.str());
      int lag0 = -1, lag1 = -1;
      double value0 = -1.0, value1 = -1.0;
      input >> lag0 >> value0 >> lag1 >> value1;
      TEST_ASSERT(!input.fail());
      TEST_ASSERT(lag0 == 0 && lag1 == 1);
      TEST_ASSERT(value0 == 0.0);
      return value1;
   }
};

TEST_BEGIN(IntegerMeanSqDispTest)
TEST_ADD(IntegerMeanSqDispTest, testLargeSquares)
TEST_ADD(IntegerMeanSqDispTest, testDifferenceRange)
TEST_ADD(IntegerMeanSqDispTest, testOrdinaryAndDoubleControls)
TEST_END(IntegerMeanSqDispTest)

#endif
