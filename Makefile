ifndef CXX
	CXX = g++
endif


CXX_FLAGS = -Wall -Werror -Wextra -pedantic -std=c++11
LD_FLAGS = -lgtest

VALGRIND_CMD = valgrind --leak-check=full --show-reachable=yes 

# Adapted from http://stackoverflow.com/a/12099167/3076272
ifneq ($(OS), Windows_NT)
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        LD_FLAGS += -lpthread
    endif
endif

SUBJECTS = octree
TARGETS = $(patsubst %,test_%, $(SUBJECTS))
CLEAN_EXTENSIONS = *.o *.gch

all: tests

test_%: test_%.o
	$(CXX) $(CXX_FLAGS) $< -o $@ $(LD_FLAGS)

test_%.o: tests/test_%.cc structures/%.h
	$(CXX) $(CXX_FLAGS) $< -c

objects: $(patsubst %,test_%.o, $(SUBJECTS))

tests: $(patsubst %,test_%, $(SUBJECTS))

run_tests: tests
ifeq ($(OS), Windows_NT)
	$(foreach test, $(TARGETS), @.\$(test).exe;)
else
	$(foreach test, $(TARGETS), @./$(test);)
endif

run_valgrind_tests: tests
ifeq ($(OS), Windows_NT)
	$(foreach test, $(TARGETS), @$(VALGRIND_CMD) .\$(test).exe;)
else
	$(foreach test, $(TARGETS), @$(VALGRIND_CMD) ./$(test);)
endif

clean:
	rm -rf $(CLEAN_EXTENSIONS) $(TARGETS)

again: clean all
