// Stupid mingw port of gtest 
#ifdef MINGW_COMPILER
    #ifdef __STRICT_ANSI__
    #undef __STRICT_ANSI__
    #endif
#endif

#include "../structures/octree.h"

#include <vector>
#include <iterator>
#include "gtest/gtest.h"

using std::vector;
using std::array;

template <typename T>
struct ValuePoint {
  array<double, 3> dimensions_;
  T value_;
};

// I don't know why, but this won't compmile unless it looks like this
template <typename T>
bool operator==(const ValuePoint<T>& lhs, const ValuePoint<T>& rhs) {
  return lhs.dimensions_ == rhs.dimensions_ && lhs.value_ == rhs.value_;
}

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

TEST(OctreeConstructor, DefaultConstructorMaxDepthSmall) {
    vector<ValuePoint<int>> data(100);
    for (double i = 0; i < 100; ++i) {
        data[i].dimensions_ = array<double, 3>{{ i, i+1, i+2 }};
        data[i].value_ = static_cast<int>(i);
    }
    Octree<std::vector<ValuePoint<int>>::iterator, ExamplePointExtractor<int>, 16, 3> o(data.begin(), data.end());
    EXPECT_EQ(o.size(), 100);
}

TEST(OctreeConstructor, DefaultConstructorMaxPerNodeLarge) {
    vector<ValuePoint<int>> data(100);
    for (double i = 0; i < 100; ++i) {
        data[i].dimensions_ = array<double, 3>{{ i, i+1, i+2 }};
        data[i].value_ = static_cast<int>(i);
    }
    Octree<std::vector<ValuePoint<int>>::iterator, ExamplePointExtractor<int>, 100> o(data.begin(), data.end());
    EXPECT_EQ(o.size(), 100);
}

#if 0
TEST(OctreePointSearch, SearchNonExistantPoint) {   
    vector<ValuePoint<int>> data(100);
    for (double i = 0; i < 100; ++i) {
        data[i].dimensions_ = array<double, 3>{{ i, i+1, i+2 }};
        data[i].value_ = static_cast<int>(i);
    }
    Octree<std::vector<ValuePoint<int>>::iterator, ExamplePointExtractor<int>> o(data.begin(), data.end());
    vector<vector<ValuePoint<int>>::iterator> output;

    std::back_insert_iterator<vector<vector<ValuePoint<int>>::iterator>> i = std::back_inserter(output);
    Point p{{-1., -2., -3.}};
    EXPECT_FALSE(o.search(p, i));
}

TEST(OctreePointSearch, SearchExistingPoint) {
    vector<ValuePoint<int>> data(100);
    for (double i = 0; i < 100; ++i) {
        data[i].dimensions_ = array<double, 3>{{ i, i+1, i+2 }};
        data[i].value_ = static_cast<int>(i);
    }
    Octree<std::vector<ValuePoint<int>>::iterator, ExamplePointExtractor<int>> o(data.begin(), data.end());
    vector<vector<ValuePoint<int>>::iterator> output;

    std::back_insert_iterator<vector<vector<ValuePoint<int>>::iterator>> i = std::back_inserter(output);
    Point p{{0., 1., 2.}};
    EXPECT_TRUE(o.search(p, i));
    EXPECT_EQ(**output.cbegin(), *data.cbegin());
}

TEST(OctreePointSearch, SearchNonExistantPointMaxDepthSmall) {
    vector<ValuePoint<int>> data(100);
    for (double i = 0; i < 100; ++i) {
        data[i].dimensions_ = array<double, 3>{{ i, i+1, i+2 }};
        data[i].value_ = static_cast<int>(i);
    }
    Octree<std::vector<ValuePoint<int>>::iterator, ExamplePointExtractor<int>, 16, 3> o(data.begin(), data.end());
    vector<vector<ValuePoint<int>>::iterator> output;

    std::back_insert_iterator<vector<vector<ValuePoint<int>>::iterator>> i = std::back_inserter(output);
    Point p{{-1., -2., -3.}};
    EXPECT_FALSE(o.search(p, i));
}

