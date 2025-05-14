#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <vector>

template <typename T>
struct Node {
  Node* left;
  Node* right;
  Node* parent;
  T* value;

  Node(const T& value)
      : value(value), left(nullptr), right(nullptr), parent(nullptr) {}
  Node() : left(nullptr), right(nullptr), parent(nullptr) {}
};

class InOrder {};

class PostOrder {};

class PreOrder {};

template <class T, class Order = InOrder, class Compare = std::less<T>,
          class Allocator = std::allocator<T> >
class BST {
  class iterator {
    typedef T value_type;
    typedef Node<value_type> node_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef size_t size_type;

   private:
    Node<value_type>* current_node_;
    Node<value_type>* root_;
    Order current_order_;

    void inorder(node_type* node, std::vector<value_type>& order_vector) {
      if (node == nullptr) {
        return;
      }
      inorder(node->left, order_vector);
      order_vector.push_back(*node->value);
      inorder(node->right, order_vector);
    }

    void postorder(node_type* node, std::vector<value_type>& order_vector) {
      if (node == nullptr) {
        return;
      }
      postorder(node->left, order_vector);
      postorder(node->right, order_vector);
      order_vector.push_back(*node->value);
    }

    void preorder(node_type* node, std::vector<value_type>& order_vector) {
      if (node == nullptr) {
        return;
      }
      order_vector.push_back(*node->value);
      preorder(node->left, order_vector);
      preorder(node->right, order_vector);
    }

    void find_node(node_type* node, const_reference value) {
      if (node != nullptr) {
        if (*node->value == value) {
          current_node_ = node;
        }
        find_node(node->left, value);
        find_node(node->right, value);
      }
    }

    iterator next_iterator(InOrder order) {
      std::vector<value_type> order_vector;
      inorder(root_, order_vector);
      size_type index = -1;
      for (size_type i = 0; i < order_vector.size(); i++) {
        if (order_vector[i] == *current_node_->value) {
          index = i + 1;
        }
      }
      if (index < order_vector.size()) {
        find_node(root_, order_vector[index]);
      } else {
        current_node_ = nullptr;
      }
      return *this;
    }

    iterator next_iterator(PreOrder order) {
      std::vector<value_type> order_vector;
      preorder(root_, order_vector);
      size_type index = -1;
      for (size_type i = 0; i < order_vector.size(); i++) {
        if (order_vector[i] == *current_node_->value) {
          index = i + 1;
        }
      }
      if (index < order_vector.size()) {
        find_node(root_, order_vector[index]);
      } else {
        current_node_ = nullptr;
      }
      return *this;
    }

    iterator next_iterator(PostOrder order) {
      std::vector<value_type> order_vector;
      postorder(root_, order_vector);
      size_type index = -1;
      for (size_type i = 0; i < order_vector.size(); i++) {
        if (order_vector[i] == *current_node_->value) {
          index = i + 1;
        }
      }
      if (index < order_vector.size()) {
        find_node(root_, order_vector[index]);
      } else {
        current_node_ = nullptr;
      }
      return *this;
    }

    iterator prev_iterator(InOrder order) {
      std::vector<T> order_vector;
      inorder(root_, order_vector);
      size_type index = -1;
      for (size_type i = 0; i < order_vector.size(); i++) {
        if (current_node_ != nullptr) {
          if (order_vector[i] == *current_node_->value) {
            index = i - 1;
          }
        }
      }
      if (current_node_ == nullptr) {
        find_node(root_, order_vector[order_vector.size() - 1]);
      } else if (index != -1) {
        find_node(root_, order_vector[index]);
      } else {
        current_node_ = nullptr;
      }
      return *this;
    }

    iterator prev_iterator(PreOrder order) {
      std::vector<T> order_vector;
      preorder(root_, order_vector);
      size_type index = -1;
      for (size_type i = 0; i < order_vector.size(); i++) {
        if (current_node_ != nullptr) {
          if (order_vector[i] == *current_node_->value) {
            index = i - 1;
          }
        }
      }
      if (current_node_ == nullptr) {
        find_node(root_, order_vector[order_vector.size() - 1]);
      } else if (index != -1) {
        find_node(root_, order_vector[index]);
      } else {
        current_node_ = nullptr;
      }
      return *this;
    }

