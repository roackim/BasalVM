TARGET_EXEC ?= main

BIN_DIR ?= bin
BUILD_DIR ?= objects
SRC_DIRS ?= src

CXX=g++

SRCS := $(shell find $(SRC_DIRS) -name *.cpp)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)



CPP_FLAGS ?= -O3 -pedantic -Wall -Wpedantic -Wextra -Wcast-align -Wuseless-cast -Wunused -Wnull-dereference -Wdouble-promotion -Wmisleading-indentation -Wduplicated-cond -Wduplicated-branches -Wnon-virtual-dtor -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wswitch-default -Wundef -Wno-unused
CPP_FLAGS_OLD ?= -fmax-errors=2 -Wall -Wextra -Wpedantic

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CMP_FLAGS ?= $(CPP_FLAGS) -MMD -MP

# linking files
$(BIN_DIR)/$(TARGET_EXEC): $(OBJS)
	@echo Linking object files..
	@$(MKDIR_P) $(dir $@)
	@$(CXX) $(OBJS) -o $@ $(LDFLAGS)




# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	@$(MKDIR_P) $(dir $@)
	@$(CXX) $(CMP_FLAGS) -c $< -o $@
	@echo $(CXX) -Wall -Wextra ... -c $< -o $@
	@echo -e "-- Static Analysis -- "
	@cppclean $<
	@cppcheck $<

flags:
	@echo Flags used for building project:
	@echo $(CMP_FLAGS)

.PHONY: clean

analyse:
	@cppclean $(SRC_DIRS)
	cppcheck --enable=all --inconclusive --library=posix $(SRC_DIRS)

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
