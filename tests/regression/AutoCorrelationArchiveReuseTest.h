#ifndef UTIL_AUTO_CORRELATION_ARCHIVE_REUSE_TEST_H
#define UTIL_AUTO_CORRELATION_ARCHIVE_REUSE_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>
#include <util/accumulators/AutoCorrelation.tpp>
#include <util/archives/MemoryIArchive.h>
#include <util/archives/MemoryOArchive.h>
#include <util/archives/BinaryFileIArchive.h>
#include <util/archives/BinaryFileOArchive.h>
#include <cstdio>
#include <sstream>
#include <string>

using namespace Util;

class AutoCorrelationArchiveReuseTest : public UnitTest
{
public:
   void testMemoryShortRestore()
   {
      printMethod(TEST_FUNC);
      for (int count = 0; count <= 1; ++count) {
         Correlation source, reused, fresh;
         source.setParam(8, 10, 2);
         addSamples(source, count);
         initializeHistory(reused);
         MemoryOArchive archive;
         archive.allocate(65536);
         archive & source;
         restore(archive, reused);
         restore(archive, fresh);
         assertSame(source, reused);
         assertSame(source, fresh);
         TEST_ASSERT(reused.maxDelay() == count - 1);
         continueSampling(source, reused, fresh);
      }
   }

   void testMemoryChildRestore()
   {
      printMethod(TEST_FUNC);
      Correlation source, reused;
      source.setParam(8, 10, 2);
      addSamples(source, 63);
      initializeHistory(reused);
      MemoryOArchive archive;
      archive.allocate(65536);
      archive & source;
      // Repeat replacement after the restored object creates new children.
      for (int repeat = 0; repeat < 2; ++repeat) {
         Correlation fresh;
         restore(archive, reused);
         restore(archive, fresh);
         assertSame(source, reused);
         assertSame(source, fresh);
         for (int i = 0; i < 64; ++i) {
            double value = double(i % 7) - 2.0;
            reused.sample(value);
            fresh.sample(value);
            assertSame(fresh, reused);
         }
      }
      reused.clear();
      TEST_ASSERT(reused.nSample() == 0);
      TEST_ASSERT(reused.maxDelay() == -1);
   }

   void testBinarySaveLoad()
   {
      printMethod(TEST_FUNC);
      Correlation source, reused, fresh;
      source.setParam(8, 10, 2);
      source.sample(3.0);
      initializeHistory(reused);
      const char* path = "AutoCorrelationArchiveReuse.bin";
      int marker = 31415;
      {
         BinaryFileOArchive archive(path);
         source.save(archive);
         archive & marker;
      }
      {
         BinaryFileIArchive archive(path);
         reused.load(archive);
         int restoredMarker = 0;
         archive & restoredMarker;
         TEST_ASSERT(restoredMarker == marker);
      }
      {
         BinaryFileIArchive archive(path);
         fresh.load(archive);
      }
      std::remove(path);
      assertSame(source, reused);
      assertSame(source, fresh);
      continueSampling(source, reused, fresh);
   }

private:
   typedef AutoCorrelation<double, double> Correlation;

   void addSamples(Correlation& accumulator, int count)
   {
      for (int i = 0; i < count; ++i) {
         accumulator.sample(double(i % 11) + 0.5);
      }
   }

   void initializeHistory(Correlation& accumulator)
   {
      accumulator.setParam(8, 10, 2);
      addSamples(accumulator, 4096);
      TEST_ASSERT(accumulator.maxDelay() > 1000);
   }

   void restore(MemoryOArchive& saved, Correlation& accumulator)
   {
      MemoryIArchive archive;
      archive = saved;
      archive & accumulator;
      archive.release();
   }

   void assertSame(Correlation& expected, Correlation& actual)
   {
      TEST_ASSERT(actual.nSample() == expected.nSample());
      TEST_ASSERT(actual.maxDelay() == expected.maxDelay());
      std::ostringstream expectedOut, actualOut;
      expected.output(expectedOut);
      actual.output(actualOut);
      TEST_ASSERT(actualOut.str() == expectedOut.str());
   }

   void continueSampling(Correlation& source, Correlation& reused,
                         Correlation& fresh)
   {
      for (int i = 0; i < 64; ++i) {
         double value = double(i % 7) - 2.0;
         source.sample(value);
         reused.sample(value);
         fresh.sample(value);
         assertSame(source, reused);
         assertSame(source, fresh);
      }
   }
};

TEST_BEGIN(AutoCorrelationArchiveReuseTest)
TEST_ADD(AutoCorrelationArchiveReuseTest, testMemoryShortRestore)
TEST_ADD(AutoCorrelationArchiveReuseTest, testMemoryChildRestore)
TEST_ADD(AutoCorrelationArchiveReuseTest, testBinarySaveLoad)
TEST_END(AutoCorrelationArchiveReuseTest)

#endif
