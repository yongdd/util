#ifndef UTIL_MPI_FACTORY_LABEL_TEST_H
#define UTIL_MPI_FACTORY_LABEL_TEST_H
#include <util/param/Factory.h>
#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>
#include <sstream>

class FactoryLabelNode : public Util::ParamComposite
{
public:
   FactoryLabelNode() : value(7) { setClassName("Known"); }
   void readParameters(std::istream& in) { read(in, "value", value); }
   int value;
};
class FactoryLabelMaker : public Util::Factory<FactoryLabelNode>
{
public:
   explicit FactoryLabelMaker(const char* name = "Known") : name_(name) {}
   FactoryLabelNode* factory(const std::string& name) const
   { return name == name_ ? new FactoryLabelNode : 0; }
private:
   std::string name_;
};
class MpiFactoryLabelTest : public UnitTest
{
public:
   void setUp() { Util::Label::clear(); }
   void tearDown() { Util::Label::clear(); }
   void testKnownObjectParameters()
   {
      printMethod(TEST_FUNC);
      FactoryLabelNode parent;
      parent.setIoCommunicator(communicator());
      FactoryLabelMaker factory;
      std::istringstream in(mpiRank() == 0 ? "Known{ value 17 }" : "");
      std::string name;
      bool end = true;
      FactoryLabelNode* child = factory.readObject(in, parent, name, end);
      bool ok = child && child->value == 17 && name == "Known" && !end;
      delete child;
      TEST_ASSERT(ok);
   }
   void testOptionalLookaheadAndClosingLabel()
   {
      printMethod(TEST_FUNC);
      FactoryLabelNode parent;
      parent.setIoCommunicator(communicator());
      FactoryLabelMaker known, other("Other");
      std::istringstream in(mpiRank() == 0 ? "Other{} Known{} }" : "");
      std::string name;
      bool end = true;
      FactoryLabelNode* child = known.readObjectOptional(in,parent,name,end);
      TEST_ASSERT(!child && name == "Other" && !end);
      TEST_ASSERT(Util::Label::buffer() == "Other{}");
      TEST_ASSERT(!Util::Label::isClear() && !Util::Label::isMatched());
      child = other.readObjectOptional(in,parent,name,end);
      bool ok = child && child->value == 7 && name == "Other" && !end;
      delete child; TEST_ASSERT(ok);
      child = known.readObjectOptional(in,parent,name,end);
      ok = child && name == "Known" && !end;
      delete child; TEST_ASSERT(ok);
      child = known.readObjectOptional(in,parent,name,end);
      TEST_ASSERT(!child && name.empty() && end);
      TEST_ASSERT(Util::Label::buffer() == "}" && !Util::Label::isClear());
      child = known.readObject(in,parent,name,end);
      TEST_ASSERT(!child && end && Util::Label::isClear());
      TEST_ASSERT(Util::Label::isMatched());
   }
   void testOptionalEmptyAndWhitespace()
   {
      printMethod(TEST_FUNC);
      for (const char* text : {"", " \t\n"}) {
         Util::Label::clear();
         FactoryLabelNode parent;
         parent.setIoCommunicator(communicator());
         FactoryLabelMaker factory;
         std::istringstream in(mpiRank() == 0 ? text : "");
         for (int repeat = 0; repeat < 2; ++repeat) {
            std::string name("stale");
            bool end = true;
            FactoryLabelNode* child = factory.readObjectOptional(in,parent,name,end);
            TEST_ASSERT(!child && name.empty() && !end);
            TEST_ASSERT(Util::Label::isClear());
         }
      }
   }
   void testOptionalAfterLastObject()
   {
      printMethod(TEST_FUNC);
      FactoryLabelNode parent;
      parent.setIoCommunicator(communicator());
      FactoryLabelMaker factory;
      std::istringstream in(mpiRank() == 0 ? "Known{}" : "");
      std::string name;
      bool end = true;
      FactoryLabelNode* child = factory.readObject(in,parent,name,end);
      bool ok = child != 0;
      delete child; TEST_ASSERT(ok);
      child = factory.readObjectOptional(in,parent,name,end);
      TEST_ASSERT(!child && name.empty() && !end);
   }

};
TEST_BEGIN(MpiFactoryLabelTest)
TEST_ADD(MpiFactoryLabelTest, testKnownObjectParameters)
TEST_ADD(MpiFactoryLabelTest, testOptionalLookaheadAndClosingLabel)
TEST_ADD(MpiFactoryLabelTest, testOptionalEmptyAndWhitespace)
TEST_ADD(MpiFactoryLabelTest, testOptionalAfterLastObject)
TEST_END(MpiFactoryLabelTest)
#endif