    iterator prev_iterator(PostOrder order) {
      std::vector<T> order_vector;
      postorder(root_, order_vector);
      size_type index = -1;
      for (size_type i = 0; i < order_vector.size(); i++) {
        if (current_node_ != nullptr) {
          if (order_vector[i] == *current_node_->value) {
            index = i - 1;
          }
        }
      }
      if (current_node_ == nullptr) {
        find_node(root_, order_vector[order_vector.size() - 1]);
      } else if (index != -1) {
        find_node(root_, order_vector[index]);
      } else {
        current_node_ = nullptr;
      }
      return *this;
    }

   public:
    iterator(node_type* node, Order order, node_type* root)
        : root_{root}, current_node_{node}, current_order_{order} {}

    iterator(const iterator& other)
        : root_(other.root_),
          current_node_(other.current_node_),
          current_order_(other.current_order_) {}

    iterator operator++() { return next_iterator(current_order_); }

    iterator operator--() { return prev_iterator(current_order_); }

    iterator operator++(int) {
      iterator copy_iteartor(*this);
      ++(*this);
      return copy_iteartor;
    }

    iterator operator--(int) {
      iterator copy_iteartor(*this);
      --(*this);
      return copy_iteartor;
    }

    value_type operator*() { return *current_node_->value; }

    bool operator==(const iterator& other) {
      return (other.current_node_ == current_node_);
    }

    bool operator!=(const iterator& other) {
      return (other.current_node_ != current_node_);
    }
  };

 private:
  typedef Node<T> node_type;
  typedef T value_type;
  typedef BST& reference;
  typedef const BST& const_reference;
  typedef const iterator const_iterator;
  typedef size_t size_type;
  typedef Allocator allocator_type;

  Order type_order_;
  size_type size_ = 0;
  size_type max_size_ = 15352;
  node_type* root_ = nullptr;
  Compare compare_;
  allocator_type allocator_;

  void copy_tree(node_type* lhs, node_type* rhs) {
    if (rhs != nullptr) {
      if (lhs == nullptr) {
        lhs = new node_type[1];
      }
      lhs->value = allocator_.allocate(1);
      *lhs->value = *rhs->value;
      if (rhs->left != nullptr) {
        lhs->left = new node_type[1];
        copy_tree(lhs->left, rhs->left);
      }

      if (rhs->right != nullptr) {
        lhs->right = new node_type[1];
        copy_tree(lhs->right, rhs->right);
      }
    }
  }

  node_type* begin_of_order(InOrder order) {
    node_type* first_in_order = root_;
    while (first_in_order->left) {
      first_in_order = first_in_order->left;
    }
    return first_in_order;
  }

  node_type* begin_of_order(PreOrder order) { return root_; }

  node_type* begin_of_order(PostOrder order) {
    node_type* first_in_order = root_;
    while (first_in_order->left || first_in_order->right) {
      if (first_in_order->left) {
        first_in_order = first_in_order->left;
      } else {
        first_in_order = first_in_order->right;
      }
    }
    return first_in_order;
  }

  void delete_node(node_type* node) {
    if (node != nullptr) {
      delete_node(node->left);
      delete_node(node->right);
      allocator_.deallocate(node->value, 1);
      delete node;
    }
  }

  node_type* find_position(const value_type& x) {
    node_type* position = root_;
    bool flag = true;
    while (flag) {
      if (*position->value == x) {
        return position;
      } else if (compare_(x, *position->value)) {
        if (position->right != nullptr) {
          position = position->right;
        } else {
          return position;
        }
      } else if (!compare_(x, *position->value)) {
        if (position->left) {
          position = position->left;
        } else {
          return position;
        }
      }
    }
    return position;
  }

  node_type* minimum(node_type* x) {
    if (x->left == nullptr) {
      return x;
    }
    return minimum(x->left);
  }

 public:
  BST() = default;

  BST(const_reference other) : size_(other.size_) {
    copy_tree(root_, other.root_);
  }

  BST(std::initializer_list<T> element) { this->insert(element); }

  template <class OtherIterator>
  BST(OtherIterator i, OtherIterator j) {
    this->insert<OtherIterator>(i, j);
  }

  reference operator=(BST other) {
    size_ = other.size_;
    copy_tree(root_, other.root_);
    return *this;
  }

  ~BST() { clear(); }

  iterator begin() {
    if (size_ != 0) {
      return iterator(begin_of_order(type_order_), type_order_, root_);
    }
    return iterator(nullptr, type_order_, nullptr);
  }

  iterator end() { return iterator(nullptr, type_order_, root_); }

