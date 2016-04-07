#ifndef TEST_HELPERS_H_DEFINED
#define TEST_HELPERS_H_DEFINED

#include "../structures/point3d.h"
#include <vector>
#include "gtest/gtest.h"

template <typename T>
struct ValuePoint {
  Point3d dimensions_;
  T value_;
};

// I don't know why, but this won't compmile unless it looks like this
template <typename T>
bool operator==(const ValuePoint<T>& lhs, const ValuePoint<T>& rhs) {
  return lhs.dimensions_ == rhs.dimensions_ && lhs.value_ == rhs.value_;
}

template <typename T>
struct ExamplePointExtractor {
  Point3d operator()(const ValuePoint<T>& p) {
    return p.dimensions_;
  }
};

class OctreeTest : public ::testing::Test {
  protected:
  	std::vector<ValuePoint<int>> data;

  	OctreeTest() : data(std::vector<ValuePoint<int>>(100)) {}

  	virtual ~OctreeTest() = default;

  	virtual void SetUp() {
	    for (double i = 0; i < data.size(); ++i) {
	        data[i].dimensions_ = Point3d{ i, i+1, i+2 };
	        data[i].value_ = static_cast<int>(i);
	    }
  	}
};

#endif // defined TEST_HELPERS_H_DEFINED
