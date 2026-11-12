#ifndef CORE_DISTRIBUTION_TEST_H
#define CORE_DISTRIBUTION_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

#include <util/accumulators/Distribution.h>

#include <cmath>
#include <sstream>
#include <string>

using namespace Util;

/*
* A value just below max must fall in the last bin. Rounding of
* (value - min)/binWidth gave nBin, and sample() wrote one element past
* the end of the histogram.
*/
class CoreDistributionTest : public UnitTest 
{

public:

   void setUp()
   {}

   void tearDown()
   {}

   void testBinIndexBelowMax()
   {
      printMethod(TEST_FUNC);

      Distribution d;
      d.setParam(0.0, 1.0, 3);
      double x = std::nextafter(1.0, 0.0);
      TEST_ASSERT(d.binIndex(x) == 2);
      TEST_ASSERT(d.binIndex(std::nextafter(0.0, 1.0)) == 0);

      Distribution e;
      e.setParam(-1.0, 1.0, 3);
      TEST_ASSERT(e.binIndex(x) == 2);

      // The sample is counted in the last bin
      d.sample(x);
      d.sample(0.5);
      std::ostringstream out;
      d.output(out);
      std::istringstream in(out.str());
      double center, rho;
      for (int i = 0; i < 3; ++i) {
         in >> center >> rho;
         // density = count/(nSample*binWidth), 2 samples, width 1/3
         if (i == 0) TEST_ASSERT(rho == 0.0);
         if (i > 0) TEST_ASSERT(std::abs(rho - 1.5) < 1.0E-6);
      }
   }

};

TEST_BEGIN(CoreDistributionTest)
TEST_ADD(CoreDistributionTest, testBinIndexBelowMax)
TEST_END(CoreDistributionTest)

#endif
