#ifndef UTIL_VECTOR_MEMORY_COUNT_TEST_H
#define UTIL_VECTOR_MEMORY_COUNT_TEST_H
#include <util/archives/MemoryCounter.h>
#include <util/archives/MemoryOArchive.h>
#include <util/archives/MemoryIArchive.h>
#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>
#include <vector>
#include <string>

class VectorMemoryCountTest : public UnitTest
{
public:
   void testIntegerVectors()
   {
      printMethod(TEST_FUNC);
      for (auto values : {std::vector<int>(), std::vector<int>{17,-23}}) {
         const size_t bytes = sizeof(size_t) + values.size()*sizeof(int);
         TEST_ASSERT(size_t(Util::memorySize(values)) == bytes);
         checkRoundTrip(values, bytes);
      }
   }
   void testNestedStrings()
   {
      printMethod(TEST_FUNC);
      std::vector<std::vector<std::string> > values =
         {{"alpha", ""}, {std::string("a\0b",3)}};
      const size_t bytes = 6*sizeof(size_t) + 6 + 1 + 4;
      TEST_ASSERT(size_t(Util::memorySize(values)) == bytes);
      checkRoundTrip(values, bytes);
   }
private:
   template <typename T>
   void checkRoundTrip(std::vector<T>& values, size_t bytes)
   {
      Util::MemoryCounter counter;
      int marker = 314159;
      counter & values & marker;
      TEST_ASSERT(counter.size() == bytes + sizeof(int));
      Util::MemoryOArchive out;
      out.allocate(counter.size());
      out & values & marker;
      TEST_ASSERT(size_t(out.cursor() - out.begin()) == counter.size());
      Util::MemoryIArchive in;
      in = out;
      std::vector<T> restored;
      int last = 0;
      in & restored & last;
      TEST_ASSERT(restored == values && last == marker);
      TEST_ASSERT(in.cursor() == in.end());
      in.release();
      counter.clear();
      counter << values;
      TEST_ASSERT(counter.size() == bytes);
   }
};
TEST_BEGIN(VectorMemoryCountTest)
TEST_ADD(VectorMemoryCountTest, testIntegerVectors)
TEST_ADD(VectorMemoryCountTest, testNestedStrings)
TEST_END(VectorMemoryCountTest)
#endif
