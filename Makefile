CXX = g++-14
CXXFLAGS = -std=c++23 -Wall -Wextra -Wpedantic -I/usr/local/include -Iinclude \
		   -Wshadow -Wconversion -Wsign-conversion \
		   -Wformat=2 -Wduplicated-branches -Wduplicated-cond -Wnull-dereference

SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests
EXEC = $(BUILD_DIR)/tsp_parser
TEST_EXEC = $(BUILD_DIR)/test_tsp

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

.PHONY: all clean test valgrind

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

TEST_SOURCES = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(TEST_SOURCES))
LIB_OBJECTS = $(filter-out $(BUILD_DIR)/main.o, $(OBJECTS))

test: $(TEST_EXEC)
	./$(TEST_EXEC)

$(TEST_EXEC): $(LIB_OBJECTS) $(TEST_OBJECTS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lgtest -lgtest_main -lpthread

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

valgrind: $(TEST_EXEC)
	valgrind --leak-check=full --error-exitcode=1 ./$(TEST_EXEC)

clean:
	rm -rf $(BUILD_DIR)
