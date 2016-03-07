ifndef CXX
    # Checking if either of these commands exists
    CLANG := $(shell clang++ --version)
    MINGW := $(shell mingw32-g++ --version)

    # Picking a compiler depending on what is available
    ifdef CLANG:
        CXX = clang++
    else
        ifdef MINGW
            CXX = mingw32-g++
        else
            CXX = g++
        endif
    endif
endif


CXX_FLAGS = -Wall -Werror -Wextra -pedantic -std=c++11 -O3
CPP_FLAGS =
LD_FLAGS = -lgtest

ifeq ($(CXX), mingw32-g++)
    CPP_FLAGS += -D__NO_INLINE__
endif

# Adapted from http://stackoverflow.com/a/12099167/3076272
ifneq ($(OS), Windows_NT)
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        LD_FLAGS += -lpthread
    endif
endif

VALGRIND_CMD = valgrind --leak-check=full --show-reachable=yes

SUBJECTS = octree
TARGETS = $(patsubst %,test_%, $(SUBJECTS))
CLEAN_EXTENSIONS = *.o *.gch

all: tests

test_%: test_%.o
	$(CXX) $(CXX_FLAGS) $(CPP_FLAGS) $< -o $@ $(LD_FLAGS)

test_%.o: tests/test_%.cc structures/%.h
	$(CXX) $(CXX_FLAGS) $(CPP_FLAGS) $< -c

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
