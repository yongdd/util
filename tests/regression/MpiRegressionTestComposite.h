#ifndef UTIL_MPI_REGRESSION_TEST_COMPOSITE_H
#define UTIL_MPI_REGRESSION_TEST_COMPOSITE_H
#include <test/CompositeTestRunner.h>
#include "MpiParamWrapperTest.h"
TEST_COMPOSITE_BEGIN(MpiRegressionTestComposite)
TEST_COMPOSITE_ADD_UNIT(MpiParamWrapperTest);
TEST_COMPOSITE_END
#endif
