#ifndef CORE_AVERAGE_VARIANCE_TEST_H
#define CORE_AVERAGE_VARIANCE_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

#include <util/accumulators/Average.h>

#include <cmath>
#include <vector>

using namespace Util;

/*
* Variance, standard deviation and error estimates of an Average for
* series whose standard deviation is small compared to the mean. The
* one-pass formula sumSq/n - (sum/n)^2 of raw values lost all precision
* there: negative variances, NaN standard deviations and errors, or a
* variance off by a factor 19. The results are compared with two-pass
* values computed here.
*/
class CoreAverageVarianceTest : public UnitTest 
{

public:

   void setUp()
   {  state_ = 12345ULL; }

   void tearDown()
   {}

   // 1000 samples of the constant 43.038536
   void testConstantSeries()
   {
      printMethod(TEST_FUNC);

      const double c = 43.038536;
      Average ave;
      for (int i = 0; i < 1000; ++i) {
         ave.sample(c);
      }
      TEST_ASSERT(std::abs(ave.average() - c) < 1.0E-12);
      double variance = ave.variance();
      TEST_ASSERT(variance >= 0.0);
      TEST_ASSERT(variance < 1.0E-20);
      checkSmall(ave.stdDeviation(), 1.0E-10);
      checkSmall(ave.error(), 1.0E-10);
      checkSmall(ave.blockingError(), 1.0E-10);
   }

   // Mean 774, standard deviation 1e-5 (uniform noise), 1000 samples
   void testNearlyConstantSeries()
   {
      printMethod(TEST_FUNC);

      std::vector<double> x(1000);
      const double width = 1.0E-5*std::sqrt(12.0);
      for (int i = 0; i < 1000; ++i) {
         x[i] = 774.0 + width*(uniform() - 0.5);
      }
      checkSeries(x, 1.0E-6);

      // An ordinary series, mean 5 and standard deviation 3: same
      // results as the two-pass values within rounding
      const double width2 = 3.0*std::sqrt(12.0);
      for (int i = 0; i < 1000; ++i) {
         x[i] = 5.0 + width2*(uniform() - 0.5);
      }
      checkSeries(x, 1.0E-12);
   }

private:

   // State of the generator of uniform deviates
   unsigned long long state_;

   // Uniform deviate in [0, 1) from a 64-bit linear congruential generator
   double uniform()
   {
      state_ = state_*6364136223846793005ULL + 1442695040888963407ULL;
      return double(state_ >> 11)*(1.0/9007199254740992.0);
   }

   // Check that a value is finite, non-negative and below max
   void checkSmall(double value, double max)
   {
      TEST_ASSERT(value == value);
      TEST_ASSERT(value >= 0.0);
      TEST_ASSERT(value < max);
   }

   /*
   * Sample x in an Average and compare its variance, standard
   * deviation and error with two-pass values, within the relative
   * tolerance tol. The blocking error of the (uncorrelated) series
   * must be finite and within a factor 2 of the error.
   */
   void checkSeries(std::vector<double> const & x, double tol)
   {
      const int n = x.size();
      Average ave;
      double mean = 0.0;
      for (int i = 0; i < n; ++i) {
         ave.sample(x[i]);
         mean += x[i];
      }
      mean /= double(n);
      double variance = 0.0;
      for (int i = 0; i < n; ++i) {
         variance += (x[i] - mean)*(x[i] - mean);
      }
      variance /= double(n);
      double error = std::sqrt(variance/double(n - 1));

      TEST_ASSERT(std::abs(ave.average() - mean) < 1.0E-12*std::abs(mean));
      TEST_ASSERT(std::abs(ave.variance()/variance - 1.0) < tol);
      TEST_ASSERT(std::abs(ave.stdDeviation()/std::sqrt(variance) - 1.0)
                  < tol);
      TEST_ASSERT(std::abs(ave.error()/error - 1.0) < tol);
      double blockingError = ave.blockingError();
      TEST_ASSERT(blockingError == blockingError);
      TEST_ASSERT(blockingError > 0.5*error);
      TEST_ASSERT(blockingError < 2.0*error);
   }

};

TEST_BEGIN(CoreAverageVarianceTest)
TEST_ADD(CoreAverageVarianceTest, testConstantSeries)
TEST_ADD(CoreAverageVarianceTest, testNearlyConstantSeries)
TEST_END(CoreAverageVarianceTest)

#endif