  const_iterator begin() const { return begin(); }

  const_iterator end() const { return end(); }

  bool operator==(const BST& other) const {
    return std::equal(this.begin(), this.end(), other.begin(), other.end());
  }

  bool operator!=(const BST& other) const { return !(this == other); }

  void swap(BST& other) {
    std::swap(this->root_, other->root_);
    std::swap(this->size_, other.size_);
    std::swap(this->allocator_, other.allocator_);
    std::swap(this->type_order_, other.type_order_);
    std::swap(this->compare_, other.compare_);
  }

  size_type size() const { return size_; }

  size_type max_size() const { return max_size_; }

  bool empty() const { return size_ == 0; }

  const iterator insert(const value_type& x) {
    if (root_ == nullptr) {
      ++size_;
      root_ = new node_type[1];
      root_->value = allocator_.allocate(1);
      *root_->value = x;
      return iterator(root_, type_order_, root_);
    } else {
      node_type* node = find_position(x);
      if (*node->value == x) {
        return iterator(node, type_order_, root_);
      } else if (compare_(x, *node->value)) {
        node_type* new_node = new node_type[1];
        new_node->value = allocator_.allocate(1);
        *new_node->value = x;
        new_node->parent = node;
        node->right = new_node;
        ++size_;
        return iterator(new_node, type_order_, root_);
      }
      node_type* new_left = new node_type[1];
      new_left->value = allocator_.allocate(1);
      *new_left->value = x;
      new_left->parent = node;
      node->left = new_left;
      ++size_;

      return iterator(new_left, type_order_, root_);
    }
  }

  void insert(std::initializer_list<value_type> il) {
    insert(il.begin(), il.end());
  }

  template <class OtherIterator>
  void insert(OtherIterator i, OtherIterator j) {
    while (i != j) {
      this->insert(*i);
      ++i;
    }
  }
  template <typename U, class Travelsar, class Compare_ = std::less<T>,
            class Allocator_ = std::allocator<T> >
  void merge(BST<U, Travelsar>& other) {
    auto i = other.begin();
    auto j = other.end();
    while (i != j){
      this->insert(*i);
      ++i;
    }
  }

  size_type erase(const value_type& x) {
    size_type result;
    if (find(x)) {
      result = 1;
    } else {
      result = 0;
    }
    this->extract(x);
    return result;
  }

  const iterator erase(node_type* node) {
    iterator current(node, type_order_, root_);
    ++current;
    this->extract(node);
    return current;
  }

  template <class OtherIterator>
  const iterator erase(OtherIterator i, OtherIterator j) {
    ++j;
    extract(i, j);
    return j;
  }

  node_type* extract(node_type*& position, const T& x) {
    if (position == nullptr) {
      return position;
    }
    if (compare_(*position->value, x)) {
      position->left = extract(position->left, x);
    } else if (compare_(x, *position->value)) {
      position->right = extract(position->right, x);
    } else {
      if (position->left == nullptr) {
        node_type* new_node = position->right;
        delete[] position;
        --size_;
        return new_node;
      } else if (position->right == nullptr) {
        node_type* new_node = position->left;
        delete[] position;
        --size_;
        return new_node;
      }
      node_type* new_node = minimum(position->right);
      position->value = new_node->value;
      position->right = extract(position->right, *new_node->value);
    }
    return position;
  }

  void extract(iterator iterator_) { extract(iterator_.cur_node()); }

  void extract(node_type* node) {
    if (node) {
      root_ = extract(root_, *node->value);
    }
  }

  void extract(const value_type& x) {
    node_type* node = find(x);
    extract(node);
  }

  void clear() { delete_node(root_); }

  node_type* find(const value_type& x) {
    node_type* node = root_;
    while (node) {
      if (*node->value == x) {
        return node;
      } else if (compare_(x, *node->value)) {
        node = node->right;
      } else {
        node = node->left;
      }
    }
    return node;
  }

  size_type count(const value_type& x) { return (find(x) ? 1 : 0); }

  bool contains(const_iterator node) { return this->count(node) != 0; }

  allocator_type get_allocator() { return allocator_; }

  const Compare key_comp() { return compare_; }

  const Compare value_comp() { return compare_; }
};

template <class T, class Order = InOrder, class Compare = std::less<T>,
          class Allocator = std::allocator<T> >
void swap(BST<T, Order>& lhs, BST<T, Order>& rhs) {
  lhs.swap(rhs);
}
