#ifndef UTIL_MEMORY_ARCHIVE_ALIGNMENT_TEST_H
#define UTIL_MEMORY_ARCHIVE_ALIGNMENT_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>
#include <util/archives/MemoryOArchive.h>
#include <util/archives/MemoryIArchive.h>
#include <cstring>
#include <vector>

class MemoryArchiveAlignmentTest : public UnitTest
{
public:
   void testPackedScalarsAndArrays()
   {
      printMethod(TEST_FUNC);
      for (int offset = 0; offset < 8; ++offset) {
         Util::MemoryOArchive out;
         out.allocate(256);
         char prefix = 'x';
         int id = 3, marker = 314159;
         double value = 1.25, array[] = {2.5, -3.0, .125};
         double matrix[2][3] = {{4.5, -5.0, 99.0}, {.25, 6.0, 99.0}};
         std::vector<unsigned char> expected;
         for (int j = 0; j < offset; ++j) {
            out & prefix;
            appendBytes(expected, prefix);
         }
         out & id & value;
         out.pack(array, 3);
         out.pack(matrix[0], 2, 2, 3);
         out & marker;
         appendBytes(expected, id);
         appendBytes(expected, value);
         for (int i = 0; i < 3; ++i) appendBytes(expected, array[i]);
         for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) appendBytes(expected, matrix[i][j]);
         }
         appendBytes(expected, marker);
         TEST_ASSERT(out.cursor() == out.begin() + expected.size());
         TEST_ASSERT(std::memcmp(out.begin(), expected.data(), expected.size()) == 0);

         Util::MemoryIArchive in;
         in = out;
         for (int j = 0; j < offset; ++j) {
            char c = 0;
            in & c;
            TEST_ASSERT(c == prefix);
         }
         int restoredId = 0, restoredMarker = 0;
         double restored = 0, restoredArray[3] = {};
         double restoredMatrix[2][4] = {{99,99,99,99}, {99,99,99,99}};
         in & restoredId & restored;
         in.unpack(restoredArray, 3);
         in.unpack(restoredMatrix[0], 2, 2, 4);
         in & restoredMarker;
         TEST_ASSERT(in.cursor() == in.begin() + expected.size());
         in.release();
         TEST_ASSERT(restoredId == id && restored == value && restoredMarker == marker);
         for (int i = 0; i < 3; ++i) TEST_ASSERT(restoredArray[i] == array[i]);
         for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) TEST_ASSERT(restoredMatrix[i][j] == matrix[i][j]);
            TEST_ASSERT(restoredMatrix[i][2] == 99 && restoredMatrix[i][3] == 99);
         }
      }
   }

private:
   template <class T>
   void appendBytes(std::vector<unsigned char>& bytes, const T& value)
   {
      const unsigned char* begin = reinterpret_cast<const unsigned char*>(&value);
      bytes.insert(bytes.end(), begin, begin + sizeof(T));
   }
};

TEST_BEGIN(MemoryArchiveAlignmentTest)
TEST_ADD(MemoryArchiveAlignmentTest, testPackedScalarsAndArrays)
TEST_END(MemoryArchiveAlignmentTest)

#endif
