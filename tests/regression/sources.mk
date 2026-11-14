util_tests_regression_=util/tests/regression/Test.cc
ifdef UTIL_MPI
util_tests_regression_+=util/tests/regression/MpiTest.cc
endif

util_tests_regression_OBJS=\
     $(addprefix $(BLD_DIR)/, $(util_tests_regression_:.cc=.o))

