CXX = g++-14
CXXFLAGS = -std=c++23 -Wall -Wextra -Wpedantic -I/usr/local/include -Iinclude \
		   -Wshadow -Wconversion -Wsign-conversion \
		   -Wformat=2 -Wduplicated-branches -Wduplicated-cond -Wnull-dereference

SRC_DIR = src
BUILD_DIR = build
EXEC = $(BUILD_DIR)/tsp_parser

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
