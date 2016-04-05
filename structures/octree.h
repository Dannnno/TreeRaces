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

#include "boundingbox.h"

using Point = std::array<double, 3>;

template <typename InputIterator, class PointExtractor, 
          size_t max_per_node = 16, size_t max_depth = 100>
class Octree {
 public:
  using tree_type = Octree<InputIterator, PointExtractor, max_per_node, max_depth>;

  Octree();

  Octree(InputIterator begin, InputIterator end);

  Octree(InputIterator begin, InputIterator end, PointExtractor f);

  Octree(const tree_type& rhs);

  template <size_t max_per_node_>
  Octree(const Octree<InputIterator, PointExtractor, max_per_node_, max_depth>& rhs);
  
  template <size_t max_depth_>
  Octree(const Octree<InputIterator, PointExtractor, max_per_node, max_depth_>& rhs);
  
  template <size_t max_per_node_, size_t max_depth_>
  Octree(const Octree<InputIterator, PointExtractor, max_per_node_, max_depth_>& rhs);
  
  Octree(tree_type&& rhs);

  void swap(tree_type& rhs);

  template <typename OutputIterator>
  bool search(const Point& p, OutputIterator* it) const;

  template <typename OutputIterator>
  void search(const BoundingBox& box, OutputIterator* it) const;

  tree_type& operator=(tree_type rhs);

  tree_type& operator=(tree_type&& rhs);

  ~Octree();

  size_t size() const;
 
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
    Node(const std::vector<std::pair<InputIterator, Point>>& input_values);

    Node(const std::vector<std::pair<InputIterator, Point>>& input_values, 
         const BoundingBox& box,
         size_t current_depth);

    ~Node();

    template <typename OutputIterator>
    bool search(const Point& p, OutputIterator* it);

    template <typename OutputIterator>
    void search(const BoundingBox& box, OutputIterator* it);

   private:
    void* value_;
    BoundingBox extrema_;
    NodeContents tag_;

    void init_max_depth_leaf(const std::vector<std::pair<InputIterator, Point>>& input_values);

    void init_leaf(const std::vector<std::pair<InputIterator, Point>>& input_values);
    
    void init_internal(
        const std::vector<std::pair<InputIterator, Point>>& input_values,
        size_t current_depth);

    unsigned int getOctantIndex(const Point& p);

    static
    BoundingBox getExtrema(const std::vector<std::pair<InputIterator, Point>>& input_values);
  };

  PointExtractor functor_;
  Node* head_;
  size_t size_;
};

// convenience macros to avoid typing so much
#define OCTREE Octree<InputIterator, PointExtractor, max_per_node, max_depth>
#define OCTREE_TEMPLATE typename InputIterator, class PointExtractor, size_t max_per_node, size_t max_depth

template <OCTREE_TEMPLATE>
OCTREE::Octree(): functor_(PointExtractor()), head_(nullptr), size_(0) {}

template <OCTREE_TEMPLATE>
OCTREE::Octree(InputIterator begin, InputIterator end)
  : Octree(begin, end, PointExtractor()) { }

template <OCTREE_TEMPLATE>
OCTREE::Octree(InputIterator begin, InputIterator end, PointExtractor f)
    : functor_(f), head_(nullptr), size_(0) {

  std::vector<std::pair<InputIterator, Point>> v;
  v.reserve(std::distance(begin, end));

  for (auto it = begin; it != end; ++it) {
    v.push_back(std::pair<InputIterator, Point>(it, functor_(*it)));
  }
  
  size_ = v.size();
  head_ = new Node(v);
}

template <OCTREE_TEMPLATE>
OCTREE::Octree(OCTREE::tree_type&& rhs) 
  : functor_(rhs.functor), head_(rhs.head_), size_(rhs.size_) { }

template <OCTREE_TEMPLATE>
void OCTREE::swap(OCTREE::tree_type& rhs) {
  std::swap(head_, rhs.head_);
  std::swap(functor_, rhs.functor_);
  std::swap(size_, rhs.size_);
}

template <OCTREE_TEMPLATE>
template <typename OutputIterator>
bool OCTREE::search(const Point& p, OutputIterator* it) const {
  return head_->search(p, it);
}

template <OCTREE_TEMPLATE>
template <typename OutputIterator>
void OCTREE::search(const BoundingBox& box, OutputIterator* it) const {
  head_->search(box, it);
}

