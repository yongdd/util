#ifndef HUNT17_PERSISTENCE_TEST_H_RADIAL_ASSIGNMENT_OPTIONS_TEST_H
#define HUNT17_PERSISTENCE_TEST_H_RADIAL_ASSIGNMENT_OPTIONS_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>
#include <util/accumulators/Distribution.h>
#include <util/accumulators/RadialDistribution.h>
#include <util/archives/BinaryFileIArchive.h>
#include <util/archives/BinaryFileOArchive.h>
#include <util/archives/MemoryIArchive.h>
#include <util/archives/MemoryOArchive.h>
#include <cstdio>
#include <sstream>
#include <string>

using namespace Util;

class RadialAssignmentOptionsTest : public UnitTest
{
public:
   void setUp() {}
   void tearDown() {}

   void testRadialAssignment()
   {
      printMethod(TEST_FUNC);
      for (int enabled = 0; enabled < 2; ++enabled) {
         RadialDistribution source, destination, fresh;
         initialize(source, enabled != 0);
         initialize(destination, enabled == 0);
         RadialDistribution copy(source);
         destination = source;
         fresh = source;
         source = source;
         TEST_ASSERT(render(copy) == render(source));
         TEST_ASSERT(render(fresh) == render(source));
         TEST_ASSERT(render(destination) == render(source));
         source.beginSnapshot(); destination.beginSnapshot();
         source.sample(.2); destination.sample(.2);
         TEST_ASSERT(render(destination) == render(source));
      }
   }

private:
   void initialize(RadialDistribution& d, bool integral)
   {
      d.setParam(1., 4); d.setNorm(2.); d.setOutputIntegral(integral);
      d.beginSnapshot(); d.sample(.1); d.sample(.4);
      d.beginSnapshot(); d.sample(.7); d.sample(.9);
   }

   template<class T> std::string render(T& d)
   { std::ostringstream out; d.output(out); return out.str(); }

};

TEST_BEGIN(RadialAssignmentOptionsTest)
TEST_ADD(RadialAssignmentOptionsTest, testRadialAssignment)
TEST_END(RadialAssignmentOptionsTest)

#endif
