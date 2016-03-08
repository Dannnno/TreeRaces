#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "boundingbox.h"

#include <array>
#include <limits>
#include <cmath>
#include <iostream>

// An axis aligned bounding boxed (AABB)
struct BoundingBox {
  bool contains(const BoundingBox& other) const;

  bool contains(const std::array<double, 3>& point) const;

  bool overlap(const BoundingBox& other, BoundingBox* out) const;

  std::array<BoundingBox, 8> partition() const;

  bool operator==(const BoundingBox& rhs) const;

  bool operator!=(const BoundingBox& rhs) const;

  friend
  std::ostream& operator<<(std::ostream& stream, const BoundingBox& rhs);

  double xhi, xlo, yhi, ylo, zhi, zlo;
};

const BoundingBox initial = BoundingBox{
    std::numeric_limits<double>::min(), std::numeric_limits<double>::max(),
    std::numeric_limits<double>::min(), std::numeric_limits<double>::max(),
    std::numeric_limits<double>::min(), std::numeric_limits<double>::max()
};

const BoundingBox invalid = BoundingBox{
    std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), 
    std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), 
    std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()
};


#endif // defined BOUNDINGBOX_H