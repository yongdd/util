#ifndef UTIL_DECIMAL_OUTPUT_MODE_TEST_H
#define UTIL_DECIMAL_OUTPUT_MODE_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>
#include <util/space/Vector.h>
#include <util/space/Tensor.h>
#include <util/archives/TextFileOArchive.h>
#include <util/archives/TextFileIArchive.h>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <string>

class UtilDecimalOutputModeTest : public UnitTest
{
public:
   void testVectors()
   {
      printMethod(TEST_FUNC);
      for (int mode = 0; mode < 4; ++mode) {
         Util::Vector source(0.0), target(-99.0);
         for (int i = 0; i < Util::Dimension; ++i) {
            source[i] = (i % 2 ? -1.0 : 1.0)*1.25*(i+1);
         }
         std::ostringstream out;
         setMode(out, mode);
         out << source << ' ' << 314159;
         TEST_ASSERT(out.str().find("0x") == std::string::npos);
         std::istringstream in(out.str());
         int marker = 0;
         in >> target >> marker;
         TEST_ASSERT(!in.fail() && marker == 314159);
         for (int i = 0; i < Util::Dimension; ++i) {
            TEST_ASSERT(target[i] == source[i]);
         }
      }
   }

   void testTensors()
   {
      printMethod(TEST_FUNC);
      for (int mode = 0; mode < 4; ++mode) {
         Util::Tensor source(0.0), target(-99.0);
         for (int i = 0; i < Util::Dimension; ++i) {
            for (int j = 0; j < Util::Dimension; ++j) {
               source(i,j) = i == j ? 0.0 : 1.25*(i*Util::Dimension+j+1);
            }
         }
         std::ostringstream out;
         setMode(out, mode);
         out << source << ' ' << 314159;
         TEST_ASSERT(out.str().find("0x") == std::string::npos);
         std::istringstream in(out.str());
         int marker = 0;
         in >> target >> marker;
         TEST_ASSERT(!in.fail() && marker == 314159);
         for (int i = 0; i < Util::Dimension; ++i) {
            for (int j = 0; j < Util::Dimension; ++j) {
               TEST_ASSERT(target(i,j) == source(i,j));
            }
         }
      }
   }

   void testScalarArchives()
   {
      printMethod(TEST_FUNC);
      for (int mode = 0; mode < 4; ++mode) {
         checkArchive(mode, false);
      }
   }

   void testArrayArchives()
   {
      printMethod(TEST_FUNC);
      for (int mode = 0; mode < 4; ++mode) {
         checkArchive(mode, true);
      }
   }

private:
   struct TemporaryFile
   {
      const char* path;
      explicit TemporaryFile(const char* name) : path(name) {}
      ~TemporaryFile() { std::remove(path); }
   };

   void setMode(std::ostream& out, int mode)
   {
      if (mode == 1) out << std::scientific;
      if (mode == 2) out << std::fixed;
      if (mode == 3) out << std::hexfloat;
   }

   void checkArchive(int mode, bool array)
   {
      TemporaryFile file(array ? "DecimalOutputArray.txt" : "DecimalOutputScalar.txt");
      const double values[] = {1.25, -2.5, 3.75};
      // Save logical 2x3 rows from stride 5; restore to different stride 4.
      const double rows[] = {1.25, 0.0, -2.5, -99.0, -99.0,
                             3.75, -5.0, 6.25, -99.0, -99.0};
      int marker = 314159;
      {
         Util::TextFileOArchive out(file.path);
         TEST_ASSERT(out.file().is_open());
         setMode(out.file(), mode);
         if (array) {
            out.pack(values, 3);
         } else {
            out.pack(values[0]);
         }
         // Generic 2D packing inherits the selected decimal format; this
         // checks continuation, not a new precision policy for generic data.
         out.pack(rows, 2, 3, 5);
         out & marker;
         TEST_ASSERT(!out.file().fail());
      }
      double restored[3] = {-99.0, -99.0, -99.0};
      double restoredRows[8];
      for (int i = 0; i < 8; ++i) restoredRows[i] = -99.0;
      int restoredMarker = 0;
      {
         Util::TextFileIArchive in(file.path);
         TEST_ASSERT(in.file().is_open());
         if (array) {
            in.unpack(restored, 3);
         } else {
            in.unpack(restored[0]);
         }
         in.unpack(restoredRows, 2, 3, 4);
         in & restoredMarker;
         TEST_ASSERT(!in.file().fail());
      }
      for (int i = 0; i < (array ? 3 : 1); ++i) {
         TEST_ASSERT(restored[i] == values[i]);
      }
      if (!array) {
         TEST_ASSERT(restored[1] == -99.0 && restored[2] == -99.0);
      }
      for (int i = 0; i < 2; ++i) {
         for (int j = 0; j < 3; ++j) {
            TEST_ASSERT(restoredRows[i*4+j] == rows[i*5+j]);
         }
         TEST_ASSERT(restoredRows[i*4+3] == -99.0);
      }
      TEST_ASSERT(restoredMarker == marker);
   }
};

TEST_BEGIN(UtilDecimalOutputModeTest)
TEST_ADD(UtilDecimalOutputModeTest, testVectors)
TEST_ADD(UtilDecimalOutputModeTest, testTensors)
TEST_ADD(UtilDecimalOutputModeTest, testScalarArchives)
TEST_ADD(UtilDecimalOutputModeTest, testArrayArchives)
TEST_END(UtilDecimalOutputModeTest)

#endif
