include ../../../config.mk
include $(SRC_DIR)/util/sources.mk
include $(SRC_DIR)/util/tests/regression/sources.mk
include $(SRC_DIR)/util/patterns.mk

TEST=util/tests/regression/Test

all: $(util_tests_regression_OBJS) $(BLD_DIR)/$(TEST)
ifdef UTIL_MPI
all: $(BLD_DIR)/util/tests/regression/MpiTest
endif

clean:
	rm -f $(util_tests_regression_OBJS) $(util_tests_regression_OBJS:.o=.d)
	rm -f $(BLD_DIR)/$(TEST) $(BLD_DIR)/util/tests/regression/MpiTest

.PHONY: all clean

-include $(util_tests_regression_OBJS:.o=.d)
-include $(util_DEPS)
