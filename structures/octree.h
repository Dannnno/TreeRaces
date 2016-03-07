/*
    file - octree.h

    Templated implementation of an octree for a cpu

 */


#ifndef OCTREE_CPU_H
#define OCTREE_CPU_H

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <limits>
#include <utility>
#include <type_traits>


using Point = std::array<double, 3>;


// An axis aligned bounding boxed (AABB)
struct BoundingBox {
  bool contains(const BoundingBox& other) const {
    return xlo <= other.xlo && xhi >= other.xhi &&
           ylo <= other.ylo && yhi >= other.yhi &&
           zlo <= other.zlo && zhi >= other.zhi;
  }

  bool contains(const std::array<double, 3>& point) const {
    return xlo <= point[0] && xhi >= point[0] &&
           ylo <= point[1] && yhi >= point[1] &&
           zlo <= point[2] && zhi >= point[2];
  }

  bool overlap(const BoundingBox& other, BoundingBox* out) const {
    // trivial cases
    if (contains(other)) {
      *out = other;
      return true;
    } else if (other.contains(*this)) {
      *out = *this;
      return true;
    } 

    // Check if there is no intersection
    if (xhi < other.xlo || xlo > other.xhi ||
        yhi < other.ylo || ylo > other.yhi ||
        zhi < other.zlo || zlo > other.zhi) {
      *out = BoundingBox{
          std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), 
          std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), 
          std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN()
      };
      return false;
    }

    // Actually calculate the bounds
    double upperX = std::min(xhi, other.xhi);
    double upperY = std::min(yhi, other.yhi);
    double upperZ = std::min(zhi, other.zhi);

    double lowerX = std::max(xlo, other.xlo);
    double lowerY = std::max(ylo, other.ylo);
    double lowerZ = std::max(zlo, other.zlo);

    *out = BoundingBox{upperX, lowerX, upperY, lowerY, upperZ, lowerZ};
    return true;
  }

  std::array<BoundingBox, 8> partition() const {
    double xmid = (xhi - xlo) / 2.;
    double ymid = (yhi - ylo) / 2.;
    double zmid = (zhi - zlo) / 2.;

    std::array<BoundingBox, 8> ret{{
      BoundingBox{xmid, xlo, ymid, ylo, zmid, zlo}, // bottom left front
      BoundingBox{xhi, xmid, ymid, ylo, zmid, zlo}, // bottom right front
      BoundingBox{xmid, xlo, yhi, ymid, zmid, zlo}, // bottom left back
      BoundingBox{xhi, xmid, yhi, ymid, zmid, zlo}, // bottom right back
      BoundingBox{xmid, xlo, ymid, ylo, zhi, zmid}, // top left front
      BoundingBox{xhi, xmid, ymid, ylo, zhi, zmid}, // top right front
      BoundingBox{xmid, xlo, yhi, ymid, zhi, zmid}, // top left back
      BoundingBox{xhi, xmid, yhi, ymid, zhi, zmid}  // top right back
    }};
    return ret;
  }

  double xhi, xlo, yhi, ylo, zhi, zlo;
};

const BoundingBox initial = BoundingBox{
    std::numeric_limits<double>::min(), std::numeric_limits<double>::max(),
    std::numeric_limits<double>::min(), std::numeric_limits<double>::max(),
    std::numeric_limits<double>::min(), std::numeric_limits<double>::max()
};

template <typename InputIterator, class PointExtractor, 
          size_t max_per_node = 16, size_t max_depth = 100>
class Octree {
 public:
  using tree_type = Octree<InputIterator, PointExtractor, max_per_node, max_depth>;

  Octree(): functor_(PointExtractor()), head_(nullptr), size_(0) {}

  Octree(InputIterator begin, InputIterator end)
      : Octree(begin, end, PointExtractor()) { }

  Octree(InputIterator begin, InputIterator end, PointExtractor f)
      : functor_(f), head_(nullptr), size_(0) {

    std::vector<std::pair<InputIterator, Point>> v;
    v.reserve(std::distance(begin, end));

    for (auto it = begin; it != end; ++it) {
      v.push_back(std::pair<InputIterator, Point>(it, functor_(*it)));
    }
    
    size_ = v.size();
    head_ = new Node(v);
  }

  Octree(const tree_type& rhs); 

  template <size_t max_per_node_>
  Octree(const Octree<InputIterator, PointExtractor, max_per_node_, max_depth>& rhs);
  
  template <size_t max_depth_>
  Octree(const Octree<InputIterator, PointExtractor, max_per_node, max_depth_>& rhs);
  
  template <size_t max_per_node_, size_t max_depth_>
  Octree(const Octree<InputIterator, PointExtractor, max_per_node_, max_depth_>& rhs);
  
