#ifndef UTIL_ENSEMBLE_RESIZE_TEST_H
#define UTIL_ENSEMBLE_RESIZE_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>
#include <util/accumulators/AutoCorrArray.h>
#include <util/accumulators/MeanSqDispArray.h>
#include <util/containers/DArray.h>
#include <sstream>
#include <string>

using namespace Util;

class EnsembleResizeTest : public UnitTest
{
public:
   void testCorrelationResize()
   {
      printMethod(TEST_FUNC);
      DArray<double> values;
      values.allocate(2);
      for (int initial = 1; initial <= 2; ++initial) {
         AutoCorrArray<double, double> reused, fresh;
         reused.setParam(2, 8);
         reused.setNEnsemble(initial);
         values[0] = values[1] = 10.0;
         reused.sample(values);
         reused.sample(values);
         // Shrinking used to renormalize old sums; growing used to access
         // shorter histories in the newly activated sequences.
         reused.setNEnsemble(3 - initial);
         TEST_ASSERT(reused.nSample() == 0);
         TEST_ASSERT(reused.nEnsemble() == 3 - initial);
         fresh.setParam(2, 8);
         fresh.setNEnsemble(3 - initial);
         values[0] = values[1] = 1.0;
         for (int i = 0; i < 3; ++i) {
            reused.sample(values);
            fresh.sample(values);
         }
         TEST_ASSERT(reused.nSample() == 3);
         TEST_ASSERT(reused.average() == 1.0);
         TEST_ASSERT(render(reused) == render(fresh));
         TEST_ASSERT(lag(reused, 0) == 1.0);
         TEST_ASSERT(lag(reused, 1) == 1.0);
      }
   }

   void testCorrelationSameSize()
   {
      printMethod(TEST_FUNC);
      AutoCorrArray<double, double> accumulator;
      accumulator.setParam(2, 8);
      DArray<double> values;
      values.allocate(2);
      values[0] = values[1] = 1.0;
      accumulator.sample(values);
      accumulator.setNEnsemble(2);
      TEST_ASSERT(accumulator.nSample() == 1);
      values[0] = values[1] = 3.0;
      accumulator.sample(values);
      TEST_ASSERT(accumulator.average() == 2.0);
      TEST_ASSERT(lag(accumulator, 0) == 5.0);
      TEST_ASSERT(lag(accumulator, 1) == 3.0);
   }

   void testDisplacementResize()
   {
      printMethod(TEST_FUNC);
      DArray<double> values;
      values.allocate(2);
      for (int initial = 1; initial <= 2; ++initial) {
         MeanSqDispArray<double> reused, fresh;
         reused.setParam(2, 8);
         reused.setNEnsemble(initial);
         values[0] = values[1] = 0.0;
         reused.sample(values);
         values[0] = values[1] = 10.0;
         reused.sample(values);
         reused.setNEnsemble(3 - initial);
         TEST_ASSERT(reused.nSample() == 0);
         TEST_ASSERT(reused.nEnsemble() == 3 - initial);
         fresh.setParam(2, 8);
         fresh.setNEnsemble(3 - initial);
         for (int i = 0; i < 3; ++i) {
            values[0] = values[1] = double(i);
            reused.sample(values);
            fresh.sample(values);
         }
         TEST_ASSERT(reused.nSample() == 3);
         TEST_ASSERT(render(reused) == render(fresh));
         TEST_ASSERT(lag(reused, 0) == 0.0);
         TEST_ASSERT(lag(reused, 1) == 1.0);
         TEST_ASSERT(lag(reused, 2) == 4.0);
      }
   }

   void testDisplacementSameSize()
   {
      printMethod(TEST_FUNC);
      MeanSqDispArray<double> accumulator;
      accumulator.setParam(2, 8);
      DArray<double> values;
      values.allocate(2);
      values[0] = values[1] = 0.0;
      accumulator.sample(values);
      accumulator.setNEnsemble(2);
      TEST_ASSERT(accumulator.nSample() == 1);
      values[0] = values[1] = 2.0;
      accumulator.sample(values);
      TEST_ASSERT(lag(accumulator, 1) == 4.0);
   }

private:
   template <class Accumulator>
   std::string render(Accumulator& accumulator)
   {
      std::ostringstream output;
      accumulator.output(output);
      return output.str();
   }

   template <class Accumulator>
   double lag(Accumulator& accumulator, int wanted)
   {
      std::istringstream input(render(accumulator));
      int index;
      double value;
      while (input >> index >> value) {
         if (index == wanted) return value;
      }
      TEST_ASSERT(false);
      return 0.0;
   }
};

TEST_BEGIN(EnsembleResizeTest)
TEST_ADD(EnsembleResizeTest, testCorrelationResize)
TEST_ADD(EnsembleResizeTest, testCorrelationSameSize)
TEST_ADD(EnsembleResizeTest, testDisplacementResize)
TEST_ADD(EnsembleResizeTest, testDisplacementSameSize)
TEST_END(EnsembleResizeTest)

#endif
