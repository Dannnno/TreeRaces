#ifndef POINTERLESS_OCTREE_CPU_H
#define POINTERLESS_OCTREE_CPU_H

#include "boundingbox.h"
#include "inneriterator.h"

#include <iostream>
#include <unordered_map>
#include <array>
#include <vector>
#include <utility>
#include <bitset>
#include <algorithm>

template <typename InputIterator, typename PointExtractor, std::size_t max_node_size = 16, std::size_t max_depth = 100>
class PointerlessOctree {
 public:
  using tree_type = PointerlessOctree<InputIterator, PointExtractor, max_node_size, max_depth>;
  // Use 3 bits for each successive level, and 1 for the root
  using index_type = std::bitset<max_depth * 3 + 1>;

  PointerlessOctree();

  PointerlessOctree(InputIterator begin, InputIterator end);

  PointerlessOctree(InputIterator begin, InputIterator end, PointExtractor f);

  PointerlessOctree(const tree_type& rhs);
  
  PointerlessOctree(tree_type&& rhs);

  void swap(tree_type& rhs);

  template <typename OutputIterator>
  bool search(const BoundingBox& box, OutputIterator& it) const;

  template <typename OutputIterator>
  bool search(const BoundingBox& box, OutputIterator& it, const index_type& current_index) const;

  tree_type& operator=(tree_type rhs);

  tree_type& operator=(tree_type&& rhs);

  ~PointerlessOctree() = default;

  std::size_t size() const;
  std::size_t depth() const;

 private:
  struct Node;

  Node& init_nodes(
      const std::vector<std::pair<InputIterator, Point3d>>& v,
      std::size_t depth, index_type index_so_far);

  using LeafNodeValue = std::vector<std::pair<InputIterator, Point3d>>;
  using InternalNodeValue = std::array<index_type, 8>;

  union NodeValues {
    NodeValues() : internalValue_() {}
    NodeValues(const LeafNodeValue& v) : leafValue_(v) {}
    NodeValues(const InternalNodeValue& v) : internalValue_(v) {}
    ~NodeValues() {}

    LeafNodeValue leafValue_;
    InternalNodeValue internalValue_;
  };

  enum class NodeContents : char {
    INTERNAL,
    LEAF
  };

  struct Node {
    Node() : type_(NodeContents::INTERNAL) {}
    Node(const Node& rhs) : extrema_(rhs.extrema_), key_(rhs.key_), type_(rhs.type_) {
      if (type_ == NodeContents::INTERNAL) {
        values_.internalValue_ = rhs.values_.internalValue_;
      } else {
        values_.leafValue_ = rhs.values_.leafValue_;
      }
    }
    ~Node();

    BoundingBox extrema_;
    NodeValues values_;
    index_type key_;
    NodeContents type_;
  };

  PointExtractor functor_;
  std::unordered_map<index_type, Node> nodes_;
  std::size_t depth_;
  std::size_t size_;
};

#define POINTERLESS_OCTREE_TEMPLATE typename InputIterator, typename PointExtractor, std::size_t max_node_size, std::size_t max_depth
#define POINTERLESSOCTREE PointerlessOctree<InputIterator, PointExtractor, max_node_size, max_depth>

template <POINTERLESS_OCTREE_TEMPLATE>
POINTERLESSOCTREE::PointerlessOctree() 
  : functor_(PointExtractor()), depth_(0), size_(0) { }

template <POINTERLESS_OCTREE_TEMPLATE>
POINTERLESSOCTREE::PointerlessOctree(InputIterator begin, InputIterator end) 
  : PointerlessOctree(begin, end, PointExtractor()) { }

template <POINTERLESS_OCTREE_TEMPLATE>
POINTERLESSOCTREE::PointerlessOctree(InputIterator begin, InputIterator end, PointExtractor f) 
  : functor_(f), depth_(0), size_(0) {

  std::vector<std::pair<InputIterator, Point3d>> v;
  v.reserve(std::distance(begin, end));

  for (auto it = begin; it != end; ++it) {
    v.push_back(std::pair<InputIterator, Point3d>(it, functor_(*it)));
  }

  index_type rootIndex(1); 

  init_nodes(v, 1, rootIndex);
}

