#ifndef HUNT17_PERSISTENCE_TEST_H_ARCHIVE_STRING_LENGTH_TEST_H
#define HUNT17_PERSISTENCE_TEST_H_ARCHIVE_STRING_LENGTH_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>
#include <util/accumulators/Distribution.h>
#include <util/accumulators/RadialDistribution.h>
#include <util/archives/BinaryFileIArchive.h>
#include <util/archives/BinaryFileOArchive.h>
#include <util/archives/MemoryIArchive.h>
#include <util/archives/MemoryOArchive.h>
#include <cstdio>
#include <sstream>
#include <string>

using namespace Util;

class ArchiveStringLengthTest : public UnitTest
{
public:
   void setUp() {}
   void tearDown() {}

   void testBinaryStrings()
   {
      printMethod(TEST_FUNC);
      const char* path = "ArchiveStringLengthStrings.bin";
      for (int i = 0; i < 5; ++i) {
         std::string source = value(i), restored = "previous data";
         int marker = 1234567, actual = 0;
         { BinaryFileOArchive ar(path); ar & source; ar & marker; }
         { BinaryFileIArchive ar(path); ar & restored; ar & actual; }
         TEST_ASSERT(restored == source);
         TEST_ASSERT(restored.size() == source.size());
         TEST_ASSERT(actual == marker);
      }
      std::remove(path);
   }

   void testMemoryStrings()
   {
      printMethod(TEST_FUNC);
      for (int i = 0; i < 5; ++i) {
         std::string source = value(i), restored = "previous data";
         int marker = 1234567, actual = 0;
         MemoryOArchive out; out.allocate(256);
         out & source; out & marker;
         MemoryIArchive in; in = out;
         in & restored; in & actual; in.release();
         TEST_ASSERT(restored == source);
         TEST_ASSERT(restored.size() == source.size());
         TEST_ASSERT(actual == marker);
      }
   }

private:

   std::string value(int i)
   {
      if (i == 0) return "ordinary";
      if (i == 1) return std::string();
      if (i == 2) return std::string("a\0b", 3);
      if (i == 3) return std::string("\0ab", 3);
      return std::string("ab\0", 3);
   }
};

TEST_BEGIN(ArchiveStringLengthTest)
TEST_ADD(ArchiveStringLengthTest, testBinaryStrings)
TEST_ADD(ArchiveStringLengthTest, testMemoryStrings)
TEST_END(ArchiveStringLengthTest)

#endif
