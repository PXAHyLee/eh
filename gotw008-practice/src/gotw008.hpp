#include <memory>

// T must have default ctor and copy assignment
template <typename T>
class Stack {
 public:
    Stack();
    ~Stack();
    Stack(const Stack &);
    Stack &operator=(const Stack &);

    // Modified API. It's not declared as const member function in the original
    // problem statement.
    // returns # of T's in the stack
    unsigned Count() const { return vused_; }

    void Push(const T &);
    T Pop();  // if empty, returns default-constructed T

    // Add additional APIs, i.e., the following functions are not in the
    // original problems.
    void swap(Stack &other) noexcept;
    unsigned Capacity() const { return vsize_; }

 private:
    T *v_;  // pointer to a memory area big enough for 'vsize_' T objects
    unsigned vsize_;  // the size of the 'v_' area
    unsigned vused_;  // the number of T's actually used in the 'v_' area

    // Add an additional data member myself
    std::allocator<T> a_;
};

template <typename T>
Stack<T>::Stack() : v_{nullptr}, vsize_{}, vused_{} {}

template <typename T>
Stack<T>::~Stack() {
    if (v_ != nullptr) {
        // v_ is a pointer previously obtained by std::allocator<T>
        a_.deallocate(v_, vsize_);
    }
}

template <typename T>
Stack<T>::Stack(const Stack &other) : vsize_(), vused_(), a_(other.a_) {
    v_ = a_.allocate(other.vsize_);
    std::uninitialized_copy_n(other.v_, other.vused_, v_);
    vsize_ = other.vsize_;
    vused_ = other.vused_;
}

template <typename T>
Stack<T> &Stack<T>::operator=(const Stack &other) {
    // Even when vsize_ < other.vsize_ is true, it would be possible that the
    // elements in other is much less than its capacity. We don't have to
    // allocate the same capacity as that of other at first.
    if (vsize_ < other.vused_) {
        Stack<T> temp(other);
        swap(temp);
        return *this;
    }
    // Now we know "vsize_ >= other.vused_", and this means all of the elements
    // could be filled in the memory in this object

    // deal with self-assignment
    if (this == &other) {
        return *this;
    }

    if (other.vused_ < vused_) {
        std::copy(other.v_, other.v_ + other.vused_, v_);
        for (T *p = v_ + other.vused_; p != v_ + vused_; ++p) {
            (*&p)->~T();
        }
    } else {
        // std::copy doesn't have strong exception guarantee.
        std::copy(other.v_, other.v_ + vused_, v_);

        // We cannot use assignment op from v_[vused_] v_[other.vused_] since
        // the memory obtained from allocator is uninitialized, so use
        // uninitialized_copy instead
        std::uninitialized_copy(other.v_ + vused_, other.v_ + other.vused_, v_ + vused_);
    }
    vused_ = other.vused_;
    a_ = other.a_;
    return *this;
}

template <typename T>
void Stack<T>::Push(const T &e) {
    if (vused_ + 1 <= vsize_) {
        // Avoid using assignment operator to v[vused_] since the memory is
        // uninitialized.
        T *dest = v_ + vused_;
        std::uninitialized_copy_n(&e, 1, dest);
        ++vused_;
        return;
    }
    // vused_ doesn't have enough space for an element 'e'
    // First, double the allocation and copy the original content.
    unsigned n = (vsize_ != 0) ? 2 * vsize_ : 1;
    T *v = a_.allocate(n);
    std::uninitialized_copy(v_, v_ + vused_, v);

    // Put the element e into the new memory allocation.
    v[vused_] = e;

    // Destroy the orignal elements located in v_.
    if (v_ != nullptr) {
        for (T *begin = v_; begin != v_ + vused_; ++begin) {
          (&*begin)->~T();
        }
        a_.deallocate(v_, vsize_);
    }
    // Finally, replace the pointer v_ with the new memory allocation v and
    // update the corresponding unsigned.
    v_ = v;
    ++vused_;
    vsize_ = n;
}

// If empty, returns default-constructed T
template <typename T>
T Stack<T>::Pop() {
    if (vused_ == 0) {
        return T();
    }
    T result(v_[vused_ - 1]);
    --vused_;
    (&v_[vused_])->~T();
    return result;
}

// Additional function(s) and API(s).

// Non-member swap function
template <typename T>
void swap(Stack<T> &a, Stack<T> &b) noexcept {
    a.swap(b);
}

template <typename T>
void Stack<T>::swap(Stack &other) noexcept {
    using std::swap;
    swap(v_, other.v_);
    swap(vsize_, other.vsize_);
    swap(vused_, other.vused_);
    swap(a_, other.a_);
}
