#ifndef CORE_DISTRIBUTION_TEST_H
#define CORE_DISTRIBUTION_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

#include <util/accumulators/Distribution.h>
#include <util/accumulators/IntDistribution.h>

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

   template <class T>
   std::string histogramText(T& histogram)
   {
      std::ostringstream out;
      histogram.output(out);
      return out.str();
   }

   template <class T>
   void checkAssignment(T& source, T& destination)
   {
      destination = source;
      TEST_ASSERT(histogramText(destination) == histogramText(source));
      T copy(source), fresh;
      fresh = source;
      source = source;
      TEST_ASSERT(histogramText(copy) == histogramText(source));
      TEST_ASSERT(histogramText(fresh) == histogramText(source));
      // Continuing both histograms also checks copied normalization counts.
      destination.sample(source.min());
      source.sample(source.min());
      TEST_ASSERT(histogramText(destination) == histogramText(source));
   }

   void testAssignment()
   {
      printMethod(TEST_FUNC);
      Distribution source, same, smaller, larger;
      source.setParam(0, 1, 4);
      same.setParam(0, 1, 4);
      smaller.setParam(-2, 2, 2);
      larger.setParam(-3, 3, 8);
      source.sample(.2);
      source.sample(.7);
      source.sample(2);
      same.sample(.4);
      checkAssignment(source, same);
      checkAssignment(source, smaller);
      checkAssignment(source, larger);
      Distribution empty;
      same = empty;
      TEST_ASSERT(same.min() == 0 && same.max() == 0);
      // Assignment to empty leaves it reusable, without retained allocation.
      same.setParam(0, 1, 4);
      checkAssignment(source, same);
   }

   void testIntegerAssignment()
   {
      printMethod(TEST_FUNC);
      IntDistribution source, same, smaller, larger;
      source.setParam(0, 3);
      same.setParam(0, 3);
      smaller.setParam(0, 1);
      larger.setParam(-2, 5);
      source.sample(0);
      source.sample(2);
      source.sample(4);
      same.sample(1);
      checkAssignment(source, same);
      checkAssignment(source, smaller);
      checkAssignment(source, larger);
      IntDistribution empty;
      same = empty;
      TEST_ASSERT(same.min() == 0 && same.max() == 0);
      same.setParam(0, 3);
      checkAssignment(source, same);
   }

};

TEST_BEGIN(CoreDistributionTest)
TEST_ADD(CoreDistributionTest, testBinIndexBelowMax)
TEST_ADD(CoreDistributionTest, testAssignment)
TEST_ADD(CoreDistributionTest, testIntegerAssignment)
TEST_END(CoreDistributionTest)

#endif
