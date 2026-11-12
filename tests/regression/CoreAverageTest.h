#ifndef CORE_AVERAGE_TEST_H
#define CORE_AVERAGE_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

#include <util/accumulators/Average.h>

#include <cmath>
#include <sstream>

using namespace Util;

class CoreAverageTest : public UnitTest
{

public:

   void setUp()
   {}

   void tearDown()
   {}

   /*
   * Clear an Average that has descendant stages and reuse it, as an
   * averaging analyzer does in a second SIMULATE command. After clear(),
   * it must give the same results as a new Average fed the same values.
   * Without the fix, clear() deleted the descendant stages but kept
   * pointers to them, so the next samples wrote through dangling
   * pointers.
   */
   void testClearReuse()
   {
      printMethod(TEST_FUNC);

      // 100 samples, enough for several descendant stages (blockFactor 2)
      Average reused;
      reused.setNSamplePerBlock(4);
      int i;
      for (i = 0; i < 100; ++i) {
         reused.sample(value(i, 0));
      }
      TEST_ASSERT(reused.nSample() == 100);

      int n;
      for (int set = 1; set < 4; ++set) {

         reused.clear();
         TEST_ASSERT(reused.nSample() == 0);
         TEST_ASSERT(reused.iBlock() == 0);

         Average fresh;
         fresh.setNSamplePerBlock(4);
         n = 40 + 50*set;
         for (i = 0; i < n; ++i) {
            reused.sample(value(i, set));
            fresh.sample(value(i, set));
         }

         TEST_ASSERT(reused.nSample() == n);
         TEST_ASSERT(fresh.nSample() == n);
         TEST_ASSERT(reused.average() == fresh.average());
         TEST_ASSERT(reused.variance() == fresh.variance());
         TEST_ASSERT(reused.blockingError() == fresh.blockingError());
         TEST_ASSERT(reused.blockAverage() == fresh.blockAverage());

         // The output lists every stage of the hierarchical analysis
         std::ostringstream reusedOut, freshOut;
         reused.output(reusedOut);
         fresh.output(freshOut);
         TEST_ASSERT(reusedOut.str() == freshOut.str());
      }

      // Clearing twice in a row, and destruction after clear(), are safe
      reused.clear();
      reused.clear();
      TEST_ASSERT(reused.nSample() == 0);
   }

private:

   // Deterministic sequence of sampled values, one per data set
   double value(int i, int set) const
   {  return std::sin(0.37*double(i) + double(set)) + 0.01*double(i); }

};

TEST_BEGIN(CoreAverageTest)
TEST_ADD(CoreAverageTest, testClearReuse)
TEST_END(CoreAverageTest)

#endif
