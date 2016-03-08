// Stupid mingw port of gtest 
#ifdef MINGW_COMPILER
	#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
	#endif
#endif

#include "../structures/octree.h"

#include <vector>
#include "gtest/gtest.h"

using std::vector;
using std::array;

template <typename T>
struct ValuePoint {
  array<double, 3> dimensions_;
  T value_;
};

template <typename T>
struct ExamplePointExtractor {
  array<double, 3> operator()(const ValuePoint<T>& p) {
    return p.dimensions_;
  }
};

TEST(OctreeConstructor, DefaultConstructor) {
	vector<ValuePoint<int>> data(100);
	for (double i = 0; i < 100; ++i) {
		data[i].dimensions_ = array<double, 3>{{ i, i+1, i+2 }};
		data[i].value_ = static_cast<int>(i);
	}
	Octree<std::vector<ValuePoint<int>>::iterator, ExamplePointExtractor<int>> o(data.begin(), data.end());
	EXPECT_EQ(o.size(), 100);
}
