BLD_DIR_REL =../../../..
include $(BLD_DIR_REL)/config.mk
include $(BLD_DIR)/util/config.mk
include $(SRC_DIR)/util/patterns.mk
include $(SRC_DIR)/util/sources.mk

TEST_DIR=$(BLD_DIR)/util/tests/param/serial
include $(SRC_DIR)/util/tests/param/serial/sources.mk

TEST=$(TEST_DIR)/Test
.SECONDARY: $(TEST).o

all: $(TEST)

run: $(TEST)
	$(TEST)

clean:
	rm -f $(TEST)
	rm -f $(util_tests_param_serial_OBJS) 
	rm -f $(util_tests_param_serial_OBJS:.o=.d)
	rm -f $(util_tests_param_serial_OBJS:.o=)
	$(MAKE) clean-outputs

clean-outputs:
	@rm -f $(SRC_DIR)/util/tests/param/serial/out/*

-include $(util_tests_param_serial_OBJS:.o=.d)
-include $(util_DEPS)

