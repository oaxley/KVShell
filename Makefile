# Makefile

ifdef RELEASE
	CC=g++ -O3
else
	CC=g++ -g3 -DDEBUG=1
endif

CFLAGS := -std=c++17
LDFLAGS := -lpthread -ldl -lsqlite3 -lSQLiteCpp -L src/SQLiteCpp/build

# directories
BUILD_DIR := build
SRC_DIR := src/

EXCLUDES := src/tomlplusplus src/SQLiteCpp
INCLUDES := -I src/tomlplusplus/include -I src/SQLiteCpp/include

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