TEST(OctreePointSearch, SearchExistingPointMaxDepthSmall) {
    vector<ValuePoint<int>> data(100);
    for (double i = 0; i < 100; ++i) {
        data[i].dimensions_ = array<double, 3>{{ i, i+1, i+2 }};
        data[i].value_ = static_cast<int>(i);
    }
    Octree<std::vector<ValuePoint<int>>::iterator, ExamplePointExtractor<int>, 16, 3> o(data.begin(), data.end());
    vector<vector<ValuePoint<int>>::iterator> output;

    std::back_insert_iterator<vector<vector<ValuePoint<int>>::iterator>> i = std::back_inserter(output);
    Point p{{0., 1., 2.}};
    EXPECT_TRUE(o.search(p, i));
    EXPECT_EQ(**output.cbegin(), *data.cbegin());
}

TEST(OctreePointSearch, SearchNonExistantPointMaxPerNodeLarge) {
    vector<ValuePoint<int>> data(100);
    for (double i = 0; i < 100; ++i) {
        data[i].dimensions_ = array<double, 3>{{ i, i+1, i+2 }};
        data[i].value_ = static_cast<int>(i);
    }
    Octree<std::vector<ValuePoint<int>>::iterator, ExamplePointExtractor<int>, 100> o(data.begin(), data.end());
    vector<vector<ValuePoint<int>>::iterator> output;

    std::back_insert_iterator<vector<vector<ValuePoint<int>>::iterator>> i = std::back_inserter(output);
    Point p{{-1., -2., -3.}};
    EXPECT_FALSE(o.search(p, i));
}

TEST(OctreePointSearch, SearchExistingPointMaxPerNodeLarge) {
    vector<ValuePoint<int>> data(100);
    for (double i = 0; i < 100; ++i) {
        data[i].dimensions_ = array<double, 3>{{ i, i+1, i+2 }};
        data[i].value_ = static_cast<int>(i);
    }
    Octree<std::vector<ValuePoint<int>>::iterator, ExamplePointExtractor<int>, 100> o(data.begin(), data.end());
    vector<vector<ValuePoint<int>>::iterator> output;

    std::back_insert_iterator<vector<vector<ValuePoint<int>>::iterator>> i = std::back_inserter(output);
    Point p{{0., 1., 2.}};
    EXPECT_TRUE(o.search(p, i));
    EXPECT_EQ(**output.cbegin(), *data.cbegin());
}


TEST(OctreeBoxSearch, SearchExternalBox) {
    vector<ValuePoint<int>> data(100);
    for (double i = 0; i < 100; ++i) {
        data[i].dimensions_ = array<double, 3>{{ i, i+1, i+2 }};
        data[i].value_ = static_cast<int>(i);
    }
    Octree<std::vector<ValuePoint<int>>::iterator, ExamplePointExtractor<int>> o(data.begin(), data.end());
    vector<vector<ValuePoint<int>>::iterator> output;

    BoundingBox box{-10, -5, -10, -5, -10, -5};
    EXPECT_FALSE(o.search(box, output));
}

#endif

TEST(OctreeBoxSearch, SearchNotPresent) {
    vector<ValuePoint<int>> data(100);
    for (double i = 0; i < 100; ++i) {
        data[i].dimensions_ = array<double, 3>{{ i, i+1, i+2 }};
        data[i].value_ = static_cast<int>(i);
    }
    Octree<vector<ValuePoint<int>>::const_iterator, ExamplePointExtractor<int>> o(data.cbegin(), data.cend());
    vector<vector<ValuePoint<int>>::const_iterator> outputValues;
    auto outputIterator = back_inserter(outputValues);
    BoundingBox box{-10, -5, -10, -5, -10, -5};
    EXPECT_FALSE(o.search(box, outputIterator));
}

TEST(OctreeBoxSearch, SearchPresent) {
    vector<ValuePoint<int>> data(100);
    for (double i = 0; i < 100; ++i) {
        data[i].dimensions_ = array<double, 3>{{ i, i+1, i+2 }};
        data[i].value_ = static_cast<int>(i);
    }
    Octree<vector<ValuePoint<int>>::const_iterator, ExamplePointExtractor<int>> o(data.cbegin(), data.cend());
    vector<vector<ValuePoint<int>>::const_iterator> outputValues;
    auto outputIterator = back_inserter(outputValues);
    BoundingBox box{10, 5, 10, 5, 10, 5};

    vector<vector<ValuePoint<int>>::const_iterator> expectedValues;
    for (auto it = data.cbegin(); it != data.cend(); ++it) {
        if (box.contains(ExamplePointExtractor<int>()(*it))) {
            expectedValues.push_back(it);
        }   
    }
    EXPECT_TRUE(o.search(box, outputIterator));
    EXPECT_EQ(expectedValues, outputValues);
}
