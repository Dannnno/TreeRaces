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
    CPP_FLAGS += -D__NO_INLINE__ -DMINGW_COMPILER
endif

# Adapted from http://stackoverflow.com/a/12099167/3076272
ifneq ($(OS), Windows_NT)
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        LD_FLAGS += -lpthread
    endif
endif

VALGRIND_CMD = valgrind --leak-check=full

HEADER_SUBJECTS = octree boundingbox
SUBJECTS = boundingbox
CLEAN_EXTENSIONS = *.o *.gch

all: all_tests

test_%.o: tests/test_%.cc structures/%.h
	$(CXX) $(CXX_FLAGS) $(CPP_FLAGS) $< -c

structures/%.o: structures/%.cc structures/%.h
	$(CXX) $(CXX_FLAGS) $(CPP_FLAGS) $< -c -o $@

all_tests: $(patsubst %,test_%.o, $(HEADER_SUBJECTS)) \
           $(patsubst %,structures/%.o, $(SUBJECTS)) run_tests.cc
	$(CXX) $(CXX_FLAGS) $(CPP_FLAGS) $^ -o $@ $(LD_FLAGS) 

run_tests: all_tests
ifeq ($(OS), Windows_NT)
	.\all_tests.exe
else
	./all_tests
endif

run_valgrind_tests: all_tests
	$(VALGRIND_CMD) ./all_tests

clean:
	rm -rf $(CLEAN_EXTENSIONS) all_tests

again: clean all
