/*
    file - octree.h

    Templated implementation of an octree for a cpu

 */


#ifndef OCTREE_CPU_H
#define OCTREE_CPU_H

#include <array>
#include <cstddef>
#include <iterator>
#include <utility>
#include <type_traits>


template <typename T, size_t max_per_node = 16>
class Octree {
 private:
  class Node;
  template <typename V> class Iterator;
 public:
  using value_type = T;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using const_pointer = const value_type*;
  using iterator = Iterator<T>;
  using const_iterator = Iterator<const T>;

  Octree(): head_(nullptr), size_(0) {}

  template <typename Container>
  Octree(const Container& points): head_(nullptr), size_(0) {
    // TODO figure out partitioning
  }

  template <typename InputIterator>
  Octree(const InputIterator& begin, const InputIterator& end): head_(nullptr), size_(0) {
    // TODO figure out partitioning
  }

  Octree(const Octree<T, max_per_node>& rhs): head_(nullptr), size_(0) {
    // TODO figure out partitioning
  }

  template <size_t max_per_node_, typename std::enable_if<max_per_node_ != max_per_node>::type>
  Octree(const Octree<T, max_per_node_>& rhs): head_(nullptr), size_(0) {
    // TODO figure out partitioning
  }
  
  Octree(Octree<T, max_per_node>&& t) noexcept : head_(t.head_), size_(t.size_) { }

  void swap(Octree<T, max_per_node>& other) {
    std::swap(head_, other.head_);
    std::swap(size_, other.size_);
  }

  Octree<T, max_per_node>& operator=(Octree<T, max_per_node> rhs) {
    swap(rhs);
    return *this;
  }

  Octree<T, max_per_node>& operator=(Octree<T, max_per_node>&& rhs) {
    swap(rhs);
    return *this;
  }

  ~Octree() noexcept {
    delete head_;
  }

  iterator begin() {
    return Iterator<T>(head_);
  }
  const_iterator begin() const {
    return Iterator<const T>(head_);
  }
  const_iterator cbegin() const {
    return begin();
  }

  iterator end() {
    return Iterator<T>(nullptr);
  }
  const_iterator end() const {
    return Iterator<const T>(nullptr);
  }
  const_iterator cend() const {
    return end();
  }

  iterator insert(T value) {
    throw std::runtime_error("NYI");
  }

  iterator remove(T value) {
    throw std::runtime_error("NYI");
  }

  iterator erase(iterator it) {
    throw std::runtime_error("NYI");
  }

  bool contains(T value) {
    return false;
  }

  bool operator==(const Octree<T>& rhs) {
    return (head_ == nullptr && rhs.head_ == nullptr) || *head_ == *(rhs.head_);
  }

  bool operator!=(const Octree<T>& rhs) {
    return !operator==(rhs);
  }
 
 private:
  Node* head_;
  size_t size_;

  class Node {
   public:    
    template <typename Container>
    Node(const Container&);
    template <typename InputIterator>
    Node(const InputIterator& begin, const InputIterator& end);
    Node(Node&&) noexcept;
    ~Node() noexcept {
      for (Node* child : children_) {
        delete child;
      }
    }
   private:
    friend class Iterator<T>;
    friend class Octree<T, max_per_node>;
    std::array<Node*, 8> children_;
    // std::array<T, max_per_node> items_;
    size_t size_;
    T value_;
  };

  template <typename V> 
  class Iterator {
   public:
    using difference_type = std::ptrdiff_t;
    using value_type = V;
    using pointer = V*;
    using reference = T&;
    using iterator_category = std::forward_iterator_tag;

    Iterator() = delete;
    Iterator(const Iterator<V>&);
    Iterator(Iterator<V>&&);

    Iterator<V>& operator=(Iterator<V>);
    Iterator<V>& operator=(Iterator<V>&&);

    bool operator==(const Iterator<V>&);
    bool operator!=(const Iterator<V>&);

    reference operator*();
    reference operator*() const;
    pointer operator->();
    pointer operator->() const;

    void operator++();
    Iterator<V>& operator++(int);

   private:
    friend class Octree<V, max_per_node>;
    friend class Node;
    Node* current_;
  };
};

// template <typename T>
// Octree<T>::Octree() : head_(nullptr) {}

// template <typename T>
// Octree<T>::Octree(const Octree<T>& rhs) {
//   for (auto item : rhs) {
//     insert(item);
//   }
// }

// template <typename T>
// Octree<T>::Octree(Octree<T>&& temp) noexcept : head_(temp.head_) {}

// template <typename T>
// Octree<T>& Octree<T>::operator=(Octree<T> temp) {
//   std::swap(head_, temp.head_);
//   return *this;
// }

// template <typename T>
// Octree<T>& Octree<T>::operator=(Octree<T>&& temp) noexcept {
//   head_ = std::move(temp.head_);
//   return *this;
// }

// template <typename T>
// Octree<T>::~Octree() noexcept {
//   delete head_;
// }

// template <typename T>
// typename Octree<T>::iterator Octree<T>::begin() {
//   return Iterator<T>(head_);
// }

// template <typename T>
// typename Octree<T>::const_iterator Octree<T>::begin() const {
//   return Iterator<const T>(head_);
// }

// template <typename T>
// typename Octree<T>::const_iterator Octree<T>::cbegin() const {
//   return Iterator<const T>(head_);
// }

// template <typename T>
// typename Octree<T>::iterator Octree<T>::end() {
//   return Iterator<T>(nullptr);
// }

// template <typename T>
// typename Octree<T>::const_iterator Octree<T>::end() const {
//   return Iterator<const T>(nullptr);
// }

// template <typename T>
// typename Octree<T>::const_iterator Octree<T>::cend() const {
//   return Iterator<const T>(nullptr);
// }

// template <typename T>
// bool Octree<T>::operator==(const Octree<T>& rhs) {
//   Iterator<const T> left = begin();
//   decltype(left) right = rhs.begin();
//   while (left != end() && right != rhs.end()) {
//     if (*left != *right) {
//       return false;
//     }
//     ++left;
//     ++right;
//   }

//   return (left == end()) == (right == end());
// }

// template <typename T>
// bool Octree<T>::operator!=(const Octree<T>& rhs) {
//   return !operator==(rhs);
// }

//   // iterator insert(T);
//   // iterator remove(T);
//   // iterator remove(iterator);
//   // bool contains(T);

// template <typename T>
// Octree<T>::Node::~Node() {
//   for (auto child : children_) {
//     delete child;
//   }
// }

#endif // DEFINED OCTREE_CPU_H