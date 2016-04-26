// Stupid mingw port of gtest 
#ifdef MINGW_COMPILER
  #ifdef __STRICT_ANSI__
  #undef __STRICT_ANSI__
  #endif
#endif

#ifndef TEST_HELPERS_H_DEFINED
#define TEST_HELPERS_H_DEFINED

#include "tr/point3d.h"
#include "tr/boundingbox.h"
#include <vector>
#include "gtest/gtest.h"

using tr::Point3d;
using tr::BoundingBox;

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
    BoundingBox allBox;

  	OctreeTest() : data(std::vector<ValuePoint<int>>(100)) {}

  	virtual ~OctreeTest() = default;

  	virtual void SetUp() {
	    for (double i = 0; i < data.size(); ++i) {
	        data[i].dimensions_ = Point3d{ i, i+1, i+2 };
	        data[i].value_ = static_cast<int>(i);
          allBox.maxes_.x = allBox.maxes_.y = allBox.maxes_.z = i + 2;
	    }
      allBox.mins_.x = allBox.mins_.y = allBox.mins_.z = 0;
  	}
};

#endif // defined TEST_HELPERS_H_DEFINED
