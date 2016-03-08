// Stupid mingw port of gtest 
#ifdef MINGW_COMPILER
	#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
	#endif
#endif

#include "../structures/boundingbox.h"

#include "gtest/gtest.h"

using std::array;

using Point = array<double, 3>;

TEST(BoundingBox, ContainsOtherBoxEqual) {
	BoundingBox first, second;
	first = BoundingBox{10, 0, 10, 0, 10, 0};
	second = first;
	EXPECT_TRUE(first.contains(second));
	EXPECT_TRUE(second.contains(first));
}

TEST(BoundingBox, ContainsOtherBoxOutside) {
	BoundingBox first{10, 0, 10, 0, 10, 0};
	BoundingBox second{20, 10, 20, 10, 20, 10};
	EXPECT_FALSE(first.contains(second));
	EXPECT_FALSE(second.contains(first));
}

TEST(BoundingBox, ContainsOtherBoxInside) {
	BoundingBox first{10, 0, 10, 0, 10, 0};
	BoundingBox second{5, 2, 5, 2, 5, 2};
	EXPECT_TRUE(first.contains(second));
	EXPECT_FALSE(second.contains(first));
}

TEST(BoundingBox, ContaintsOtherBoxOverlap) {
	BoundingBox first{10, 0, 10, 0, 10, 0};
	BoundingBox second{11, 1, 10, 0, 10, 0};
	EXPECT_FALSE(first.contains(second));
	EXPECT_FALSE(second.contains(first));
}

TEST(BoundingBox, ContainsPointInside) {
	BoundingBox first{10, 0, 10, 0, 10, 0};
	Point p{{1, 2, 3}};
	EXPECT_TRUE(first.contains(p));
}

TEST(BoundingBox, ContainsPointOutside) {
	BoundingBox first{10, 0, 10, 0, 10, 0};
	Point p{{11, 2, 3}};
	EXPECT_FALSE(first.contains(p));
}

TEST(BoundingBox, ContainsPointOnLowEdge) {
	BoundingBox first{10, 0, 10, 0, 10, 0};
	Point p{{0, 0, 0}};
	EXPECT_TRUE(first.contains(p));
}

TEST(BoundingBox, ContainsPointOnHighEdge) {
	BoundingBox first{10, 0, 10, 0, 10, 0};
	Point p{{10, 10, 10}};
	EXPECT_FALSE(first.contains(p));
}

TEST(BoundingBox, OverlapOtherBoxEqual) {
	BoundingBox first, second, out;
	first = BoundingBox{10, 0, 10, 0, 10, 0};
	second = first;
	EXPECT_TRUE(first.overlap(second, &out));
	EXPECT_EQ(first, out);
}

TEST(BoundingBox, OverlapOtherBoxOutside) {
	BoundingBox first{9, 0, 9, 0, 9, 9};
	BoundingBox second{20, 10, 20, 10, 20, 10};
	BoundingBox out;
	EXPECT_FALSE(first.overlap(second, &out));
	EXPECT_EQ(out, invalid);
}

TEST(BoundingBox, OverlapOtherBoxInside) {
	BoundingBox first{10, 0, 10, 0, 10, 0};
	BoundingBox second{5, 2, 5, 2, 5, 2};
	BoundingBox out1, out2;
	EXPECT_TRUE(first.overlap(second, &out1));
	EXPECT_TRUE(second.overlap(first, &out2));
	EXPECT_EQ(out1, out2);
}

TEST(BoundingBox, OverlapPartial) {
	BoundingBox first{10, 0, 10, 0, 10, 0};
	BoundingBox second{11, 1, 10, 0, 10, 0};
	BoundingBox out;
	BoundingBox expected{10, 1, 10, 0, 10, 0};
	EXPECT_TRUE(first.overlap(second, &out));
	EXPECT_EQ(out, expected);
}

TEST(BoundingBox, Partition) {
	BoundingBox extrema{100, 0, 100, 0, 100, 0};
	array<BoundingBox, 8> partitions = extrema.partition();
	array<BoundingBox, 8> expectedPartitions{{
		BoundingBox{50, 0, 50, 0, 50, 0},
		BoundingBox{100, 50, 50, 0, 50, 0},
		BoundingBox{50, 0, 100, 50, 50, 0},
		BoundingBox{100, 50, 100, 50, 50, 0},
		BoundingBox{50, 0, 50, 0, 100, 50},
		BoundingBox{100, 50, 50, 0, 100, 50},
		BoundingBox{50, 0, 100, 50, 100, 50},
		BoundingBox{100, 50, 100, 50, 100, 50}
	}};
	for (unsigned i = 0; i < 8; ++i) {
		EXPECT_EQ(partitions[i], expectedPartitions[i]);
	}
}
