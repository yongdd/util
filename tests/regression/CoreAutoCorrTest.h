#ifndef CORE_AUTO_CORR_TEST_H
#define CORE_AUTO_CORR_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

#include <util/accumulators/AutoCorr.h>
#include <util/accumulators/AutoCorrelation.tpp>   // template

#include <cmath>
#include <sstream>

using namespace Util;

class CoreAutoCorrTest : public UnitTest 
{

public:

   void setUp()
   {  state_ = 12345ULL; }

   void tearDown()
   {}

   /*
   * AR(1) series x' - 5 = 0.8 (x - 5) + 0.6 eps with unit variance
   * noise, 2e5 samples, buffer capacity 64. corrTime() sums the
   * normalized autocorrelations for lags 1 to 31, about
   * sum_{i=1}^{31} 0.8^i = 3.996. It divided the mean by nSample twice,
   * so the mean was not subtracted (about 30).
   */
   void testCorrTime()
   {
      printMethod(TEST_FUNC);

      AutoCorr<double, double> acf;
      acf.setParam(64);
      const double width = std::sqrt(12.0);
      double y = 0.0;
      for (int i = 0; i < 200000; ++i) {
         y = 0.8*y + 0.6*width*(uniform() - 0.5);
         acf.sample(5.0 + y);
      }
      double expected = 0.0;
      double r = 1.0;
      for (int i = 1; i < 32; ++i) {
         r *= 0.8;
         expected += r;
      }
      TEST_ASSERT(std::abs(acf.corrTime() - expected) < 0.3);
   }

   /*
   * clear() and reuse of an AutoCorrelation: after clear(), the output
   * must be that of a new object fed the same values. AutoCorrStage::
   * clear() kept the descendant stages and their data, so the output
   * after clear() still listed long lags from the first data set.
   */
   void testAutoCorrelationClear()
   {
      printMethod(TEST_FUNC);

      AutoCorrelation<double, double> reused;
      reused.setParam(8, 10, 2);
      int i;
      for (i = 0; i < 4096; ++i) {
         reused.sample(10.0 + std::sin(0.3*double(i)));
      }
      TEST_ASSERT(reused.maxDelay() > 1000);
      reused.clear();

      AutoCorrelation<double, double> fresh;
      fresh.setParam(8, 10, 2);
      for (i = 0; i < 64; ++i) {
         reused.sample(1.0 + std::cos(0.7*double(i)));
         fresh.sample(1.0 + std::cos(0.7*double(i)));
      }
      TEST_ASSERT(reused.nSample() == 64);
      TEST_ASSERT(reused.maxDelay() == fresh.maxDelay());
      std::ostringstream reusedOut, freshOut;
      reused.output(reusedOut);
      fresh.output(freshOut);
      TEST_ASSERT(reusedOut.str() == freshOut.str());

      // Clearing twice, and destruction after clear(), are safe
      reused.clear();
      reused.clear();
      TEST_ASSERT(reused.nSample() == 0);
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

};

TEST_BEGIN(CoreAutoCorrTest)
TEST_ADD(CoreAutoCorrTest, testCorrTime)
TEST_ADD(CoreAutoCorrTest, testAutoCorrelationClear)
TEST_END(CoreAutoCorrTest)

#endif
