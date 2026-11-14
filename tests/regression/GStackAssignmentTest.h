#ifndef UTIL_G_STACK_ASSIGNMENT_TEST_H
#define UTIL_G_STACK_ASSIGNMENT_TEST_H

#include <util/containers/GStack.h>
#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

class GStackAssignmentTest : public UnitTest
{
public:
   void testPointerIdentityAndIndependence()
   {
      printMethod(TEST_FUNC);
      int a = 11, b = 22, old = 33;
      Util::GStack<int> source, target;
      source.reserve(2); target.reserve(4);
      source.push(a); source.push(b);
      target.push(old); target.push(old); target.push(old);
      target = source;
      TEST_ASSERT(target.size() == 2 && target.capacity() == 4);
      TEST_ASSERT(target.isValid() && source.isValid());
      TEST_ASSERT(&target.pop() == &b);
      TEST_ASSERT(&target.pop() == &a);
      TEST_ASSERT(source.size() == 2 && &source.peek() == &b);
      TEST_ASSERT(target.isValid());
   }

   void testEmptyAndSelfAssignment()
   {
      printMethod(TEST_FUNC);
      int a = 9;
      Util::GStack<int> empty, target;
      empty.reserve(1); target.reserve(2); target.push(a);
      target = target;
      TEST_ASSERT(target.size() == 1 && &target.peek() == &a);
      target = empty;
      TEST_ASSERT(target.size() == 0 && target.isValid());
      TEST_ASSERT(target.capacity() == 2);
   }
};
TEST_BEGIN(GStackAssignmentTest)
TEST_ADD(GStackAssignmentTest, testPointerIdentityAndIndependence)
TEST_ADD(GStackAssignmentTest, testEmptyAndSelfAssignment)
TEST_END(GStackAssignmentTest)
#endif
