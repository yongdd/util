#ifndef UTIL_STREAM_LINE_END_TEST_H
#define UTIL_STREAM_LINE_END_TEST_H
#include <util/misc/ioUtil.h>
#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>
#include <vector>
#include <sstream>

class StreamLineEndTest : public UnitTest
{
public:
   void testLineSequences()
   {
      printMethod(TEST_FUNC);
      const char* cases[] = {"", "alpha", "alpha\n", "\n", "alpha\nbeta",
                             "alpha\nbeta\n", "\n\n", "alpha \t\n"};
      for (const char* input : cases) {
         std::istringstream reference(input), actual(input);
         std::vector<std::string> expected, result;
         std::string text;
         while (std::getline(reference, text)) expected.push_back(text);
         std::stringstream line;
         while (Util::getLine(actual, line)) result.push_back(line.str());
         TEST_ASSERT(result == expected);
         TEST_ASSERT(line.str().empty());
         TEST_ASSERT(!Util::getLine(actual, line));
      }
   }

   void testReuseConsumedOutputStream()
   {
      printMethod(TEST_FUNC);
      std::istringstream input("17\n23");
      std::stringstream line;
      int value = 0;
      TEST_ASSERT(Util::getLine(input, line)); line >> value;
      TEST_ASSERT(value == 17);
      TEST_ASSERT(Util::getLine(input, line)); line >> value;
      TEST_ASSERT(value == 23);
      TEST_ASSERT(!Util::getLine(input, line));
   }
};
TEST_BEGIN(StreamLineEndTest)
TEST_ADD(StreamLineEndTest, testLineSequences)
TEST_ADD(StreamLineEndTest, testReuseConsumedOutputStream)
TEST_END(StreamLineEndTest)
#endif
