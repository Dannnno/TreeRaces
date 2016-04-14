# If we aren't doing CI
ifndef TRAVIS 
    # Checking if either of these commands exists
    CLANG := $(shell clang++ --version)
    MINGW := $(shell mingw32-g++ --version)

    # Picking a compiler depending on what is available
    ifdef CLANG
        CXX = clang++
    else
        ifdef MINGW
            CXX = mingw32-g++
        else
            CXX = g++
        endif
    endif
endif

CXX_FLAGS = -Wall -Werror -Wextra -pedantic -std=c++11
TRAVIS ?= DEBUG

ifdef TRAVIS
    CXX_FLAGS += -g
    ifeq ($(CXX), clang++)
        CXX_FLAGS += -O1
    else
        CXX_FLAGS += -Og
    endif
else
    CXX_FLAGS += -O3
endif

host = $(shell hostname)
muddhosts = knuth shuffler shadowfax

ifdef host
    ifeq ($(host), $(filter $(host), $(muddhosts)))
        GTEST_DIR = /cs/cs70/googletest/googletest
        CPP_FLAGS += -isystem $(GTEST_DIR)/include -isystem $(GTEST_DIR)
        LD_FLAGS += $(GTEST_DIR)/src/gtest-all.cc 
    endif
endif

LD_FLAGS ?= -lgtest

ifdef COVERAGE
    CXX_FLAGS += -fprofile-arcs -ftest-coverage
    LD_FLAGS += -lgcov
endif

ifeq ($(CXX), mingw32-g++)
    CPP_FLAGS += -D__NO_INLINE__ -DMINGW_COMPILER
endif

# Adapted from http://stackoverflow.com/a/12099167/3076272
ifneq ($(OS), Windows_NT)
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        LD_FLAGS += -lpthread
    endif
endif

VALGRIND_CMD = valgrind --leak-check=full --error-exitcode=1

HEADER_SUBJECTS = boundingbox octree pointerless_octree point3d
SUBJECTS = boundingbox point3d
CLEAN_EXTENSIONS = *.o *.gch *.gcda *.gcno

all: all_tests

test_%.o: tests/test_%.cc structures/%.h
	$(CXX) $(CXX_FLAGS) $(CPP_FLAGS) $< -c

structures/%.o: structures/%.cc structures/%.h
	$(CXX) $(CXX_FLAGS) $(CPP_FLAGS) $< -c -o $@

all_tests: $(patsubst %,test_%.o, $(HEADER_SUBJECTS)) \
           $(patsubst %,structures/%.o, $(SUBJECTS)) run_tests.cc \
           tests/test_helpers.h structures/inneriterator.h
	$(CXX) $(CXX_FLAGS) $(CPP_FLAGS) $(filter %.o %.cc,$^) -o $@ $(LD_FLAGS) 

run_tests: all_tests
ifeq ($(OS), Windows_NT)
	.\all_tests.exe
else
	./all_tests
endif

run_valgrind_tests: all_tests
	$(VALGRIND_CMD) ./all_tests

coverage:
	find -name *.gcda ../
	cp -R ../structures/* ./structures/
	cp -R ../tests/* ./tests/
	gcovr -rp .
	gcovr -rpb .

clean:
	rm -rf $(CLEAN_EXTENSIONS) all_tests

again: clean all
