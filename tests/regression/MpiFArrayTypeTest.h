#ifndef UTIL_MPI_F_ARRAY_TYPE_TEST_H
#define UTIL_MPI_F_ARRAY_TYPE_TEST_H

#include <util/containers/FArray.h>
#include <util/mpi/MpiSendRecv.h>
#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

class MpiFArrayTypeTest : public UnitTest
{
public:
   void testIntegerTypes()
   {
      printMethod(TEST_FUNC);
      const bool small = checkType<int,4>();
      const bool large = checkType<int,32>();
      TEST_ASSERT(small && large);
   }

   void testDoubleTypes()
   {
      printMethod(TEST_FUNC);
      const bool small = checkType<double,2>();
      const bool large = checkType<double,25>();
      TEST_ASSERT(small && large);
   }

private:
   template <typename Data, int Capacity>
   bool checkType()
   {
      typedef Util::FArray<Data,Capacity> Array;
      typedef Util::MpiTraits<Array> Traits;

      Array::commitMpiType();
      const MPI_Datatype first = Traits::type;
      Array::commitMpiType();
      MPI::Aint lower, extent;
      Traits::type.Get_extent(lower, extent);
      const bool metadata = Traits::hasType
         && static_cast<MPI_Datatype>(Traits::type) == first
         && Traits::type.Get_size() == int(Capacity*sizeof(Data))
         && lower == 0 && extent == MPI::Aint(sizeof(Array));

      // All ranks make the same decision before entering any transport.
      const int localReady = metadata && communicator().Get_size() >= 2 ? 1 : 0;
      int ready = 0;
      communicator().Allreduce(&localReady, &ready, 1, MPI::INT, MPI::MIN);
      if (!ready) {
         return false;
      }

      // Two adjacent objects exercise the datatype extent. Full arrays on
      // either side make writes outside the selected objects observable.
      const Data guard = Data(-9876);
      Array packet[4];
      for (int j = 0; j < 4; ++j) {
         for (int i = 0; i < Capacity; ++i) {
            packet[j][i] = guard;
         }
      }
      if (mpiRank() == 0) {
         for (int j = 1; j <= 2; ++j) {
            for (int i = 0; i < Capacity; ++i) {
               packet[j][i] = Data(100*j+i+1);
            }
         }
         Util::send<Array>(communicator(), &packet[1], 2, 1, 927);
      } else if (mpiRank() == 1) {
         Util::recv<Array>(communicator(), &packet[1], 2, 0, 927);
      }

      bool correct = true;
      for (int i = 0; i < Capacity; ++i) {
         correct = correct && packet[0][i] == guard;
         correct = correct && packet[3][i] == guard;
         if (mpiRank() <= 1) {
            correct = correct && packet[1][i] == Data(101+i);
            correct = correct && packet[2][i] == Data(201+i);
         }
      }

      // Broadcast from the receiver, including any additional test ranks.
      Util::bcast<Array>(communicator(), &packet[1], 2, 1);
      for (int i = 0; i < Capacity; ++i) {
         correct = correct && packet[0][i] == guard;
         correct = correct && packet[3][i] == guard;
         correct = correct && packet[1][i] == Data(101+i);
         correct = correct && packet[2][i] == Data(201+i);
      }
      // Assertions are deferred until every case has completed its collectives.
      return correct;
   }
};

TEST_BEGIN(MpiFArrayTypeTest)
TEST_ADD(MpiFArrayTypeTest, testIntegerTypes)
TEST_ADD(MpiFArrayTypeTest, testDoubleTypes)
TEST_END(MpiFArrayTypeTest)

#endif