  Octree(tree_type&& rhs)
    : functor_(rhs.functor), head_(rhs.head_), size_(rhs.size_) { }

  void swap(const tree_type& rhs) {
    std::swap(head_, rhs.head_);
    std::swap(functor_, rhs.functor_);
    std::swap(size_, rhs.size_);
  }

  tree_type& operator=(tree_type rhs) {
    swap(rhs);
    return *this;
  }

  tree_type& operator=(tree_type&& rhs) {
    swap(rhs);
    return *this;
  }

  ~Octree() {
    delete head_;
  }
 
 private:  
  class Node;

  enum NodeContents {
    LEAF = 1,
    MAX_DEPTH_LEAF = 2,
    INTERNAL = 4
  };

  struct LeafNodeValues {
    std::array<std::pair<InputIterator, Point>, max_per_node> values_;
    size_t size_;
  };

  using childNodeArray = std::array<Node*, 8>;
  using maxItemNode = std::vector<std::pair<InputIterator, Point>>;

  class Node {
   public:    
    Node(const std::vector<std::pair<InputIterator, Point>>& input_values)
      : Node(input_values, Node::getExtrema(input_values), 0) { }

    Node(const std::vector<std::pair<InputIterator, Point>>& input_values, 
         const BoundingBox& box,
         size_t current_depth) : value_(nullptr), extrema_(box)  {
      if (current_depth > max_depth) {
        init_max_depth_leaf(input_values);
      } else if (input_values.size() <= max_per_node) {
        init_leaf(input_values);
      } else {
        init_internal(input_values, current_depth);
      }
    }

    ~Node() {
      if (tag_ == NodeContents::INTERNAL) {
        childNodeArray* children = static_cast<childNodeArray*>(value_);
        for (size_t i = 0; i < 8; ++i) {
          delete children[0][i];
          children[0][i] = nullptr;
        }
        delete children;
      } else if (tag_ == NodeContents::LEAF) {
        delete static_cast<LeafNodeValues*>(value_);
      } else if (tag_ == NodeContents::MAX_DEPTH_LEAF) {
        delete static_cast<maxItemNode*>(value_);
      }
      
      value_ = nullptr;
    }

   private:
    void* value_;
    BoundingBox extrema_;
    NodeContents tag_;

    void init_max_depth_leaf(
        const std::vector<std::pair<InputIterator, Point>>& input_values) {  
      value_ = new std::vector<std::pair<InputIterator, Point>>(input_values);
      tag_ = NodeContents::MAX_DEPTH_LEAF;
    }

    void init_leaf(
        const std::vector<std::pair<InputIterator, Point>>& input_values)  {
      std::array<std::pair<InputIterator, Point>, max_per_node> a;
      std::copy(input_values.begin(), input_values.end(), a.begin());
      value_ = new LeafNodeValues{a, input_values.size()};
      tag_ = NodeContents::LEAF;
    }
    
    void init_internal(
        const std::vector<std::pair<InputIterator, Point>>& input_values,
        size_t current_depth)  {
      std::array<std::vector<std::pair<InputIterator, Point>>, 8> childVectors;
      std::array<BoundingBox, 8> boxes = extrema_.partition();
      std::array<Node*, 8> children;

      for (unsigned child = 0; child < 8; ++child) {
        std::vector<std::pair<InputIterator, Point>>& childVector = childVectors[child];
        childVector.reserve(input_values.size() / 8);

        std::copy_if(
          input_values.begin(), 
          input_values.end(), 
          std::back_inserter(childVector),
          [&boxes, child](const std::pair<InputIterator, Point>& element) -> bool {
            Point p = std::get<1>(element);
            return boxes[child].contains(p);
          }
        );

        children[child] = childVector.empty()
            ? nullptr
            : new Node(childVector, boxes[child], ++current_depth);
      }

      value_ = new std::array<Node*, 8>(children);
      tag_ = NodeContents::INTERNAL;
    }

    static
    BoundingBox getExtrema(
        const std::vector<std::pair<InputIterator, Point>>& input_values) {
      BoundingBox b = initial;
      for (auto value : input_values) {
        const std::array<double, 3>& point = std::get<1>(value);

        if (point[0] < b.xlo) {
          b.xlo = point[0];
        } else if (point[0] > b.xhi) {
          b.xhi = point[0];
        }

        if (point[1] < b.ylo) {
          b.ylo = point[1];
        } else if (point[1] > b.yhi) {
          b.yhi = point[1];
        }

        if (point[2] < b.zlo) {
          b.zlo = point[2];
        } else if (point[2] > b.zhi) {
          b.zhi = point[2];
        }
      }

      return b;
    }
  };

  PointExtractor functor_;
  Node* head_;
  size_t size_;
};


#endif // DEFINED OCTREE_CPU_H