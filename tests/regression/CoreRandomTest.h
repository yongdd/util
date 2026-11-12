#ifndef CORE_RANDOM_TEST_H
#define CORE_RANDOM_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

#include <util/random/Random.h>

using namespace Util;

/*
* Random::gaussian() kept the second deviate of each pair in
* function-static variables, shared by all Random objects and kept
* across setSeed(): the same seed did not give the same sequence after
* an odd number of draws by another object, or after a reseed.
*/
class CoreRandomTest : public UnitTest 
{

public:

   void setUp()
   {}

   void tearDown()
   {}

   // Two objects with the same seed give the same sequence
   void testGaussianSameSeed()
   {
      printMethod(TEST_FUNC);

      const int n = 7;
      double x[n];
      Random a;
      a.setSeed(42);
      for (int i = 0; i < n; ++i) {
         x[i] = a.gaussian();
      }

      // b draws interleaved with an odd number of draws by c
      Random b;
      Random c;
      b.setSeed(42);
      c.setSeed(7);
      for (int i = 0; i < n; ++i) {
         TEST_ASSERT(b.gaussian() == x[i]);
         c.gaussian();
      }
   }

   // setSeed() restarts the sequence, also after an odd number of draws
   void testGaussianReseed()
   {
      printMethod(TEST_FUNC);

      const int n = 5;
      double x[n];
      Random a;
      a.setSeed(42);
      for (int i = 0; i < n; ++i) {
         x[i] = a.gaussian();
      }
      a.setSeed(42);
      for (int i = 0; i < n; ++i) {
         TEST_ASSERT(a.gaussian() == x[i]);
      }
   }

};

TEST_BEGIN(CoreRandomTest)
TEST_ADD(CoreRandomTest, testGaussianSameSeed)
TEST_ADD(CoreRandomTest, testGaussianReseed)
TEST_END(CoreRandomTest)

#endif
