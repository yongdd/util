#ifndef UTIL_RADIAL_SHELL_VOLUME_TEST_H
#define UTIL_RADIAL_SHELL_VOLUME_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>
#include <util/accumulators/RadialDistribution.h>
#include <algorithm>
#include <cmath>
#include <sstream>

class RadialShellVolumeTest : public UnitTest
{
public:
   void testDensity()
   {
      printMethod(TEST_FUNC);
      check(false);
   }

   void testIntegral()
   {
      printMethod(TEST_FUNC);
      check(true);
   }

private:
   void check(bool integral)
   {
      const int counts[] = {100, 26000, 26755, 26756, 30000};
      const long double pi = std::acos(-1.0L);
      for (int n : counts) {
         Util::RadialDistribution distribution;
         distribution.setParam(1.0, n);
         distribution.setNorm(2.0);
         distribution.setOutputIntegral(integral);
         distribution.beginSnapshot();
         distribution.sample(0.5/n);
         distribution.sample((n - 0.5)/n);
         std::ostringstream out;
         distribution.output(out);
         std::istringstream in(out.str());
         for (int i = 0; i < n; ++i) {
            double center, density, accumulated = 0.0;
            TEST_ASSERT(bool(in >> center >> density));
            if (integral) TEST_ASSERT(bool(in >> accumulated));
            // Integrate the shell independently of the index polynomial.
            const long double lo = static_cast<long double>(i)/n;
            const long double hi = static_cast<long double>(i+1)/n;
            const long double volume = (4.0L*pi/3.0L)*
                                       (hi*hi*hi - lo*lo*lo);
            const long double count = (i == 0 || i == n-1) ? 1.0L : 0.0L;
            const long double expected = count/(2.0L*volume);
            // The text writer retains eight digits after the decimal.
            TEST_ASSERT(std::isfinite(density) && density >= 0.0);
            TEST_ASSERT(std::fabs(density - expected) <=
                        3.0e-8L*std::max(1.0L, std::fabs(expected)));
            if (integral) {
               // Preserve the existing integral of g-1 convention.
               const long double total = i == n-1 ? 1.0L : 0.5L;
               const long double expectedIntegral = total - 4*pi*hi*hi*hi/3;
               TEST_ASSERT(std::isfinite(accumulated));
               TEST_ASSERT(std::fabs(accumulated - expectedIntegral) <=
                           3.0e-8L*std::max(1.0L, std::fabs(expectedIntegral)));
            }
         }
         std::string extra;
         TEST_ASSERT(!(in >> extra));
      }
   }
};

TEST_BEGIN(RadialShellVolumeTest)
TEST_ADD(RadialShellVolumeTest, testDensity)
TEST_ADD(RadialShellVolumeTest, testIntegral)
TEST_END(RadialShellVolumeTest)

#endif
