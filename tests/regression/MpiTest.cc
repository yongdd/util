/* Run the MPI regression tests with at least two processes. */
#ifdef UTIL_MPI
#define TEST_MPI
#include <util/global.h>
#include "MpiRegressionTestComposite.h"

int main(int argc, char** argv)
{
   MPI::Init(argc, argv);
   if (MPI::COMM_WORLD.Get_size() < 2) {
      std::cerr << "MPI regression tests require at least two processes\n";
      MPI::Finalize();
      return 1;
   }
   MpiRegressionTestComposite runner;
   int local = runner.run(), failures = 0;
   MPI::COMM_WORLD.Allreduce(&local, &failures, 1, MPI::INT, MPI::MAX);
   MPI::Finalize();
   return failures != 0;
}
#else
#error "Build MpiTest with UTIL_MPI enabled"
#endif
