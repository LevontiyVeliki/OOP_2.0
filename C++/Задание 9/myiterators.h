#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>

template <class Iterator, class F>
class TransformIterator {
 private:
  Iterator it_;
  F func_;

 public:
  TransformIterator(const Iterator& it, const F& func) : it_(it), func_(func) {}

  auto operator*() const { return func_(*it_); }
  TransformIterator<Iterator, F>& operator++() {
    ++it_;
    return *this;
  }

  TransformIterator<Iterator, F> operator--() {
    --it_;
    return *this;
  }


  bool operator==(const TransformIterator<Iterator, F>& other) const {
    return this->it_ == other.it_;
  }

};

template <class Iterator, class F>
class FilterIterator {
    using iterator = Iterator;
 private:
  Iterator it_;
  Iterator end_;
  F func_;

 public:
  FilterIterator(const Iterator& it, const Iterator& end, const F& func)
      : it_(it), end_(end), func_(func) {}

  auto operator*() const { return *it_; }
  FilterIterator<Iterator, F>& operator++() {
    ++it_;
    while (it_ != end_ && !func_(*it_)) {
      ++it_;
    }
    return *this;
  }
  FilterIterator<Iterator, F> operator--() {
    --it_;
    while (it_ != end_ && !func_(*it_)) {
      --it_;
    }
    return *this;
  }

  bool operator==(const FilterIterator<Iterator, F>& other) const {
    return it_ == other.it_;
  }

  bool operator!=(const FilterIterator<Iterator, F>& other) const {
    return !(*this == other);
  }
};

template <typename Iterator>
class ReverseIterator {
    using iterator = Iterator;
 private:
  Iterator it_;

 public:
  ReverseIterator(const Iterator& it) : it_(it) {}
  ReverseIterator<Iterator> operator++() {
    --it_;
    return *this;
  }
  ReverseIterator<Iterator> operator--() {
    ++it_;
    return *this;
  }
  auto operator*() const {
    auto copy = it_;
    return *(--copy);
  }
  bool operator==(const ReverseIterator<Iterator>& other) {
    return this->it_ == other.it_;
  }
  bool operator!=(const ReverseIterator<Iterator>& other) {
    return !(*this == other);
  }
};

template <typename Iterator>
class KeysIterator {
  using iterator = Iterator;
 private:
  Iterator it_;

 public:
  KeysIterator(const Iterator& it) : it_(it) {}
  KeysIterator<Iterator> operator++() {
    ++it_;
    return *this;
  }

  KeysIterator<Iterator> operator--() {
    --it_;
    return *this;
  }

  auto operator*() const { return it_->first; }

  bool operator==(const KeysIterator<Iterator>& other) const {
    return this->it_ == other.it_;
  }
  bool operator!=(const KeysIterator<Iterator>& other) const {
    return !(*this == other);
  }
};

template <typename Iterator>
class ValuesIterator {
using iterator = Iterator;
 private:
  Iterator it_;

 public:
  ValuesIterator(const Iterator& it) : it_(it) {}
  ValuesIterator<Iterator> operator++() {
    ++it_;
    return *this;
  }

  ValuesIterator<Iterator> operator--() {
    --it_;
    return *this;
  }

  auto operator*() const { return it_->second; }

  bool operator==(const ValuesIterator<Iterator>& other) const {
    return this->it_ == other.it_;
  }
  bool operator!=(const ValuesIterator<Iterator>& other) const {
    return !(*this == other);
  }
};