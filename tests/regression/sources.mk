util_tests_regression_=util/tests/regression/Test.cc

util_tests_regression_OBJS=\
     $(addprefix $(BLD_DIR)/, $(util_tests_regression_:.cc=.o))

