// Stupid mingw port of gtest 
#ifdef MINGW_COMPILER
	#ifdef __STRICT_ANSI__
	#undef __STRICT_ANSI__
	#endif
#endif

#include <limits>
#include <iostream>

#include "gtest/gtest.h"
#include "../structures/point3d.h"


TEST(Point3dComparisons, EqualityEqual) {
	Point3d p1{1, 2, 3};
	Point3d p2{1, 2, 3};

	EXPECT_EQ(p1, p2) << "Comparing " << p1 << "with " << p2 << std::endl;
}

TEST(Point3dComparisons, EqualityEqualNaN) {
	Point3d p1{std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()};
	Point3d p2{std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()};

	EXPECT_EQ(p1, p2) << "Comparing " << p1 << "with " << p2 << std::endl;
}

TEST(Point3dComparisons, EqualityInEqual) {
	Point3d p1{1, 2, 3};
	Point3d p2{1, 3, 3};

	EXPECT_NE(p1, p2) << "Comparing " << p1 << "with " << p2 << std::endl;
}

TEST(Point3dComparisons, EqualityInEqualNaN) {
	Point3d p1{1, std::numeric_limits<double>::quiet_NaN(), 3};
	Point3d p2{1, 3, 3};

	EXPECT_NE(p1, p2) << "Comparing " << p1 << "with " << p2 << std::endl;
}
