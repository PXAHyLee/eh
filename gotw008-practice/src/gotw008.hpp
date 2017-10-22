// #include <iostream>
#include <memory>

// T must have default ctor and copy assignment
template <typename T>
class Stack {
public:
  Stack();
  ~Stack();
  Stack(const Stack &);
  Stack &operator=(const Stack &);

  // modified API. Originally, it's not declared as const member function
  // returns # of T's in the stack
  unsigned Count() const { return vused_; }

  // modified API. This is not included in the original problem.
  unsigned Capacity() const { return vsize_; }
  void Push(const T &);
  T Pop(); // if empty, returns default-constructed T

  // Add my custom swap function
  void swap(Stack& other) noexcept;

private:
  T *v_;           // pointer to a memory area big enough for 'vsize_' T objects
  unsigned vsize_; // the size of the 'v_' area
  unsigned vused_; // the number of T's actually used in the 'v_' area

  // add the allocator myself
  std::allocator<T> a_;
};

template<typename T>
void Stack<T>::swap(Stack& other) noexcept {
  // std::cout << __PRETTY_FUNCTION__ << std::endl;
  using std::swap;
  swap(v_, other.v_);
  swap(vsize_, other.vsize_);
  swap(vused_, other.vused_);
  swap(a_, other.a_);
}

// non-member function swap
template<typename T>
void swap(Stack<T>& a, Stack<T>& b) noexcept {
  // std::cout << "non-member function swap" << std::endl;
  a.swap(b);
}

template<typename T> Stack<T>::Stack() : v_{nullptr}, vsize_{}, vused_{} {
  // Note, even if vsize_ is zero, the allocation function would be called to
  // allocate an array with no elements!
}

template<typename T> Stack<T>::~Stack() {
  // v_ is a pointer previously obtained by std::allocator<T>
  if (v_ != nullptr) {
    a_.deallocate(v_, vsize_);
  }
}

// copy constructor
template<typename T> Stack<T>::Stack(const Stack &other)
    : vsize_(), vused_(), a_(other.a_) {
  v_ = a_.allocate(other.vsize_);
  std::uninitialized_copy_n(other.v_, other.vused_, v_);
  vsize_ = other.vsize_;
  vused_ = other.vused_;
}

// copy assignment
// std::copy doesn't have strong exception guarantee.
template<typename T> Stack<T>& Stack<T>::operator=(const Stack& other) {
  // Even when vsize_ < other.vsize_ is true, it would be possible that the
  // elements in other is much less than its capacity.
  if (vsize_ < other.vused_) {
    Stack<T> temp(other);
    swap(temp);
    return *this;
  }

  // deal with self-assignment
  if (this == &other)
    return *this;

  // vsize_ >= other.vused_
  if (other.vused_ < vused_) {
    std::copy(other.v_, other.v_ + other.vused_, v_);
    for (T* p = v_ + other.vused_; p != v_ + vused_; ++p) {
      (*&p)->~T();
    }
  } else {
    std::copy(other.v_, other.v_ + vused_, v_);

    // We cannot use assignment op from v_[vused_] v_[other.vused_] since the
    // memory obtained from allocator is uninitialized, so use
    // uninitialized_copy
    std::uninitialized_copy(other.v_ + vused_,
        other.v_ + other.vused_,
        v_ + vused_);
  }
  vused_ = other.vused_;
  a_ = other.a_;
  return *this;
}

template<typename T>
void Stack<T>::Push(const T& e) {
  if (vused_ + 1 <= vsize_) {
    T* start = v_ + vused_;
    v_[vused_] = e;
    ++vused_;
    return;
  }
  // vused_ doesn't have enough space for a element 'e'
  // First, double the allocation and copy the original content.
  unsigned n = (vsize_ != 0) ? 2 * vsize_ : 1;
  T* v = a_.allocate(n);
  std::uninitialized_copy(v_, v_ + vused_, v);

  // Put the element e into the new memory allocation.
  v[vused_] = e;

  // Replace the pointer v_ with the new memory allocation v. Destroy the
  // orignal element if necessary.
  if (v_ != nullptr) {
    for (T* begin = v_; begin != v_ + vused_; ++begin)
      (&*begin)->~T();
    a_.deallocate(v_, vsize_);
  }
  v_ = v;
  ++vused_;
  vsize_ = n;
}

// if empty, returns default-constructed T
template<typename T> T Stack<T>::Pop() {
  if (vused_ == 0) {
    return T();
  }
  T result(v_[vused_ - 1]);
  // std::cout << result << std::endl;
  --vused_;
  (&v_[vused_])->~T();
  return result;
}
