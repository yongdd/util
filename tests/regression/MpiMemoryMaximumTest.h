#ifndef UTIL_MPI_MEMORY_MAXIMUM_TEST_H
#define UTIL_MPI_MEMORY_MAXIMUM_TEST_H
#include <util/misc/Memory.h>
#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

class MpiMemoryMaximumTest : public UnitTest
{
public:
   void testUnequalRankMaxima()
   {
      printMethod(TEST_FUNC);
      const long before = Util::Memory::total();
      const long extra = Util::Memory::max() + 100 + 11*mpiRank();
      Util::Memory::add<char>(extra);
      const long local = Util::Memory::max();
      long expected = -1;
      communicator().Allreduce(&local, &expected, 1, MPI::LONG, MPI::MAX);
      const long actual = Util::Memory::max(communicator());
      Util::Memory::sub<char>(extra);
      TEST_ASSERT(actual == expected);
      TEST_ASSERT(actual >= local);
      TEST_ASSERT(Util::Memory::total() == before);
   }
};
TEST_BEGIN(MpiMemoryMaximumTest)
TEST_ADD(MpiMemoryMaximumTest, testUnequalRankMaxima)
TEST_END(MpiMemoryMaximumTest)
#endif
