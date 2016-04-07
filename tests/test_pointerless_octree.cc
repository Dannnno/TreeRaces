// Stupid mingw port of gtest 
#ifdef MINGW_COMPILER
    #ifdef __STRICT_ANSI__
    #undef __STRICT_ANSI__
    #endif
#endif

#include "../structures/point3d.h"
#include "../structures/boundingbox.h"
#include "../structures/pointerless_octree.h"
#include "test_helpers.h"

#include <vector>
#include <iterator>
#include "gtest/gtest.h"

using std::vector;
using std::array;

class PointerlessOctreeTest : public OctreeTest {};

TEST_F(PointerlessOctreeTest, DefaultConstructor) {
    PointerlessOctree<std::vector<ValuePoint<int>>::iterator, ExamplePointExtractor<int>> o;
    EXPECT_EQ(o.size(), 0);
}

TEST_F(PointerlessOctreeTest, DefaultConstructorMaxDepthSmall) {
    PointerlessOctree<std::vector<ValuePoint<int>>::iterator, ExamplePointExtractor<int>, 16, 3> o;
    EXPECT_EQ(o.size(), 0);
}

TEST_F(PointerlessOctreeTest, DefaultConstructorMaxPerNodeLarge) {
    PointerlessOctree<std::vector<ValuePoint<int>>::iterator, ExamplePointExtractor<int>, 100> o;
    EXPECT_EQ(o.size(), 0);
}


TEST_F(PointerlessOctreeTest, IteratorConstructor) {
    PointerlessOctree<std::vector<ValuePoint<int>>::iterator, ExamplePointExtractor<int>> o(data.begin(), data.end());
    EXPECT_EQ(o.size(), 100);
}

TEST_F(PointerlessOctreeTest, IteratorConstructorMaxDepthSmall) {
    PointerlessOctree<std::vector<ValuePoint<int>>::iterator, ExamplePointExtractor<int>, 16, 3> o(data.begin(), data.end());
    EXPECT_EQ(o.size(), 100);
}

TEST_F(PointerlessOctreeTest, IteratorConstructorMaxPerNodeLarge) {
    PointerlessOctree<std::vector<ValuePoint<int>>::iterator, ExamplePointExtractor<int>, 100> o(data.begin(), data.end());
    EXPECT_EQ(o.size(), 100);
}

// TEST_F(OctreeBoxSearch, SearchNotPresent) {
//     vector<ValuePoint<int>> data(100);
//     for (double i = 0; i < 100; ++i) {
//         data[i].dimensions_ = Point3d{ i, i+1, i+2 };
//         data[i].value_ = static_cast<int>(i);
//     }
//     Octree<vector<ValuePoint<int>>::const_iterator, ExamplePointExtractor<int>> o(data.cbegin(), data.cend());
//     vector<vector<ValuePoint<int>>::const_iterator> outputValues;
//     auto outputIterator = back_inserter(outputValues);
//     BoundingBox box{{-5, -5, -5}, {-10, -10, -10}};
//     EXPECT_FALSE(o.search(box, outputIterator));
// }

// TEST_F(OctreeBoxSearch, SearchPresent) {
//     vector<ValuePoint<int>> data(100);
//     for (double i = 0; i < 100; ++i) {
//         data[i].dimensions_ = Point3d{ i, i+1, i+2 };
//         data[i].value_ = static_cast<int>(i);
//     }
//     Octree<vector<ValuePoint<int>>::const_iterator, ExamplePointExtractor<int>> o(data.cbegin(), data.cend());
//     vector<vector<ValuePoint<int>>::const_iterator> outputValues;
//     auto outputIterator = back_inserter(outputValues);
//     BoundingBox box{{5, 5, 5}, {10, 10, 10}};

//     vector<vector<ValuePoint<int>>::const_iterator> expectedValues;
//     for (auto it = data.cbegin(); it != data.cend(); ++it) {
//         if (box.contains(ExamplePointExtractor<int>()(*it))) {
//             expectedValues.push_back(it);
//         }   
//     }
//     EXPECT_TRUE(o.search(box, outputIterator));
//     EXPECT_EQ(expectedValues, outputValues);
// }
