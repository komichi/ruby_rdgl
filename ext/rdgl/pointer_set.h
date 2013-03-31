#include <iostream>
#include <set>
#include <algorithm>

namespace RDGL {

/******************************************************************************/
/* PointerSet *****************************************************************/
/******************************************************************************/

// For example, implement the following (where T == Vertex)
// bool operator()(Vertex * a, Vertex * b) const { return *a < *b; }

template<typename T>
class PointerSet : public std::set<T *, T>
{
  public:
    PointerSet(void) : std::set<T *, T>() { ; }
    void insert(T & v);
    T * find(T & v);
    T * remove(T & e);
    template <typename U>
    friend std::ostream & operator<<(std::ostream & os, PointerSet<U> & s);
};

template <typename T>
void
PointerSet<T>::insert(T & e)
{
  std::set<T *, T>::insert(&e);
}

template <typename T>
T *
PointerSet<T>::find(T & e)
{
  typename PointerSet<T>::iterator i = std::set<T *, T>::find(&e);
  if (i == this->end()) return NULL;
  else return *i;
}

template <typename T>
T *
PointerSet<T>::remove(T & e)
{
  T * obj = NULL;
  typename PointerSet<T>::iterator i = std::set<T *, T>::find(&e);
  if (i != this->end())
  {
    obj = *i;
    std::set<T *, T>::erase(i);
  }
  return obj;
}

template <typename T>
std::ostream &
operator<<(std::ostream & os, PointerSet<T> & s)
{
  typename PointerSet<T>::iterator i;
  os << "Set: " << std::endl;
  for (i=s.begin() ; i != s.end(); i++)
    os << "\t" << **i;
  return os;
}

}

/*

class Foo
{
  public:
    int i;
    Foo(void) { i = 0; }
    Foo(int j) { i = j; }
    Foo(const Foo & f) { i = f.i; }
    bool operator<(const Foo & b) const;
    bool operator>(const Foo & b) const;
    bool operator()(const Foo * a, Foo * b) const;
    bool operator()(Foo * a, Foo * b) const;
    bool operator()(Foo * a, Foo * b);
    bool operator==(const Foo & b) const;
    bool operator!=(const Foo & b) const;
    friend std::ostream & operator<<(std::ostream & os, Foo & f);
};

bool Foo::operator<(const Foo & b) const { return i < b.i; }
bool Foo::operator>(const Foo & b) const { return i > b.i; }
bool Foo::operator()(const Foo * a, Foo * b) const { return *a < *b; }
bool Foo::operator()(Foo * a, Foo * b) const { return *a < *b; }
bool Foo::operator()(Foo * a, Foo * b) { return *a < *b; }
bool Foo::operator==(const Foo & b) const { return i == b.i; }
bool Foo::operator!=(const Foo & b) const { return i != b.i; }

std::ostream & operator<<(std::ostream & os, Foo & f)
{
  os << f.i << std::endl;
}


main(void)
{
  Foo f1(9), f2(1), f3(8), f4(2);
  Foo f5(7), f6(3), f7(6), f8(4);
  Foo f9(4), f10(4);
  PointerSet<Foo> * foo_set = new PointerSet<Foo>;
  foo_set->insert(f1);
  foo_set->insert(f1);
  foo_set->insert(f2);
  foo_set->insert(f4);
  foo_set->insert(f6);
  foo_set->insert(f3);
  foo_set->insert(f5);
  foo_set->insert(f2);
  foo_set->insert(f8);
  foo_set->insert(f8);
  foo_set->insert(f8);
  foo_set->insert(f9);
  foo_set->insert(f10);
  std::cout << *foo_set;
}
*/