template <POINTERLESS_OCTREE_TEMPLATE>
typename POINTERLESSOCTREE::Node& POINTERLESSOCTREE::init_nodes(
    const std::vector<std::pair<InputIterator, Point3d>>& v, 
    std::size_t depth, index_type index_so_far) {
  BoundingBox extrema = makeBoundingBox(InnerIterator<InputIterator>(v.begin()), InnerIterator<InputIterator>(v.end()));
  bool at_max_depth = depth == max_depth;
  bool leaf_node = v.size() <= max_node_size;
  NodeContents type = leaf_node || at_max_depth ? NodeContents::LEAF : NodeContents::INTERNAL;
  std::size_t node_depth = depth;

  Node n;
  n.extrema_ = extrema;
  n.key_ = index_so_far;
  n.type_ = type;

  if (type == NodeContents::LEAF) {
    n.values_.leafValue_ = v;
    size_ += v.size();
    depth_ = std::max(node_depth, depth_);
  } else {
    std::array<std::vector<std::pair<InputIterator, Point3d>>, 8> childVectors;
    auto childExtrema = extrema.partition();
    for (unsigned char child = 0; child < 8; ++child) {
      std::vector<std::pair<InputIterator, Point3d>>& childVector = childVectors[child];
      childVector.reserve(v.size() / 8);

      std::copy_if(
        v.begin(), v.end(), std::back_inserter(childVector),
        [&childExtrema, child](const std::pair<InputIterator, Point3d>& element) -> bool {
          Point3d p = std::get<1>(element);

          return childExtrema[child].contains(p);
        }
      );

      std::array<index_type, 8> values;

      index_type morton_index = (index_so_far << 3) | index_type(child);

      values[child] = childVector.empty()
        ? index_type(0)
        : init_nodes(childVector, depth + 1, morton_index).key_;

      n.values_.internalValue_ = values;
    }

  }

  nodes_.insert(std::make_pair(index_so_far, n));
  return nodes_.at(index_so_far);
}

template <POINTERLESS_OCTREE_TEMPLATE>
std::size_t POINTERLESSOCTREE::size() const {
  return size_;
}

template <POINTERLESS_OCTREE_TEMPLATE>
std::size_t POINTERLESSOCTREE::depth() const {
  return depth_;
}

template <POINTERLESS_OCTREE_TEMPLATE>
POINTERLESSOCTREE::Node::~Node() {
  switch (type_) {
    case NodeContents::INTERNAL:
      values_.internalValue_.~InternalNodeValue();
      break;
    case NodeContents::LEAF:
      values_.leafValue_.~LeafNodeValue();
      break;
    default:
      throw "Invalid node type";
  }
}

template <POINTERLESS_OCTREE_TEMPLATE>
template <typename OutputIterator>
bool POINTERLESSOCTREE::search(const BoundingBox& b, OutputIterator& out) const {
  return search(b, out, 1);
}

template <POINTERLESS_OCTREE_TEMPLATE>
template <typename OutputIterator>
bool POINTERLESSOCTREE::search(const BoundingBox& b, OutputIterator& out, const index_type& current_index) const {
  bool success = false;
  if (nodes_.find(current_index) != nodes_.end() && current_index != index_type(0)) {
    const Node& n = nodes_.at(current_index);
    if (n.type_ == NodeContents::INTERNAL) {
      index_type next_index = current_index << 3;
      for (unsigned char octant = 0; octant < 8; ++octant) {
        index_type child_index = next_index | index_type(octant);
        if (nodes_.find(child_index) != nodes_.end() && child_index != index_type(0)) {
          success |= search(b, out, child_index);
        }
      }
    } else {
      for (auto value : n.values_.leafValue_) {
        if (b.contains(std::get<1>(value))) {
          *out = std::get<0>(value);
          ++out;
          success = true;
        }
      }
    }
  }
  return success;
}

#endif // defined POINTERLESS_OCTREE_CPU_H
