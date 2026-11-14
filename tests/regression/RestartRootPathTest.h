#ifndef UTIL_RESTART_ROOT_PATH_TEST_H
#define UTIL_RESTART_ROOT_PATH_TEST_H

#include <test/UnitTest.h>
#include <test/UnitTestRunner.h>
#include <util/misc/FileMaster.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

class RestartRootPathTest : public UnitTest
{
public:
   void testRootAndDirectoryId()
   {
      printMethod(TEST_FUNC);
      check(true, false, true, true);
      check(true, true, true, true);
      check(true, false, true, false);
   }

   void testSinglePrefixControls()
   {
      printMethod(TEST_FUNC);
      check(false, false, false, false);
      check(true, false, false, false);
      check(false, false, true, true);
   }

private:
   // Isolate even a misdirected write, and restore cwd after a failed assertion.
   class Directory
   {
   public:
      Directory()
      {
         using Util::Exception;
         char buffer[4096];
         UTIL_CHECK(::getcwd(buffer, sizeof(buffer)));
         previous_ = buffer;
         char pattern[] = "RestartRootPath-XXXXXX";
         char* name = ::mkdtemp(pattern);
         UTIL_CHECK(name);
         path_ = previous_ + "/" + name;
         UTIL_CHECK(::chdir(path_.c_str()) == 0);
      }
      ~Directory()
      {
         ::chdir(previous_.c_str());
         // These are the only locations the test or buggy writer can use.
         for (const char* suffix : {"root/7/", "root/", "7/", ""}) {
            std::string base = path_ + "/" + suffix;
            std::remove((base+"restart.txt").c_str());
            std::remove((base+"input/ordinary.txt").c_str());
            std::remove((base+"output/ordinary.txt").c_str());
            ::rmdir((base+"input").c_str());
            ::rmdir((base+"output").c_str());
            ::rmdir(base.c_str());
         }
      }
      std::string path() const { return path_; }
   private:
      std::string previous_, path_;
   };

   void put(std::string const& path, int value)
   {
      std::ofstream out(path.c_str());
      TEST_ASSERT(out.is_open());
      out << value << '\n';
      TEST_ASSERT(bool(out));
   }
   int get(std::string const& path)
   {
      std::ifstream in(path.c_str());
      int value = 0;
      TEST_ASSERT(bool(in >> value));
      return value;
   }
   void check(bool root, bool absolute, bool id, bool wrongExists)
   {
      Directory directory;
      if (root) TEST_ASSERT(::mkdir("root", 0700) == 0);
      const std::string expected = (root ? "root/" : "") + std::string(id ? "7/" : "");
      if (id) TEST_ASSERT(::mkdir(expected.c_str(), 0700) == 0);
      if (root && id && wrongExists) TEST_ASSERT(::mkdir("7", 0700) == 0);
      TEST_ASSERT(::mkdir((expected+"input").c_str(), 0700) == 0);
      TEST_ASSERT(::mkdir((expected+"output").c_str(), 0700) == 0);
      Util::FileMaster master;
      if (root) master.setRootPrefix(absolute ? directory.path()+"/root/" : "root/");
      if (id) master.setDirectoryId(7);
      master.setInputPrefix("input/"); master.setOutputPrefix("output/");
      put(expected+"input/ordinary.txt", 2718);
      {
         std::ifstream in;
         master.openInputFile("ordinary.txt", in);
         int value = 0; in >> value;
         TEST_ASSERT(!in.fail() && value == 2718);
      }
      {
         std::ofstream out;
         master.openOutputFile("ordinary.txt", out); out << 1618 << '\n';
      }
      TEST_ASSERT(get(expected+"output/ordinary.txt") == 1618);
      put(expected+"restart.txt", 111);
      if (root && id && wrongExists) put("7/restart.txt", 222);
      {
         std::ifstream in;
         master.openRestartIFile("restart.txt", in);
         int value = 0; in >> value;
         TEST_ASSERT(!in.fail() && value == 111);
      }
      {
         std::ofstream out;
         master.openRestartOFile("restart.txt", out); out << 314159 << '\n';
      }
      TEST_ASSERT(get(expected+"restart.txt") == 314159);
      {
         std::ifstream in;
         master.openRestartIFile("restart.txt", in);
         int value = 0; in >> value;
         TEST_ASSERT(!in.fail() && value == 314159);
      }
      if (root && id && wrongExists) TEST_ASSERT(get("7/restart.txt") == 222);
      if (root && id && !wrongExists) {
         std::ifstream wrong("7/restart.txt");
         TEST_ASSERT(!wrong.is_open());
      }
   }
};
TEST_BEGIN(RestartRootPathTest)
TEST_ADD(RestartRootPathTest, testRootAndDirectoryId)
TEST_ADD(RestartRootPathTest, testSinglePrefixControls)
TEST_END(RestartRootPathTest)
#endif
