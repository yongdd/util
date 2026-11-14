#!/usr/bin/env python3
"""Check the advertised standalone util feature configuration in tiny fixtures."""
import argparse
import os
from pathlib import Path
import shlex
import shutil
import subprocess
import sys
import tempfile
import unittest

UTIL_SOURCE = Path(__file__).resolve().parents[2]
FIXTURE_FILES = (
    '.config/setup', '.config/main_config.1', '.config/main_config.2',
    '.config/configure', '.config/util_config', '.config/compiler/gcc',
    '.config/makeDep', '.config/makeDepend.py', '.config/TextWrapper.py',
    '.config/doc/Doxyfile', '.config/doc/makefile', 'patterns.mk', 'global.h',
    'misc/Log.h', 'misc/Exception.h', 'misc/Log.cpp', 'misc/Exception.cpp',
)
MARKER = r'''#include <util/global.h>
#include <cstdio>
using Util::Exception;
int main()
{
#ifdef UTIL_DEBUG
   const int debug = 1;
#else
   const int debug = 0;
#endif
#ifdef NDEBUG
   const int ndebug = 1;
#else
   const int ndebug = 0;
#endif
   int evaluated = 0;
   UTIL_ASSERT((++evaluated, true));
   std::printf("%d %d %d\n", debug, ndebug, evaluated);
}
'''
MAKEFILE = '''include ../config.mk
include patterns.mk
.PHONY: marker print-options
marker: $(BLD_DIR)/util/FeatureProbe.o $(BLD_DIR)/util/misc/Exception.o $(BLD_DIR)/util/misc/Log.o
\t$(CXX) $(LDFLAGS) -o $(BLD_DIR)/FeatureProbe $^
print-options:
\t@printf '%s\\n' 'CPPFLAGS=$(CPPFLAGS)' 'CXX=$(CXX)' 'CXX_STD=$(CXX_STD)' 'MAKEDEP_ARGS=$(MAKEDEP_ARGS)'
'''


class StandaloneBuildConfigTest(unittest.TestCase):
    def setUp(self):
        for tool in ('bash', 'make', 'g++', 'python3'):
            if shutil.which(tool) is None:
                self.skipTest('Standalone GNU build requires ' + tool)
        self.temp = tempfile.TemporaryDirectory(prefix='util-build-config-')
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.source = self.root / 'src/util'
        for name in FIXTURE_FILES:
            target = self.source / name
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(UTIL_SOURCE / name, target)
        self.env = os.environ.copy()
        for key in ('MAKEFLAGS', 'MFLAGS', 'CPPFLAGS', 'CXXFLAGS',
                    'UTIL_DEBUG', 'UTIL_MPI', 'UTIL_CXX11'):
            self.env.pop(key, None)
        self.env['PYTHONDONTWRITEBYTECODE'] = '1'
        self.env['PYTHONPATH'] = str(self.root / 'scripts/python')
        self.runCommand(['bash', '.config/setup'])
        (self.source / 'Makefile').write_text(MAKEFILE)
        (self.source / 'FeatureProbe.cpp').write_text(MARKER)

    def runCommand(self, args, cwd=None):
        result = subprocess.run(args, cwd=cwd or self.source, env=self.env,
                                text=True, stdout=subprocess.PIPE,
                                stderr=subprocess.STDOUT, timeout=30)
        self.assertEqual(result.returncode, 0,
                         'Command failed: ' + repr(args) + '\n' + result.stdout)
        return result.stdout

    def configure(self, *args):
        return self.runCommand(['bash', 'configure', *args], self.root / 'src')

    def options(self):
        text = self.runCommand(['make', '--no-print-directory', 'print-options'])
        return dict(line.split('=', 1) for line in text.splitlines() if '=' in line)

    def testDebugToggleAndQuery(self):
        for option, expected, query in (
            (None, (0, 1, 0), '-g OFF'),
            ('-g1', (1, 0, 1), '-g ON'),
            ('-g0', (0, 1, 0), '-g OFF'),
        ):
            with self.subTest(option=option):
                if option:
                    self.configure(option)
                # Force these fixture objects to rebuild; generated dependency
                # files are intentionally not included by the small harness.
                self.runCommand(['make', '--no-print-directory', '-B', 'marker'])
                actual = tuple(map(int, self.runCommand(
                    [str(self.root / 'src/FeatureProbe')]).split()))
                self.assertEqual(actual, expected)
                self.assertIn(query, self.configure('-q'))

    def testFeatureAndDependencyFlags(self):
        self.env['CPPFLAGS'] = '-DUSER_MARKER'
        self.configure('-m1', '-g1')
        config = self.root / 'src/config.mk'
        text = config.read_text()
        self.assertIn('#UTIL_CXX11=1', text)
        config.write_text(text.replace('#UTIL_CXX11=1', 'UTIL_CXX11=1'))
        enabled = self.options()
        flags = {'-DUTIL_MPI', '-DUTIL_DEBUG', '-DUTIL_CXX11', '-DUSER_MARKER'}
        for key in ('CPPFLAGS', 'MAKEDEP_ARGS'):
            self.assertTrue(flags.issubset(shlex.split(enabled[key])), enabled)
        self.assertEqual(enabled['CXX'], 'mpicxx')
        self.assertEqual(enabled['CXX_STD'], '--std=c++11')
        query = self.configure('-q')
        self.assertIn('-m ON', query)
        self.assertIn('-g ON', query)
        # This checks make expansion only; no MPI compiler/runtime is required.
        self.configure('-m0', '-g0')
        config.write_text(config.read_text().replace('\nUTIL_CXX11=1', '\n#UTIL_CXX11=1'))
        disabled = self.options()
        for key in ('CPPFLAGS', 'MAKEDEP_ARGS'):
            tokens = set(shlex.split(disabled[key]))
            self.assertFalse(tokens & (flags - {'-DUSER_MARKER'}), disabled)
            self.assertIn('-DUSER_MARKER', tokens)
        self.assertEqual(disabled['CXX'], 'g++')
        self.assertEqual(disabled['CXX_STD'], '--std=c++98')
        query = self.configure('-q')
        self.assertIn('-m OFF', query)
        self.assertIn('-g OFF', query)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument('--util-source', type=Path, default=UTIL_SOURCE)
    options, remaining = parser.parse_known_args()
    UTIL_SOURCE = options.util_source.resolve()
    unittest.main(argv=[sys.argv[0], *remaining])
