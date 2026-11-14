#ifndef UTIL_REGRESSION_TEST_COMPOSITE_H
#define UTIL_REGRESSION_TEST_COMPOSITE_H

/*
* Regression tests for fixed bugs, kept apart from the unit tests.
*/

#include <test/CompositeTestRunner.h>

#include "CoreAverageTest.h"
#include "CoreDSymmMatrixParamTest.h"
#include "CoreAverageVarianceTest.h"
#include "CoreRationalTest.h"
#include "CoreAutoCorrTest.h"
#include "AutoCorrelationArchiveReuseTest.h"
#include "CoreDistributionTest.h"
#include "CoreRandomTest.h"
#include "CoreParamEndOfFileTest.h"
#include "CoreParamBlankTest.h"

#include "RadialArchiveRoundTripTest.h"

#include "RadialAssignmentOptionsTest.h"
#include "RadialShellVolumeTest.h"
#include "MemoryArchiveAlignmentTest.h"

#include "ArchiveStringLengthTest.h"

#include "TextArchiveCharTest.h"

#include "IntegerMeanSqDispTest.h"
#include "EnsembleResizeTest.h"

#include "TextArchiveRawArrayTest.h"

#include "CardinalBSplineConstantTest.h"

#include "RestartRootPathTest.h"

#include "GStackAssignmentTest.h"

#include "GStackClearTest.h"

#include "ListIteratorFrontTest.h"

#include "StreamLineEndTest.h"

#include "RationalSelfDivisionTest.h"

TEST_COMPOSITE_BEGIN(RegressionTestComposite)
TEST_COMPOSITE_ADD_UNIT(CoreAverageTest);
TEST_COMPOSITE_ADD_UNIT(CoreDSymmMatrixParamTest);
TEST_COMPOSITE_ADD_UNIT(CoreAverageVarianceTest);
TEST_COMPOSITE_ADD_UNIT(CoreRationalTest);
TEST_COMPOSITE_ADD_UNIT(CoreAutoCorrTest);
TEST_COMPOSITE_ADD_UNIT(AutoCorrelationArchiveReuseTest);
TEST_COMPOSITE_ADD_UNIT(CoreDistributionTest);
TEST_COMPOSITE_ADD_UNIT(CoreRandomTest);
TEST_COMPOSITE_ADD_UNIT(CoreParamEndOfFileTest);
TEST_COMPOSITE_ADD_UNIT(CoreParamBlankTest);
TEST_COMPOSITE_ADD_UNIT(RadialArchiveRoundTripTest);
TEST_COMPOSITE_ADD_UNIT(RadialAssignmentOptionsTest);
TEST_COMPOSITE_ADD_UNIT(RadialShellVolumeTest);
TEST_COMPOSITE_ADD_UNIT(MemoryArchiveAlignmentTest);
TEST_COMPOSITE_ADD_UNIT(ArchiveStringLengthTest);
TEST_COMPOSITE_ADD_UNIT(TextArchiveCharTest);
TEST_COMPOSITE_ADD_UNIT(IntegerMeanSqDispTest);
TEST_COMPOSITE_ADD_UNIT(EnsembleResizeTest);
TEST_COMPOSITE_ADD_UNIT(TextArchiveRawArrayTest);
TEST_COMPOSITE_ADD_UNIT(CardinalBSplineConstantTest);
TEST_COMPOSITE_ADD_UNIT(RestartRootPathTest);
TEST_COMPOSITE_ADD_UNIT(GStackAssignmentTest);
TEST_COMPOSITE_ADD_UNIT(GStackClearTest);
TEST_COMPOSITE_ADD_UNIT(ListIteratorFrontTest);
TEST_COMPOSITE_ADD_UNIT(StreamLineEndTest);
TEST_COMPOSITE_ADD_UNIT(RationalSelfDivisionTest);
TEST_COMPOSITE_END

#endif
