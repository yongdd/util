#ifndef UTIL_MPI_PARAM_WRAPPER_TEST_H
#define UTIL_MPI_PARAM_WRAPPER_TEST_H
#include <util/param/ParamComposite.h>
#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>
#include <sstream>

class MpiParamWrapperTest : public UnitTest
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
   void testCArray()
   {
      printMethod(TEST_FUNC);
      int values[2] = {-1,-1};
      Util::CArrayParam<int> param("values", values, 2);
      read(param, "values[ 17 23 ]");
      TEST_ASSERT(values[0] == 17 && values[1] == 23);
   }
   void testDArray()
   {
      printMethod(TEST_FUNC);
      Util::DArray<int> values; values.allocate(2);
      values[0] = -1; values[1] = -1;
      Util::DArrayParam<int> param("values", values, 2);
      read(param, "values[ 17 23 ]");
      TEST_ASSERT(values[0] == 17 && values[1] == 23);
   }
   void testFArray()
   {
      printMethod(TEST_FUNC);
      Util::FArray<int,2> values;
      values[0] = -1; values[1] = -1;
      Util::FArrayParam<int,2> param("values", values);
      read(param, "values[ 17 23 ]");
      TEST_ASSERT(values[0] == 17 && values[1] == 23);
   }
   void testFSArrayLogicalSize()
   {
      printMethod(TEST_FUNC);
      Util::FSArray<int,4> values;
      values.append(-1); values.append(-1); values.append(-1);
      Util::FSArrayParam<int,4> param("values", values, 2);
      read(param, "values[ 17 23 ]");
      TEST_ASSERT(values.size() == 2);
      TEST_ASSERT(values[0] == 17 && values[1] == 23);
   }
   void testFSArrayEmpty()
   {
      printMethod(TEST_FUNC);
      Util::FSArray<int,4> values;
      values.append(-1);
      Util::FSArrayParam<int,4> param("values", values, 0);
      read(param, "values[ ]");
      TEST_ASSERT(values.size() == 0);
   }
   void testCArray2DStride()
   {
      printMethod(TEST_FUNC);
      int values[6] = {-1,-1,-1,-1,-1,-1};
      Util::CArray2DParam<int> param("values", values, 2, 2, 3);
      read(param, "values[ 1 2 3 4 ]");
      TEST_ASSERT(values[0] == 1 && values[1] == 2);
      TEST_ASSERT(values[3] == 3 && values[4] == 4);
   }
   void testDMatrix()
   {
      printMethod(TEST_FUNC);
      Util::DMatrix<int> values; values.allocate(2,2);
      Util::DMatrixParam<int> param("values", values, 2, 2);
      read(param, "values[ 1 2 3 4 ]");
      TEST_ASSERT(values(0,0) == 1 && values(0,1) == 2);
      TEST_ASSERT(values(1,0) == 3 && values(1,1) == 4);
   }
   void testSymmetricMatrix()
   {
      printMethod(TEST_FUNC);
      Util::DMatrix<double> values; values.allocate(2,2);
      Util::DSymmMatrixParam<double> param("values", values, 2);
      read(param, "values( 0 1 7.5 )");
      TEST_ASSERT(values(0,0) == 0 && values(1,1) == 0);
      TEST_ASSERT(values(0,1) == 7.5 && values(1,0) == 7.5);
   }
private:
   Util::BracketPolicy::Type policy_;
   void read(Util::Parameter& param, const char* text)
   {
      param.setIoCommunicator(communicator());
      std::istringstream in(mpiRank() == 0 ? text : "");
      param.readParam(in);
   }
};
TEST_BEGIN(MpiParamWrapperTest)
TEST_ADD(MpiParamWrapperTest, testCArray)
TEST_ADD(MpiParamWrapperTest, testDArray)
TEST_ADD(MpiParamWrapperTest, testFArray)
TEST_ADD(MpiParamWrapperTest, testFSArrayLogicalSize)
TEST_ADD(MpiParamWrapperTest, testFSArrayEmpty)
TEST_ADD(MpiParamWrapperTest, testCArray2DStride)
TEST_ADD(MpiParamWrapperTest, testDMatrix)
TEST_ADD(MpiParamWrapperTest, testSymmetricMatrix)
TEST_END(MpiParamWrapperTest)
#endif
