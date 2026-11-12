#ifndef CORE_PARAM_BLANK_TEST_H
#define CORE_PARAM_BLANK_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

#include <util/param/ParamComposite.h>
#include <util/param/ParamComponent.h>
#include <util/param/Manager.h>
#include <util/param/Factory.h>
#include <util/param/Label.h>
#include <util/misc/Exception.h>

#include <sstream>
#include <string>

using namespace Util;

/*
* Base class of the objects in a CoreBlankManager.
*/
class CoreBlankItem : public ParamComposite
{

public:

   CoreBlankItem()
    : x(0.0),
      m(0)
   {  setClassName("Item"); }

   virtual ~CoreBlankItem()
   {}

   double x;
   int m;

};

/*
* Item "P{ x m }".
*/
class CoreBlankItemP : public CoreBlankItem
{

public:

   CoreBlankItemP()
   {  setClassName("P"); }

   virtual void readParameters(std::istream& in)
   {
      read<double>(in, "x", x);
      read<int>(in, "m", m);
   }

};

/*
* Item "Q{ m }".
*/
class CoreBlankItemQ : public CoreBlankItem
{

public:

   CoreBlankItemQ()
   {  setClassName("Q"); }

   virtual void readParameters(std::istream& in)
   {  read<int>(in, "m", m); }

};

/*
* Factory for P and Q.
*/
class CoreBlankFactory : public Factory<CoreBlankItem>
{

public:

   virtual CoreBlankItem* factory(const std::string& className) const
   {
      if (className == "P") return new CoreBlankItemP();
      if (className == "Q") return new CoreBlankItemQ();
      return 0;
   }

};

/*
* Manager "Items{ [base] P{...} Q{...} ... }". As the AnalyzerManager
* of pscf, it first looks for an optional parameter (base) and then
* reads its items, with a Blank before each item.
*/
class CoreBlankManager : public Manager<CoreBlankItem>
{

public:

   CoreBlankManager()
    : base(0)
   {  setClassName("Items"); }

   virtual void readParameters(std::istream& in)
   {
      base = 0;
      readOptional<int>(in, "base", base);
      Manager<CoreBlankItem>::readParameters(in);
   }

   virtual Factory<CoreBlankItem>* newDefaultFactory() const
   {  return new CoreBlankFactory(); }

   int base;

};

/*
* Blank lines in parameter files, as read before each item of a Manager.
*
* Blank::readParam read the rest of the current line with
* istream::getline into a 255-character buffer. A remainder of 255 or
* more characters (e.g., trailing spaces) set failbit, and reading
* stopped with 'istream::fail() before reading Label'. A shorter
* remainder was discarded even if it was not blank, so that an item
* written on the same line as the closing bracket of the previous item
* was silently dropped, and the first line of an item whose label had
* already been read by the lookahead for an optional parameter lost its
* parameters. Blank now skips white space of any length up to and
* including the end of the line, and leaves other text in the stream.
*/
class CoreParamBlankTest : public UnitTest
{

public:

   void setUp()
   {
      Label::clear();
      ParamComponent::setEcho(false);
   }

   void tearDown()
   {  Label::clear(); }

   /*
   * Standard layout (control).
   */
   void testStandard()
   {
      printMethod(TEST_FUNC);
      CoreBlankManager manager;
      TEST_ASSERT(!readThrows(manager, standard("", "", "")));
      checkStandard(manager);

      CoreBlankManager withBase;
      TEST_ASSERT(!readThrows(withBase,
                  "Items{\n  base 7\n\n  P{\n    x 2.5\n    m 1\n  }\n"
                  "\n  Q{\n    m 3\n  }\n\n}\n"));
      checkStandard(withBase);
      TEST_ASSERT(withBase.base == 7);
   }

   /*
   * Long trailing white space where a Blank is read.
   */
   void testLongLine()
   {
      printMethod(TEST_FUNC);
      std::string spaces(300, ' ');
      int nSpace[4] = {253, 254, 255, 300};

      for (int i = 0; i < 4; ++i) {
         std::string s(nSpace[i], ' ');

         // After the closing bracket of the first item
         CoreBlankManager m1;
         TEST_ASSERT(!readThrows(m1, standard("", s, "")));
         checkStandard(m1);

         // After the closing bracket of the last item
         CoreBlankManager m2;
         TEST_ASSERT(!readThrows(m2, standard("", "", s)));
         checkStandard(m2);

         // After the label of the first item, read by the lookahead
         CoreBlankManager m3;
         TEST_ASSERT(!readThrows(m3, standard(s, "", "")));
         checkStandard(m3);
      }

      // Tabs and a carriage return
      CoreBlankManager m4;
      TEST_ASSERT(!readThrows(m4, standard("", spaces + "\t \r", "")));
      checkStandard(m4);
   }

   /*
   * Item on the same line as the closing bracket of the previous item.
   */
   void testItemOnSameLine()
   {
      printMethod(TEST_FUNC);

      CoreBlankManager manager;
      TEST_ASSERT(!readThrows(manager,
                  "Items{\n  P{\n    x 2.5\n    m 1\n  }  Q{ m 3 }\n}\n"));
      checkStandard(manager);

      // All on one line
      CoreBlankManager oneLine;
      TEST_ASSERT(!readThrows(oneLine,
                  "Items{ P{ x 2.5 m 1 } Q{ m 3 } }"));
      checkStandard(oneLine);
   }

   /*
   * Parameters on the line of an item label that was read by the
   * lookahead for the optional parameter base.
   */
   void testParamOnLabelLine()
   {
      printMethod(TEST_FUNC);

      CoreBlankManager manager;
      TEST_ASSERT(!readThrows(manager,
                  "Items{\n  P{  x 2.5\n    m 1\n  }\n  Q{\n    m 3\n  }\n}\n"));
      checkStandard(manager);
      TEST_ASSERT(manager.base == 0);
   }

private:

   /*
   * Standard manager block, with text appended to the line of the first
   * item label (afterLabel), the line of the closing bracket of the
   * first item (afterFirst) and that of the last item (afterLast).
   */
   std::string standard(std::string const & afterLabel,
                        std::string const & afterFirst,
                        std::string const & afterLast)
   {
      return "Items{\n\n  P{" + afterLabel + "\n    x 2.5\n    m 1\n  }"
             + afterFirst + "\n\n  Q{\n    m 3\n  }" + afterLast
             + "\n\n}\n";
   }

   /*
   * Check the items read from a standard manager block.
   */
   void checkStandard(CoreBlankManager& manager)
   {
      TEST_ASSERT(manager.size() == 2);
      TEST_ASSERT(manager[0].className() == "P");
      TEST_ASSERT(manager[0].x == 2.5);
      TEST_ASSERT(manager[0].m == 1);
      TEST_ASSERT(manager[1].className() == "Q");
      TEST_ASSERT(manager[1].m == 3);
   }

   /*
   * Read manager from text, return true if this throws an Exception.
   */
   bool readThrows(CoreBlankManager& manager, std::string const & text)
   {
      std::istringstream in(text);
      bool thrown = false;
      try {
         manager.readParam(in);
      } catch (Exception&) {
         thrown = true;
      }
      Label::clear();
      return thrown;
   }

};

TEST_BEGIN(CoreParamBlankTest)
TEST_ADD(CoreParamBlankTest, testStandard)
TEST_ADD(CoreParamBlankTest, testLongLine)
TEST_ADD(CoreParamBlankTest, testItemOnSameLine)
TEST_ADD(CoreParamBlankTest, testParamOnLabelLine)
TEST_END(CoreParamBlankTest)

#endif
