#ifndef UTIL_TEXT_ARCHIVE_CHAR_TEST_H
#define UTIL_TEXT_ARCHIVE_CHAR_TEST_H
#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>
#include <util/archives/TextFileIArchive.h>
#include <util/archives/TextFileOArchive.h>
#include <cstdio>
#include <string>
#include <vector>
using namespace Util;

class TextArchiveCharTest : public UnitTest
{
public:
   void testScalarRecords()
   {
      printMethod(TEST_FUNC);
      const char* path = "TextArchiveChars.txt";
      for (char value : {'Q', ' ', '\t', '\n', '\r', '\0'}) {
         int first = 314, last = 2718;
         char copy = '?', next = '!';
         {
            TextFileOArchive out(path);
            out & first; out & value; out & next; out & last;
         }
         int a = 0, b = 0;
         char restoredNext = '?';
         {
            TextFileIArchive in(path);
            in & a; in & copy; in & restoredNext; in & b;
            TEST_ASSERT(!in.file().fail());
         }
         TEST_ASSERT(a == first && b == last);
         TEST_ASSERT(copy == value && restoredNext == next);
         // A character at the start is a nearby previously working case.
         { TextFileOArchive out(path); out & value; out & last; }
         { TextFileIArchive in(path); in & copy; in & b; }
         TEST_ASSERT(copy == value && b == last);
      }
      std::remove(path);
   }

   void testArraysAndStrings()
   {
      printMethod(TEST_FUNC);
      const char* path = "TextArchiveMixed.txt";
      int values[] = {1, 2, 3, 4};
      char marker = ' ';
      std::string strings[] = {"", "  two words\nnext line\t", "tail"};
      std::vector<char> chars = {'a', '\n', ' ', 'b'};
      {
         TextFileOArchive out(path);
         out.pack(values, 4);
         out & marker;
         out.pack(values, 2, 2, 2);
         out & marker;
         for (std::string& value : strings) { out & value; out & marker; }
         out & chars;
      }
      {
         TextFileIArchive in(path);
         int copy[4] = {};
         char c = '?';
         in.unpack(copy, 4); in & c;
         TEST_ASSERT(c == marker);
         for (int i=0; i<4; ++i) TEST_ASSERT(copy[i] == values[i]);
         in.unpack(copy, 2, 2, 2); in & c;
         TEST_ASSERT(c == marker);
         for (int i=0; i<4; ++i) TEST_ASSERT(copy[i] == values[i]);
         for (std::string& value : strings) {
            std::string restored;
            in & restored; in & c;
            TEST_ASSERT(restored == value && c == marker);
         }
         std::vector<char> restored;
         in & restored;
         TEST_ASSERT(restored == chars);
         TEST_ASSERT(!in.file().fail());
      }
      std::remove(path);
   }

   void testLegacyWireFormat()
   {
      printMethod(TEST_FUNC);
      const char* path = "TextArchiveLegacy.txt";
      // Exact bytes produced by the existing output archive.
      { std::ofstream out(path); out << "314\nQ\n2718\n"; }
      int first = 0, last = 0;
      char c = '?';
      { TextFileIArchive in(path); in & first; in & c; in & last; }
      TEST_ASSERT(first == 314 && c == 'Q' && last == 2718);
      std::remove(path);
   }
};
TEST_BEGIN(TextArchiveCharTest)
TEST_ADD(TextArchiveCharTest, testScalarRecords)
TEST_ADD(TextArchiveCharTest, testArraysAndStrings)
TEST_ADD(TextArchiveCharTest, testLegacyWireFormat)
TEST_END(TextArchiveCharTest)
#endif