template <OCTREE_TEMPLATE>
typename OCTREE::tree_type& OCTREE::operator=(typename OCTREE::tree_type rhs) {
  swap(rhs);
  return *this;
}

template <OCTREE_TEMPLATE>
typename OCTREE::tree_type& OCTREE::operator=(typename OCTREE::tree_type&& rhs) {
  swap(rhs);
  return *this;
}

template <OCTREE_TEMPLATE>
OCTREE::~Octree() {
  delete head_;
}

template <OCTREE_TEMPLATE>
size_t OCTREE::size() const {
  return size_;
}

template <OCTREE_TEMPLATE>
OCTREE::Node::Node(const std::vector<std::pair<InputIterator, Point>>& input_values)
  : Node(input_values, Node::getExtrema(input_values), 0) { }

template <OCTREE_TEMPLATE>
OCTREE::Node::Node(
    const std::vector<std::pair<InputIterator, Point>>& input_values, 
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
template <OCTREE_TEMPLATE>
OCTREE::Node::~Node() {
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
template <OCTREE_TEMPLATE>
template <typename OutputIterator>
bool OCTREE::Node::search(const Point& p, OutputIterator* it) {
  if (extrema_.contains(p)) {
    if (tag_ == NodeContents::INTERNAL) {
      unsigned index = getOctantIndex(p);
      childNodeArray& children = *static_cast<childNodeArray*>(value_);
      if (children[index] != nullptr) {
        **it = children[index].search(p);
        return true;
      }
    } else if (tag_ == NodeContents::LEAF) {
      LeafNodeValues& children = *static_cast<LeafNodeValues*>(value_);
      for (size_t i = 0; i < children.size_; ++i) {
        if (std::get<1>(children[i]) == p) {
          **it = std::get<0>(children[i]);
          return true;
        }
      }
    } else if (tag_ == NodeContents::MAX_DEPTH_LEAF) {
      maxItemNode& children = *static_cast<maxItemNode*>(value_);
      for (auto item : children) {
        if (std::get<1>(item) == p) {
          **it = std::get<0>(item);
          return true;
        }
      }
    }
  } 
  return false;
}

#define UNUSED(x) (void)(x)
template <OCTREE_TEMPLATE>
template <typename OutputIterator>
void OCTREE::Node::search(const BoundingBox& box, OutputIterator* it) {
  UNUSED(box);
  UNUSED(it);
}

template <OCTREE_TEMPLATE>
void OCTREE::Node::init_max_depth_leaf(
    const std::vector<std::pair<InputIterator, Point>>& input_values) {  
  value_ = new std::vector<std::pair<InputIterator, Point>>(input_values);
  tag_ = NodeContents::MAX_DEPTH_LEAF;
}

template <OCTREE_TEMPLATE>
void OCTREE::Node::init_leaf(
    const std::vector<std::pair<InputIterator, Point>>& input_values)  {
  std::array<std::pair<InputIterator, Point>, max_per_node> a;
  std::copy(input_values.begin(), input_values.end(), a.begin());
  value_ = new LeafNodeValues{a, input_values.size()};
  tag_ = NodeContents::LEAF;
}

template <OCTREE_TEMPLATE>
void OCTREE::Node::init_internal(
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

template <OCTREE_TEMPLATE>
unsigned int OCTREE::Node::getOctantIndex(const Point& p) {
  // children are ordered left to right, front to back, bottom to top.

  double xmid = (extrema_.xhi - extrema_.xlo) / 2.;
  double ymid = (extrema_.yhi - extrema_.ylo) / 2.;
  double zmid = (extrema_.zhi - extrema_.zlo) / 2.;
  bool left = p[0] < xmid && p[0] >= extrema_.xlo;
  bool front = p[1] < ymid && p[1] >= extrema_.ylo;
  bool bottom = p[2] < zmid && p[2] >= extrema_.zlo;

  if (bottom && left && front) {
    return 0;
  } else if (bottom && !left && front) {
    return 1;
  } else if (bottom && left && !front) {
    return 2;
  } else if (bottom && !left && !front) {
    return 3;
  } else if (!bottom && left && front) {
    return 4;
  } else if (!bottom && !left && front) {
    return 5;
  } else if (!bottom && left && !front) {
    return 6;
  } else if (!bottom && !left && !front) {
    return 7;
  }
}

template <OCTREE_TEMPLATE>
BoundingBox OCTREE::Node::getExtrema(
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

#endif // DEFINED OCTREE_CPU_H