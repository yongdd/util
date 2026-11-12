#ifndef CORE_DSYMM_MATRIX_PARAM_TEST_H
#define CORE_DSYMM_MATRIX_PARAM_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

#include <util/param/DSymmMatrixParam.h>
#include <util/param/BracketPolicy.h>
#include <util/param/Label.h>
#include <util/containers/DMatrix.h>
#include <util/misc/Exception.h>

#include <sstream>
#include <string>

using namespace Util;

/*
* Reading of a symmetric matrix parameter, such as chi, with n = 2.
* The row and column indices of each element were used without being
* checked: a 1-based entry such as 'chi( 2 1 20.0 )' left the matrix at
* zero and wrote past the arrays. Out-of-range or negative indices and
* entries that cannot be read must throw an Exception; valid input is
* still read. The message is checked as well, since the unchecked code
* can also stop, by chance, at the later check that an element is not
* given twice. Each test also reads valid input.
*/
class CoreDSymmMatrixParamTest : public UnitTest 
{

public:

   void setUp()
   {
      Label::clear();
      BracketPolicy::set(BracketPolicy::Optional);
   }

   void tearDown()
   {  Label::clear(); }

   void testReadIndexTooLarge()
   {
      printMethod(TEST_FUNC);
      DMatrix<double> m;
      m.allocate(2, 2);
      std::string message;

      // Valid input, all elements
      TEST_ASSERT(!readThrows("Chi(\n 0 0 1.5\n 1 0 2.0\n 1 1 -3.0\n)\n",
                              m));
      TEST_ASSERT(m(0,0) == 1.5);
      TEST_ASSERT(m(0,1) == 2.0);
      TEST_ASSERT(m(1,0) == 2.0);
      TEST_ASSERT(m(1,1) == -3.0);

      // 1-based indices
      TEST_ASSERT(readThrows("Chi(\n 2 1 20.0\n)\n", m, &message));
      TEST_ASSERT(message.find("out of range") != std::string::npos);
      TEST_ASSERT(readThrows("Chi(\n 1 2 20.0\n)\n", m, &message));
      TEST_ASSERT(message.find("out of range") != std::string::npos);
   }

   void testReadIndexNegative()
   {
      printMethod(TEST_FUNC);
      DMatrix<double> m;
      m.allocate(2, 2);
      std::string message;

      TEST_ASSERT(readThrows("Chi(\n -1 0 2.0\n)\n", m, &message));
      TEST_ASSERT(message.find("out of range") != std::string::npos);
      TEST_ASSERT(readThrows("Chi(\n 0 -1 2.0\n)\n", m, &message));
      TEST_ASSERT(message.find("out of range") != std::string::npos);

      // Valid input, elements not listed are zero
      TEST_ASSERT(!readThrows("Chi(\n 1 0 20.0\n)\n", m));
      TEST_ASSERT(m(0,0) == 0.0);
      TEST_ASSERT(m(0,1) == 20.0);
      TEST_ASSERT(m(1,0) == 20.0);
      TEST_ASSERT(m(1,1) == 0.0);
   }

   void testReadUnreadable()
   {
      printMethod(TEST_FUNC);
      DMatrix<double> m;
      m.allocate(2, 2);
      std::string message;

      TEST_ASSERT(readThrows("Chi(\n a 0 2.0\n)\n", m, &message));
      TEST_ASSERT(message.find("Error reading") != std::string::npos);
      TEST_ASSERT(readThrows("Chi(\n 0 x 2.0\n)\n", m, &message));
      TEST_ASSERT(message.find("Error reading") != std::string::npos);
      TEST_ASSERT(readThrows("Chi(\n 0 1 abc\n)\n", m, &message));
      TEST_ASSERT(message.find("Error reading") != std::string::npos);

      // Valid input without brackets, all n(n+1)/2 elements
      TEST_ASSERT(!readThrows("Chi  0 0 1.0  0 1 2.0  1 1 3.0\n", m));
      TEST_ASSERT(m(0,0) == 1.0);
      TEST_ASSERT(m(1,0) == 2.0);
      TEST_ASSERT(m(1,1) == 3.0);
   }

private:

   /*
   * Read the 2x2 symmetric matrix parameter "Chi" from text. Return
   * true if this throws an Exception, and copy its message to *message
   * if message is not null.
   */
   bool readThrows(std::string const & text, DMatrix<double>& m,
                   std::string* message = 0)
   {
      DSymmMatrixParam<double> param("Chi", m, 2);
      std::istringstream in(text);
      bool thrown = false;
      try {
         param.readParam(in);
      } catch (Exception& e) {
         thrown = true;
         if (message) *message = e.message();
      }
      Label::clear();
      return thrown;
   }

};

TEST_BEGIN(CoreDSymmMatrixParamTest)
TEST_ADD(CoreDSymmMatrixParamTest, testReadIndexTooLarge)
TEST_ADD(CoreDSymmMatrixParamTest, testReadIndexNegative)
TEST_ADD(CoreDSymmMatrixParamTest, testReadUnreadable)
TEST_END(CoreDSymmMatrixParamTest)

#endif
