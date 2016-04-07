// Stupid mingw port of gtest 
#ifdef MINGW_COMPILER
	#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
	#endif
#endif

#include "../structures/point3d.h"
#include "../structures/boundingbox.h"

#include "gtest/gtest.h"
#include <array>
#include <vector>

using std::array;
using std::vector;

TEST(BoundingBox, FromIterators) {
	vector<Point3d> points;
	points.push_back(Point3d{0,0,0});
	points.push_back(Point3d{10,10,10});
	BoundingBox first = makeBoundingBox(points.begin(), points.end());
	BoundingBox second{{0, 0, 0}, {10, 10, 10}};
	EXPECT_EQ(first, second);
}

TEST(BoundingBox, ContainsOtherBoxEqual) {
	BoundingBox first, second;
	first = BoundingBox{{10, 10, 10}, {0, 0, 0}};
	second = first;
	EXPECT_TRUE(first.contains(second));
	EXPECT_TRUE(second.contains(first));
}

TEST(BoundingBox, ContainsOtherBoxOutside) {
	BoundingBox first{{0, 0, 0}, {10, 10, 10}};
	BoundingBox second{{10, 10, 10}, {20, 20, 20}};
	EXPECT_FALSE(first.contains(second));
	EXPECT_FALSE(second.contains(first));
}

TEST(BoundingBox, ContainsOtherBoxInside) {
	BoundingBox first{{0, 0, 0}, {10, 10, 10}};
	BoundingBox second{{2, 2, 2}, {5, 5, 5}};
	EXPECT_TRUE(first.contains(second));
	EXPECT_FALSE(second.contains(first));
}

TEST(BoundingBox, ContaintsOtherBoxOverlap) {
	BoundingBox first{{0, 0, 0}, {10, 10, 10}};
	BoundingBox second{{1, 0, 0}, {11, 10, 10}};
	EXPECT_FALSE(first.contains(second));
	EXPECT_FALSE(second.contains(first));
}

TEST(BoundingBox, ContainsPoint3dInside) {
	BoundingBox first{{0, 0, 0}, {10, 10, 10}};
	Point3d p{1, 2, 3};
	EXPECT_TRUE(first.contains(p));
}

TEST(BoundingBox, ContainsPoint3dOutside) {
	BoundingBox first{{0, 0, 0}, {10, 10, 10}};
	Point3d p{11, 2, 3};
	EXPECT_FALSE(first.contains(p));
}

TEST(BoundingBox, ContainsPoint3dOnLowEdge) {
	BoundingBox first{{0, 0, 0}, {10, 10, 10}};
	Point3d p{0, 0, 0};
	EXPECT_TRUE(first.contains(p));
}

TEST(BoundingBox, ContainsPoint3dOnHighEdge) {
	BoundingBox first{{0, 0, 0}, {10, 10, 10}};
	Point3d p{10, 10, 10};
	EXPECT_FALSE(first.contains(p));
}

TEST(BoundingBox, OverlapOtherBoxEqual) {
	BoundingBox first, second;
	first = BoundingBox{{0, 0, 0}, {10, 10, 10}};
	second = first;
	EXPECT_EQ(first.overlap(second), first);
}

TEST(BoundingBox, OverlapOtherBoxOutside) {
	BoundingBox first{{0, 0, 9}, {9, 9, 9}};
	BoundingBox second{{10, 10, 10}, {20, 20, 20}};
	EXPECT_EQ(invalidBox, first.overlap(second));
}

TEST(BoundingBox, OverlapOtherBoxInside) {
	BoundingBox first{{0, 0, 0}, {10, 10, 10}};
	BoundingBox second{{2, 2, 2}, {5, 5, 5}};
	EXPECT_EQ(first.overlap(second), second.overlap(first));
	EXPECT_EQ(first.overlap(second), second);
}

TEST(BoundingBox, OverlapPartial) {
	BoundingBox first{{0, 0, 0}, {10, 10, 10}};
	BoundingBox second{{1, 0, 0}, {11, 10, 10}};
	BoundingBox expected{{1, 0, 0}, {10, 10, 10}};
	EXPECT_EQ(expected, first.overlap(second));
}

TEST(BoundingBox, Partition) {
	BoundingBox extrema{{0, 0, 0}, {100, 100, 100}};
	array<BoundingBox, 8> partitions = extrema.partition();
	array<BoundingBox, 8> expectedPartitions{{
		BoundingBox{{0, 0, 0}, {50, 50, 50}},
		BoundingBox{{50, 0, 0}, {100, 50, 50}},
		BoundingBox{{0, 50, 0}, {50, 100, 50}},
		BoundingBox{{50, 50, 0}, {100, 100, 50}},
		BoundingBox{{0, 0, 50}, {50, 50, 100}},
		BoundingBox{{50, 0, 50}, {100, 50, 100}},
		BoundingBox{{0, 50, 50}, {50, 100, 100}},
		BoundingBox{{50, 50, 50}, {100, 100, 100}}
	}};
	for (unsigned i = 0; i < 8; ++i) {
		EXPECT_EQ(partitions[i], expectedPartitions[i]);
	}
}