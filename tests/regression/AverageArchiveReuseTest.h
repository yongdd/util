#ifndef UTIL_AVERAGE_ARCHIVE_REUSE_TEST_H
#define UTIL_AVERAGE_ARCHIVE_REUSE_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>
#include <util/accumulators/Average.h>
#include <util/archives/MemoryIArchive.h>
#include <util/archives/MemoryOArchive.h>
#include <util/archives/BinaryFileIArchive.h>
#include <util/archives/BinaryFileOArchive.h>
#include <cmath>
#include <cstdio>
#include <sstream>

using namespace Util;

/*
* Load an Average archive into an Average that already holds a long
* history. The archive replaces the whole state, so the result must equal
* a load into a new Average, including the hierarchical error analysis,
* and must stay equal when sampling continues. Without the fix, the
* stages of the old history stayed registered (and were never deleted),
* so output() printed their rows and blockingError() used them.
*/
class AverageArchiveReuseTest : public UnitTest
{
public:

   void testLoadParametersIntoUsed()
   {
      printMethod(TEST_FUNC);
      for (int count : {8, 100}) {
         Average source, reused, fresh;
         source.setNSamplePerBlock(4);
         addSamples(source, count);
         initializeHistory(reused);
         const char* path = "AverageArchiveReuse.bin";
         int marker = 31415;
         {
            BinaryFileOArchive archive(path);
            source.save(archive);
            archive & marker;
         }
         {
            BinaryFileIArchive archive(path);
            reused.loadParameters(archive);
            int restoredMarker = 0;
            archive & restoredMarker;
            TEST_ASSERT(restoredMarker == marker);
         }
         {
            BinaryFileIArchive archive(path);
            fresh.loadParameters(archive);
         }
         std::remove(path);
         assertSame(source, reused);
         assertSame(source, fresh);
         continueSampling(source, reused, fresh);
      }
   }

   void testSerializeIntoUsed()
   {
      printMethod(TEST_FUNC);
      Average source, reused;
      source.setNSamplePerBlock(4);
      addSamples(source, 100);
      initializeHistory(reused);
      MemoryOArchive saved;
      saved.allocate(65536);
      saved & source;
      // Repeat the load after the restored object has created new stages
      for (int repeat = 0; repeat < 2; ++repeat) {
         Average fresh;
         restore(saved, reused);
         restore(saved, fresh);
         assertSame(source, reused);
         assertSame(source, fresh);
         for (int i = 0; i < 200; ++i) {
            double x = value(i + 7*repeat);
            reused.sample(x);
            fresh.sample(x);
         }
         assertSame(fresh, reused);
      }
      reused.clear();
      TEST_ASSERT(reused.nSample() == 0);
   }

private:

   // Deterministic sampled values
   double value(int i) const
   {  return std::sin(0.37*double(i)) + 0.01*double(i); }

   void addSamples(Average& average, int count)
   {
      for (int i = 0; i < count; ++i) {
         average.sample(value(i));
      }
   }

   // Long history with many stages and a different block size
   void initializeHistory(Average& average)
   {
      average.setNSamplePerBlock(3);
      for (int i = 0; i < 4096; ++i) {
         average.sample(100.0 + double(i % 7));
      }
   }

   void restore(MemoryOArchive& saved, Average& average)
   {
      MemoryIArchive archive;
      archive = saved;
      archive & average;
      archive.release();
   }

   void assertSame(Average& expected, Average& actual)
   {
      TEST_ASSERT(actual.nSample() == expected.nSample());
      TEST_ASSERT(actual.nSamplePerBlock() == expected.nSamplePerBlock());
      TEST_ASSERT(actual.iBlock() == expected.iBlock());
      TEST_ASSERT(actual.average() == expected.average());
      TEST_ASSERT(actual.variance() == expected.variance());
      TEST_ASSERT(actual.blockingError() == expected.blockingError());
      std::ostringstream expectedOut, actualOut;
      expected.output(expectedOut);
      actual.output(actualOut);
      TEST_ASSERT(actualOut.str() == expectedOut.str());
   }

   void continueSampling(Average& source, Average& reused, Average& fresh)
   {
      for (int i = 0; i < 300; ++i) {
         double x = value(1000 + i);
         source.sample(x);
         reused.sample(x);
         fresh.sample(x);
         if (i % 50 == 49) {
            assertSame(source, reused);
            assertSame(source, fresh);
         }
      }
   }

};

TEST_BEGIN(AverageArchiveReuseTest)
TEST_ADD(AverageArchiveReuseTest, testLoadParametersIntoUsed)
TEST_ADD(AverageArchiveReuseTest, testSerializeIntoUsed)
TEST_END(AverageArchiveReuseTest)

#endif
