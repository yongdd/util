#ifndef UTIL_G_STACK_CLEAR_TEST_H
#define UTIL_G_STACK_CLEAR_TEST_H
#include <util/containers/GStack.h>
#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

class GStackClearTest : public UnitTest
{
public:
   void testPopulatedClearAndRefill()
   {
      printMethod(TEST_FUNC);
      int a = 1, b = 2, c = 3;
      Util::GStack<int> stack;
      stack.reserve(2); stack.push(a); stack.push(b);
      stack.clear();
      TEST_ASSERT(stack.size() == 0 && stack.capacity() == 2);
      TEST_ASSERT(stack.isAllocated() && stack.isValid());
      TEST_ASSERT(a == 1 && b == 2);
      stack.push(c);
      TEST_ASSERT(stack.isValid() && &stack.peek() == &c);
      TEST_ASSERT(&stack.pop() == &c && stack.isValid());
      stack.clear();
      TEST_ASSERT(stack.isValid());
   }
};
TEST_BEGIN(GStackClearTest)
TEST_ADD(GStackClearTest, testPopulatedClearAndRefill)
TEST_END(GStackClearTest)
#endif
