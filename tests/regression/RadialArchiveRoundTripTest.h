#ifndef HUNT17_PERSISTENCE_TEST_H_RADIAL_ARCHIVE_ROUND_TRIP_TEST_H
#define HUNT17_PERSISTENCE_TEST_H_RADIAL_ARCHIVE_ROUND_TRIP_TEST_H

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

class RadialArchiveRoundTripTest : public UnitTest
{
public:
   void setUp() {}
   void tearDown() {}

   void testRadialSaveLoad()
   {
      printMethod(TEST_FUNC);
      RadialDistribution source;
      initialize(source, true);
      const char* path = "RadialArchiveRoundTripRadial.bin";
      { BinaryFileOArchive ar(path); source.save(ar); }
      RadialDistribution restored, serialized;
      { BinaryFileIArchive ar(path); restored.load(ar); }
      { BinaryFileIArchive ar(path); ar & serialized; }
      std::remove(path);
      TEST_ASSERT(render(restored) == render(source));
      TEST_ASSERT(render(serialized) == render(source));
      // Continuing the accumulator checks the saved counters and normalization.
      source.beginSnapshot(); restored.beginSnapshot();
      source.sample(.3); restored.sample(.3);
      TEST_ASSERT(render(restored) == render(source));

      Distribution base, baseRestored;
      base.setParam(0., 1., 4); base.sample(.1); base.sample(.7);
      { BinaryFileOArchive ar(path); base.save(ar); }
      { BinaryFileIArchive ar(path); baseRestored.load(ar); }
      std::remove(path);
      TEST_ASSERT(render(baseRestored) == render(base));
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

TEST_BEGIN(RadialArchiveRoundTripTest)
TEST_ADD(RadialArchiveRoundTripTest, testRadialSaveLoad)
TEST_END(RadialArchiveRoundTripTest)

#endif
