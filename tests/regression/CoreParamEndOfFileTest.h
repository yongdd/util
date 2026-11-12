#ifndef CORE_PARAM_END_OF_FILE_TEST_H
#define CORE_PARAM_END_OF_FILE_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>

#include <util/param/ParamComposite.h>
#include <util/param/ParamComponent.h>
#include <util/param/Label.h>
#include <util/misc/Exception.h>

#include <sstream>
#include <string>

using namespace Util;

/*
* Optional block "Opt{ y }" of the end-of-file tests.
*/
class CoreParamEndChild : public ParamComposite
{

public:

   CoreParamEndChild()
    : y(0)
   {  setClassName("Opt"); }

   virtual void readParameters(std::istream& in)
   {  read<int>(in, "y", y); }

   int y;

};

/*
* Block "Top{ x [z] [Opt{ y }] }": a required parameter followed by an
* optional parameter (if hasOptionalParam) and an optional block (if
* hasOptionalBlock), as System is followed by the optional Sweep and
* Simulator blocks in pscf.
*/
class CoreParamEndParent : public ParamComposite
{

public:

   CoreParamEndParent(bool hasOptionalParam, bool hasOptionalBlock)
    : x(0),
      z(0),
      child(),
      hasOptionalParam_(hasOptionalParam),
      hasOptionalBlock_(hasOptionalBlock)
   {  setClassName("Top"); }

   virtual void readParameters(std::istream& in)
   {
      read<int>(in, "x", x);
      if (hasOptionalParam_) {
         readOptional<int>(in, "z", z);
      }
      if (hasOptionalBlock_) {
         readParamCompositeOptional(in, child);
      }
   }

   int x;
   int z;
   CoreParamEndChild child;

private:

   bool hasOptionalParam_;
   bool hasOptionalBlock_;

};

/*
* Parameter files that end at the end of the stream.
*
* A file whose closing bracket is the last character, without a final
* newline, was rejected when an optional item came last in the block:
* the attempt to match the optional item read the '}' into the label
* buffer and set eofbit, and the extractor for the required '}' then
* threw 'EOF before reading required label' without looking at the
* buffer. Conversely, an optional block that was absent at the end of
* the stream was taken to be present, because Begin tested whether the
* label buffer was clear instead of whether the label was matched.
*/
class CoreParamEndOfFileTest : public UnitTest
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
   * Optional parameter as last item, '}' as last character.
   */
   void testOptionalParamLast()
   {
      printMethod(TEST_FUNC);
      std::string text = "Top{\n  x  3\n}";

      CoreParamEndParent noEol(true, false);
      TEST_ASSERT(!readThrows(noEol, text));
      TEST_ASSERT(noEol.x == 3);
      TEST_ASSERT(noEol.isActive());

      CoreParamEndParent eol(true, false);
      TEST_ASSERT(!readThrows(eol, text + "\n"));
      TEST_ASSERT(eol.x == 3);

      // The optional parameter, when present, is read
      CoreParamEndParent present(true, false);
      TEST_ASSERT(!readThrows(present, "Top{\n  x  3\n  z  4\n}"));
      TEST_ASSERT(present.x == 3);
      TEST_ASSERT(present.z == 4);
   }

   /*
   * Optional block as last item, '}' as last character.
   */
   void testOptionalBlockLast()
   {
      printMethod(TEST_FUNC);

      CoreParamEndParent absent(true, true);
      TEST_ASSERT(!readThrows(absent, "Top{\n  x  3\n}"));
      TEST_ASSERT(absent.x == 3);
      TEST_ASSERT(!absent.child.isActive());

      CoreParamEndParent present(true, true);
      TEST_ASSERT(!readThrows(present,
                              "Top{\n  x  3\n  Opt{\n    y  5\n  }\n}"));
      TEST_ASSERT(present.x == 3);
      TEST_ASSERT(present.child.isActive());
      TEST_ASSERT(present.child.y == 5);

      // A missing closing bracket is still an error
      CoreParamEndParent truncated1(true, true);
      TEST_ASSERT(readThrows(truncated1, "Top{\n  x  3\n"));
      CoreParamEndParent truncated2(true, true);
      TEST_ASSERT(readThrows(truncated2, "Top{\n  x  3"));
   }

   /*
   * Optional block that is absent at the end of the stream.
   */
   void testOptionalBlockAtEnd()
   {
      printMethod(TEST_FUNC);
      const char* texts[3] = {"Top{\n  x  3\n}", "Top{\n  x  3\n}\n",
                              "Top{\n  x  3\n}\n   \n"};
      for (int i = 0; i < 3; ++i) {
         std::istringstream in(texts[i]);
         CoreParamEndParent top(false, false);
         CoreParamEndChild extra;
         bool thrown = false;
         try {
            top.readParam(in);
            extra.readParamOptional(in);
         } catch (Exception&) {
            thrown = true;
         }
         Label::clear();
         TEST_ASSERT(!thrown);
         TEST_ASSERT(top.x == 3);
         TEST_ASSERT(!extra.isActive());
      }

      // Empty stream
      std::istringstream in("");
      CoreParamEndChild extra;
      bool thrown = false;
      try {
         extra.readParamOptional(in);
      } catch (Exception&) {
         thrown = true;
      }
      Label::clear();
      TEST_ASSERT(!thrown);
      TEST_ASSERT(!extra.isActive());
   }

private:

   /*
   * Read block from text, return true if this throws an Exception.
   */
   bool readThrows(ParamComposite& block, std::string const & text)
   {
      std::istringstream in(text);
      bool thrown = false;
      try {
         block.readParam(in);
      } catch (Exception&) {
         thrown = true;
      }
      Label::clear();
      return thrown;
   }

};

TEST_BEGIN(CoreParamEndOfFileTest)
TEST_ADD(CoreParamEndOfFileTest, testOptionalParamLast)
TEST_ADD(CoreParamEndOfFileTest, testOptionalBlockLast)
TEST_ADD(CoreParamEndOfFileTest, testOptionalBlockAtEnd)
TEST_END(CoreParamEndOfFileTest)

#endif
