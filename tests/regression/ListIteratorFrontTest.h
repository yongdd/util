#ifndef UTIL_LIST_ITERATOR_FRONT_TEST_H
#define UTIL_LIST_ITERATOR_FRONT_TEST_H
#include <util/containers/List.h>
#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

class ListIteratorFrontTest : public UnitTest
{
public:
   void testForwardAndReverseEndpoints()
   {
      printMethod(TEST_FUNC);
      Util::Node<int> nodes[3];
      Util::List<int> list;
      list.initialize(nodes, 3);
      for (int i = 0; i < 3; ++i) {
         nodes[i].data() = i + 1; list.pushBack(nodes[i]);
      }
      Util::ListIterator<int> it(list);
      TEST_ASSERT(it.isFront() && !it.isBack() && *it == 1);
      ++it; TEST_ASSERT(!it.isFront() && !it.isBack() && *it == 2);
      ++it; TEST_ASSERT(!it.isFront() && it.isBack() && *it == 3);
      --it; --it;
      TEST_ASSERT(it.isFront() && *it == 1);
      --it; TEST_ASSERT(it.isEnd());
      TEST_ASSERT(list.isValid());
   }

   void testSingleton()
   {
      printMethod(TEST_FUNC);
      Util::Node<int> node;
      Util::List<int> list;
      list.initialize(&node, 1); list.pushBack(node);
      Util::ListIterator<int> it(list);
      TEST_ASSERT(it.isFront() && it.isBack());
   }
};
TEST_BEGIN(ListIteratorFrontTest)
TEST_ADD(ListIteratorFrontTest, testForwardAndReverseEndpoints)
TEST_ADD(ListIteratorFrontTest, testSingleton)
TEST_END(ListIteratorFrontTest)
#endif
