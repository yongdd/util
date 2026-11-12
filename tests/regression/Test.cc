/*
* This program runs the regression tests in the util/tests/regression
* directory.
*/

#include "RegressionTestComposite.h"

int main(int argc, char* argv[])
{
   RegressionTestComposite runner;
   if (argc == 2) {
      runner.addFilePrefix(argv[1]);
   }
   int failures = runner.run();
   return (failures != 0);
}
