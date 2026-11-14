#ifndef UTIL_POLYNOMIAL_EMPTY_COPY_TEST_H
#define UTIL_POLYNOMIAL_EMPTY_COPY_TEST_H
#include <util/math/Polynomial.h>
#include <util/containers/DArray.h>
#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

class PolynomialEmptyCopyTest : public UnitTest
{
public:
   template <class T> void checkEmpty()
   {
      Util::DArray<T> coefficients;
      Util::Polynomial<T> empty(coefficients), standard;
      Util::Polynomial<T> copy(empty), ordinary(standard);
      Util::Polynomial<T> reflected = empty.reflect();
      Util::Polynomial<T> shifted = empty.shift(T(2));
      Util::Polynomial<T> integral = empty.integrate();
      Util::Polynomial<T> derivative = empty.differentiate();
      Util::Polynomial<T>* values[] = {&empty, &copy, &ordinary, &reflected,
                                      &shifted, &integral, &derivative};
      for (unsigned i = 0; i < sizeof(values)/sizeof(values[0]); ++i) {
         TEST_ASSERT(values[i]->size() == 0);
         TEST_ASSERT(values[i]->degree() == -1);
         TEST_ASSERT((*values[i])(T(-2)) == T(0));
         TEST_ASSERT(values[i]->evaluate(3.0) == 0.0);
      }
      copy += T(3);
      TEST_ASSERT(copy(T(2)) == T(3));
      TEST_ASSERT(empty.size() == 0 && empty(T(2)) == T(0));
      Util::DArray<T> nonempty(2);
      nonempty[0] = T(2); nonempty[1] = T(3);
      Util::Polynomial<T> linear(nonempty), linearCopy(linear);
      TEST_ASSERT(linearCopy(T(2)) == T(8));
      linearCopy[0] = T(5);
      TEST_ASSERT(linear(T(2)) == T(8));
   }
   void testDouble() { printMethod(TEST_FUNC); checkEmpty<double>(); }
   void testRational() { printMethod(TEST_FUNC); checkEmpty<Util::Rational>(); }
};
TEST_BEGIN(PolynomialEmptyCopyTest)
TEST_ADD(PolynomialEmptyCopyTest, testDouble)
TEST_ADD(PolynomialEmptyCopyTest, testRational)
TEST_END(PolynomialEmptyCopyTest)
#endif
