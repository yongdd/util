#ifndef UTIL_EMPTY_ARRAY_PARAM_TEST_H
#define UTIL_EMPTY_ARRAY_PARAM_TEST_H
#include <util/param/FSArrayParam.h>
#include <util/param/ScalarParam.h>
#include <util/param/BracketPolicy.h>
#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>
#include <sstream>

class EmptyArrayParamTest : public UnitTest
{
   Util::BracketPolicy::Type policy_;
   bool echo_;
public:
   void setUp()
   {
      policy_ = Util::BracketPolicy::get();
      echo_ = Util::ParamComponent::echo();
      Util::ParamComponent::setEcho(false);
      Util::Label::clear();
   }
   void tearDown()
   {
      Util::Label::clear();
      Util::BracketPolicy::set(policy_);
      Util::ParamComponent::setEcho(echo_);
   }
   void check(Util::BracketPolicy::Type policy, int n, bool required,
              bool brackets, bool present = true)
   {
      using namespace Util;
      BracketPolicy::set(policy);
      Label::clear(); Label::setIsMatched(false);
      FSArray<int,4> source;
      FSArrayParam<int,4> parameter("values", source, n, required);
      int marker = 0;
      ScalarParam<int> following("next", marker);
      std::ostringstream input;
      if (present) {
         input << "values" << (brackets ? "[\n" : "\n");
         for (int i = 0; i < n; ++i) { input << 10+i << '\n'; }
         if (brackets) { input << "]\n"; }
      }
      input << "next 314159\n";
      std::istringstream in(input.str());
      parameter.readParam(in); following.readParam(in);
      TEST_ASSERT(parameter.isActive() == present);
      TEST_ASSERT(source.size() == (present ? n : 0));
      TEST_ASSERT(marker == 314159 && Label::isClear());
      std::ostringstream out;
      parameter.writeParam(out); following.writeParam(out);
      TEST_ASSERT((out.str().find("values") != std::string::npos) == present);
      FSArray<int,4> target;
      FSArrayParam<int,4> loaded("values", target, n, required);
      int last = 0;
      ScalarParam<int> loadedFollowing("next", last);
      std::istringstream restored(out.str());
      loaded.readParam(restored); loadedFollowing.readParam(restored);
      TEST_ASSERT(loaded.isActive() == present && last == 314159);
      TEST_ASSERT(target.size() == source.size() && Label::isClear());
      for (int i = 0; i < target.size(); ++i) {
         TEST_ASSERT(target[i] == source[i]);
      }
   }
   void testEmptyWithoutBrackets()
   {
      printMethod(TEST_FUNC);
      check(Util::BracketPolicy::Forbidden, 0, true, false);
      check(Util::BracketPolicy::Forbidden, 0, false, false);
   }
   void testAbsentAndNonempty()
   {
      printMethod(TEST_FUNC);
      check(Util::BracketPolicy::Forbidden, 0, false, false, false);
      check(Util::BracketPolicy::Forbidden, 2, true, false);
      check(Util::BracketPolicy::Forbidden, 2, false, false);
   }
   void testOtherPolicies()
   {
      printMethod(TEST_FUNC);
      check(Util::BracketPolicy::Required, 0, true, true);
      check(Util::BracketPolicy::Required, 0, false, true);
      check(Util::BracketPolicy::Optional, 0, true, false);
      check(Util::BracketPolicy::Optional, 0, true, true);
      check(Util::BracketPolicy::Optional, 0, false, false);
   }
};
TEST_BEGIN(EmptyArrayParamTest)
TEST_ADD(EmptyArrayParamTest, testEmptyWithoutBrackets)
TEST_ADD(EmptyArrayParamTest, testAbsentAndNonempty)
TEST_ADD(EmptyArrayParamTest, testOtherPolicies)
TEST_END(EmptyArrayParamTest)
#endif
