#ifndef UTIL_TEXT_ARCHIVE_RAW_ARRAY_TEST_H
#define UTIL_TEXT_ARCHIVE_RAW_ARRAY_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>
#include <util/archives/TextFileIArchive.h>
#include <util/archives/TextFileOArchive.h>
#include <cstdio>
#include <iterator>
#include <string>
#include <vector>

class TextArchiveRawArrayTest : public UnitTest
{
public:
   void testRawArrays()
   {
      printMethod(TEST_FUNC);
      const char* path = "TextArchiveRawArray.txt";
      const char source[] = {'A', ' ', '\n', '\0', '\t', '\r', char(255), 'Z'};
      for (int n : {0, 1, 3, 8}) {
         for (bool prefix : {false, true}) {
            int first = 2718, last = 314159;
            {
               Util::TextFileOArchive out(path);
               if (prefix) out & first;
               out.pack(source, n);
               out & last;
            }
            // Independent byte oracle also preserves the legacy wire format.
            std::string expected = prefix ? "2718\n" : "";
            for (int i = 0; i < n; ++i) { expected += source[i]; expected += "  "; }
            expected += "\n314159\n";
            std::ifstream raw(path, std::ios::binary);
            std::string wire((std::istreambuf_iterator<char>(raw)),
                             std::istreambuf_iterator<char>());
            TEST_ASSERT(wire == expected);
            std::vector<char> copy(n + 2, '#');
            int a = 0, b = 0;
            {
               Util::TextFileIArchive in(path);
               if (prefix) in & a;
               in.unpack(copy.data(), n);
               in & b;
               TEST_ASSERT(!in.file().fail());
            }
            TEST_ASSERT(!prefix || a == first);
            TEST_ASSERT(b == last);
            for (int i = 0; i < n; ++i) TEST_ASSERT(copy[i] == source[i]);
            TEST_ASSERT(copy[n] == '#' && copy[n+1] == '#');
         }
      }
      std::remove(path);
   }

   void testLegacyRecordAndExactDestination()
   {
      printMethod(TEST_FUNC);
      const char* path = "TextArchiveRawLegacy.txt";
      { std::ofstream out(path); out << "A  B  C  \n314159\n"; }
      char copy[3] = {};
      int marker = 0;
      {
         Util::TextFileIArchive in(path);
         in.unpack(copy, 3);
         in & marker;
         TEST_ASSERT(!in.file().fail());
      }
      TEST_ASSERT(copy[0] == 'A' && copy[1] == 'B' && copy[2] == 'C');
      TEST_ASSERT(marker == 314159);
      std::remove(path);
   }

   void testStridedRows()
   {
      printMethod(TEST_FUNC);
      const char* path = "TextArchiveRawRows.txt";
      const char source[10] = {'A', ' ', '\n', 'x', 'x', '\0', '\t', 'Z', 'x', 'x'};
      char copy[12];
      for (char& c : copy) c = '#';
      int marker = 314159, restored = 0;
      {
         Util::TextFileOArchive out(path);
         out.pack(source, 2, 3, 5);
         out & marker;
      }
      {
         Util::TextFileIArchive in(path);
         in.unpack(copy, 2, 3, 6);
         in & restored;
         TEST_ASSERT(!in.file().fail());
      }
      for (int row = 0; row < 2; ++row) {
         for (int col = 0; col < 3; ++col) {
            TEST_ASSERT(copy[row*6+col] == source[row*5+col]);
         }
         for (int col = 3; col < 6; ++col) TEST_ASSERT(copy[row*6+col] == '#');
      }
      TEST_ASSERT(restored == marker);
      std::remove(path);
   }
};
TEST_BEGIN(TextArchiveRawArrayTest)
TEST_ADD(TextArchiveRawArrayTest, testRawArrays)
TEST_ADD(TextArchiveRawArrayTest, testLegacyRecordAndExactDestination)
TEST_ADD(TextArchiveRawArrayTest, testStridedRows)
TEST_END(TextArchiveRawArrayTest)
#endif
