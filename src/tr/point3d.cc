#include "tr/point3d.h"

#include <cmath>
#include <iostream>

namespace tr {

bool Point3d::isNaN() const {
  return std::isnan(x) && std::isnan(y) && std::isnan(z);
}

bool Point3d::hasNaN() const {
  return std::isnan(x) || std::isnan(y) || std::isnan(z);
}

bool Point3d::operator==(const Point3d& other) const {
  // TODO - give some leeway cuz floating point
  // NOTE - we don't compare equal if nans, must specifically compare the values of isNaN()
  return (x == other.x && y == other.y && z == other.z) && (isNaN() == other.isNaN());
}

bool Point3d::operator!=(const Point3d& other) const {
  return !operator==(other);
}

std::ostream& operator<<(std::ostream& out, const Point3d& rhs) {
	out << "{" << rhs.x << ", " << rhs.y << ", " << rhs.z << "}";
	return out;
}

}
