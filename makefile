TARGET_EXEC ?= main

BIN_DIR ?= ./bin
BUILD_DIR ?= ./objects
SRC_DIRS ?= ./src

CXX=g++

SRCS := $(shell find $(SRC_DIRS) -name *.cpp)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)


CPPFLAGS ?= -fmax-errors=2 -Wall -Wextra -Wpedantic

# linking files
$(BIN_DIR)/$(TARGET_EXEC): $(OBJS)
	@echo Linking object files..
	@$(MKDIR_P) $(dir $@)
	@$(CXX) $(OBJS) -o $@ $(LDFLAGS)
	@echo -e "Done."

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	@$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

MKDIR_P ?= mkdir -p
