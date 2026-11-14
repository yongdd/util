#ifndef UTIL_MPI_DARRAY_ARCHIVE_TEST_H
#define UTIL_MPI_DARRAY_ARCHIVE_TEST_H
#include <util/param/DArrayParam.h>
#include <util/param/BracketPolicy.h>
#include <util/archives/BinaryFileIArchive.h>
#include <util/archives/BinaryFileOArchive.h>
#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>
#include <cstdio>
#include <fstream>
#include <sstream>

class MpiDArrayArchiveTest : public UnitTest
{
public:
   void setUp()
   {
      policy_ = Util::BracketPolicy::get();
      Util::BracketPolicy::set(Util::BracketPolicy::Required);
      Util::Label::clear();
   }
   void tearDown()
   {
      Util::BracketPolicy::set(policy_);
      Util::Label::clear();
   }
   void testFreshArchiveLoad()
   {
      printMethod(TEST_FUNC);
      checkLoad(false);
   }
   void testPreallocatedArchiveLoad()
   {
      printMethod(TEST_FUNC);
      checkLoad(true);
   }
private:
   Util::BracketPolicy::Type policy_;
   void checkLoad(bool preallocated)
   {
      const char* filename = "MpiDArrayArchiveTest.bin";
      if (mpiRank() == 0) {
         Util::DArray<int> original;
         original.allocate(2);
         Util::DArrayParam<int> source("values", original, 2);
         std::istringstream text("values[ 17 23 ]");
         source.readParam(text);
         Util::BinaryFileOArchive out(filename);
         source.save(out);
      }
      Util::DArray<int> restored;
      if (preallocated) {
         restored.allocate(2);
         restored[0] = -1; restored[1] = -1;
      }
      Util::DArrayParam<int> destination("values", restored, 2);
      destination.setIoCommunicator(communicator());
      {
         Util::BinaryFileIArchive in;
         if (mpiRank() == 0) {
            in.file().open(filename, std::ios::in | std::ios::binary);
         }
         destination.load(in);
      }
      if (mpiRank() == 0) { std::remove(filename); }
      TEST_ASSERT(restored.isAllocated() && restored.capacity() == 2);
      TEST_ASSERT(restored[0] == 17 && restored[1] == 23);
   }
};
TEST_BEGIN(MpiDArrayArchiveTest)
TEST_ADD(MpiDArrayArchiveTest, testFreshArchiveLoad)
TEST_ADD(MpiDArrayArchiveTest, testPreallocatedArchiveLoad)
TEST_END(MpiDArrayArchiveTest)
#endif
