#ifndef UTIL_REGRESSION_TEST_COMPOSITE_H
#define UTIL_REGRESSION_TEST_COMPOSITE_H

/*
* Regression tests for fixed bugs, kept apart from the unit tests.
*/

#include <test/CompositeTestRunner.h>

#include "CoreAverageTest.h"
#include "CoreDSymmMatrixParamTest.h"
#include "CoreAverageVarianceTest.h"

TEST_COMPOSITE_BEGIN(RegressionTestComposite)
TEST_COMPOSITE_ADD_UNIT(CoreAverageTest);
TEST_COMPOSITE_ADD_UNIT(CoreDSymmMatrixParamTest);
TEST_COMPOSITE_ADD_UNIT(CoreAverageVarianceTest);
TEST_COMPOSITE_END

#endif
