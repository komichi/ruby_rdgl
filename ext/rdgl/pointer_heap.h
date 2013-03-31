#include <iostream>
#include <vector>
#include <algorithm>

namespace std
{
  template <class _Tp>
  struct deref_less : public binary_function<_Tp, _Tp, bool>
  {
    bool
    operator()(const _Tp& __x, const _Tp& __y) const
    { if (__x && __y) return *__x < *__y;  return false; }
  };

  template <class _Tp>
  struct deref_greater : public binary_function<_Tp, _Tp, bool>
  {
    bool
    operator()(const _Tp& __x, const _Tp& __y) const
    { if (__x && __y) return *__x > *__y;  return false; }
  };
}

namespace RDGL {

#ifndef PH_CMP
#define PH_CMP std::deref_greater<T *>()
#endif

// PointerHeap<Foo> requires something like the following to be defined
//  in order for the comparison to work on the objects (not their pointers).
// bool operator()(const Foo * a, const Foo * b) const { return *a < *b; }
template<typename T>
class PointerHeap : public std::vector<T *>
{
  public:
    PointerHeap(void);
    PointerHeap(PointerHeap & f);
    T * insert(T & v);
    T * pop(void);
    T * min(void);
    void reheapify(void);
    void sort_vec(void);
    template<typename U>
    friend std::ostream & operator<<(std::ostream & os, PointerHeap<U> & f);
};

template<typename T>
PointerHeap<T>::PointerHeap(void) : std::vector<T *>()
{
  reheapify();
}

template<typename T>
PointerHeap<T>::PointerHeap(PointerHeap<T> & f) : std::vector<T *>(f)
{
  reheapify();
}

template<typename T>
void
PointerHeap<T>::reheapify(void)
{
  make_heap(this->begin(), this->end(), PH_CMP);
}

// returns the minimum element
template<typename T>
T *
PointerHeap<T>::min(void)
{
  if (this->size() > 0) return this->front();
  return NULL;
}

// removes the minimum element
template<typename T>
T *
PointerHeap<T>::pop(void)
{
  T * minv = NULL;
  if (this->size() > 0)
  {
    minv = min();
    pop_heap(this->begin(), this->end(), PH_CMP);
    this->pop_back();
  }
  return minv;
}

// performs a reheapify after a vector insert;
// only adds if the value doesn't already exist
template<typename T>
T *
PointerHeap<T>::insert(T & v)
{
  bool is_empty = false;
  typename PointerHeap<T>::iterator result;
  is_empty = (this->size() == 0);
  if (! is_empty) result = find(this->begin(), this->end(), &v);
  // if the vertex isn't there
  if (is_empty || result == this->end())
  {
    this->push_back(&v);
    push_heap(this->begin(), this->end(), PH_CMP);
  }
  return &v;
}

// resorts the underlying vector
template<typename T>
void
PointerHeap<T>::sort_vec(void)
{
  sort_heap(this->begin(), this->end(), PH_CMP);
}

template<typename T>
std::ostream &
operator<<(std::ostream & os, PointerHeap<T> & h)
{
  typename PointerHeap<T>::iterator iter;
  os << "Heap contents: " << std::endl;
  for (iter = h.begin(); iter != h.end(); ++iter)
    os << *(*iter);
  os << std::endl;
  return os;
}

}

/*
int
main(void)
{
  Foo f1(9), f2(1), f3(8), f4(2);
  Foo f5(7), f6(3), f7(6), f8(4);
  PointerHeap<Foo> * foo_heap = new PointerHeap<Foo>;
  foo_heap->insert(f1);
  foo_heap->insert(f4);
  foo_heap->insert(f6);
  foo_heap->insert(f7);
  foo_heap->insert(f3);
  foo_heap->insert(f5);
  foo_heap->insert(f2);
  foo_heap->insert(f8);
  std::cout << "all  : ";
  std::cout << *foo_heap;
  while (foo_heap->size() != 5) foo_heap->pop();
  std::cout << "all-5: ";
  std::cout << *foo_heap;
  foo_heap->insert(f1);
  foo_heap->insert(f2);
  foo_heap->insert(f3);
  foo_heap->insert(f4);
  foo_heap->insert(f6);
  foo_heap->insert(f7);
  foo_heap->insert(f5);
  foo_heap->insert(f8);
  std::cout << "all  : ";
  std::cout << *foo_heap;
  //while (foo_heap->size() != 4) std::cout << *foo_heap->pop();
  //std::cout << endl;
  while (foo_heap->size() != 0)
    std::cout << *foo_heap->pop();
  std::cout << std::endl;
}
*/

