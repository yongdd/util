/*
* Run the MPI regression tests with at least two processes.
* The separate --required-eof case must terminate via MPI_Abort with code 65.
*/
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
   if (argc == 2 && std::string(argv[1]) == "--required-eof") {
      // UTIL_THROW aborts an initialized MPI world; it cannot be caught here.
      FactoryLabelNode parent;
      parent.setIoCommunicator(MPI::COMM_WORLD);
      FactoryLabelMaker factory;
      std::istringstream in("");
      std::string name;
      bool end = false;
      factory.readObject(in, parent, name, end);
      MPI::Finalize();
      return 1; // Returning instead of rejecting required EOF is a failure.
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
