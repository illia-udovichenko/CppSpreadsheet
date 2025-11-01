CC = g++
CFLAGS = -std=c++23 -Wall -pedantic -g
LDFLAGS = -L./parser/x86_64-linux-gnu -lexpression_parser

# Directories
SRC_DIR = src
TEST_DIR = tests

# Automatically find all .cpp files in src and tests
SRCS = $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(TEST_DIR)/*.cpp)

# Generate .o file list from .cpp
OBJS = $(SRCS:.cpp=.o)

# Executable name
EXEC = SpreadSheet

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

# Generic compilation rule
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)