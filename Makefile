# Makefile

# generic variables
ifdef DEBUG
	CC=g++ -DDEBUG=1
else
	CC=g++
endif

CFLAGS := -g3 -std=c++17
LDFLAGS := -lpthread -ldl

# directories
BUILD_DIR := build
SRC_DIR := src/

EXCLUDES := src/tomlplusplus
INCLUDES := -I src/tomlplusplus/include

# target
TARGET := $(BUILD_DIR)/kvshell

# source and object files
FIND_SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
SRCS := $(filter-out $(addsuffix /%,$(EXCLUDES)),$(FIND_SRCS))
OBJS := $(SRCS:%.cpp=%.o)

# rules
.PHONY: clean all

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	@mkdir -p $@

$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $^ -o $@

clean:
	@rm -f $(OBJS)
	@rm -f $(TARGET)

