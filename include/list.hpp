#ifndef INCG_LIST_HPP
#define INCG_LIST_HPP
#include <cstddef>

#include <algorithm>
#include <functional>
#include <iterator>

extern std::unordered_set<void*> newed;
extern std::unordered_set<void*> deleted;

// TODO: Check for memory leaks
// TODO: Write tests.
template<typename Ty>
class List {
public:
  using value_type = Ty;

private:
  struct Node {
    value_type value;
    Node*      prev;
    Node*      next;
  };

public:
  using this_type       = List;
  using size_type       = std::size_t;
  using reference       = value_type&;
  using const_reference = const value_type&;

  class iterator {
  public:
    friend class this_type;

    /* IMPLICIT */ iterator(Node* node) : m_node{node} {}

    friend bool operator==(const iterator& lhs, const iterator& rhs)
    {
      return lhs.m_node == rhs.m_node;
    }

    friend bool operator!=(const iterator& lhs, const iterator& rhs)
    {
      return !(lhs == rhs);
    }

    value_type& operator*() { return m_node->value; }

    iterator& operator++()
    {
      m_node = m_node->next;
      return *this;
    }

    iterator operator++(int)
    {
      iterator it{*this};
      ++(*this);
      return it;
    }

    iterator& operator--()
    {
      m_node = m_node->prev;
      return *this;
    }

    iterator operator--(int)
    {
      iterator it{*this};
      --(*this);
      return it;
    }

  private:
    Node* m_node;
  };

  class const_iterator {
  public:
    friend class this_type;

    /* IMPLICIT */ const_iterator(iterator it) : m_it{it} {}

    friend bool operator==(const const_iterator& lhs, const const_iterator& rhs)
    {
      return lhs.m_it == rhs.m_it;
    }

    friend bool operator!=(const const_iterator& lhs, const const_iterator& rhs)
    {
      return !(lhs == rhs);
    }

    const value_type& operator*() const { return *m_it; }

    const_iterator& operator++()
    {
      ++m_it;
      return *this;
    }

    const_iterator operator++(int)
    {
      const_iterator it{*this};
      ++(*this);
      return it;
    }

    const_iterator& operator--()
    {
      --m_it;
      return *this;
    }

    const_iterator operator--(int)
    {
      const_iterator it{*this};
      --(*this);
      return it;
    }

  private:
    iterator m_it;
  };

  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  friend auto operator<=>(const this_type& lhs, const this_type& rhs)
  {
    return std::lexicographical_compare_three_way(
      lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
  }

  List() : m_begin{nullptr}, m_end{nullptr}, m_size{0} { initialize(); }

  List(const this_type& other) : List{}
  {
    for (const value_type& element : other) { push_back(element); }
  }

  this_type& operator=(const this_type& other)
  {
    this_type newList{other};
    swap(newList);
    return *this;
  }

  ~List() { destroy(); }

  size_type size() const { return m_size; }

  [[nodiscard]] bool empty() const { return size() == 0; }

  reference front() { return *begin(); }

  const_reference front() const
  {
    return const_cast<this_type*>(this)->front();
  }

  reference back() { return *rbegin(); }

  const_reference back() const { return const_cast<this_type*>(this)->back(); }

  iterator begin() { return iterator{m_begin}; }

  const_iterator begin() const { return const_cast<this_type*>(this)->begin(); }

  const_iterator cbegin() const { return begin(); }

  iterator end() { return iterator{m_end}; }

  const_iterator end() const { return const_cast<this_type*>(this)->end(); }

  const_iterator cend() const { return end(); }

  reverse_iterator rbegin() { return reverse_iterator{end()}; }

  const_reverse_iterator rbegin() const
  {
    return const_cast<this_type*>(this)->rbegin();
  }

  const_reverse_iterator crbegin() const { return rbegin(); }

  reverse_iterator rend() { return reverse_iterator{begin()}; }

  const_reverse_iterator rend() const
  {
    return const_cast<this_type*>(this)->rend();
  }

  const_reverse_iterator crend() const { return rend(); }

  void sort() { sort(std::less<value_type>{}); }

  template<typename BinaryComparator>
  void sort(BinaryComparator binaryComparator)
  {
    for (Node* node{m_begin}; node->next != nullptr; node = node->next) {
      for (Node* next{node->next}; next != nullptr; next = next->next) {
        if (std::invoke(binaryComparator, next->value, node->value)) {
          using std::swap;
          swap(node->value, next->value);
        }
      }
    }
  }

  void push_back(const_reference element) { insert(end(), element); }

  void push_front(const_reference element) { insert(begin(), element); }

  void pop_back()
  {
    if (empty()) { return; }

    erase(std::prev(end()));
  }

  void pop_front()
  {
    if (empty()) { return; }

    erase(begin());
  }

  iterator insert(const_iterator pos, const_reference value)
  {
    Node* node{pos.m_it.m_node};
    Node* prev{node->prev};
    Node* newNode{nullptr};

    try {
      newNode = new Node{value, prev, node};
      newed.insert(newNode);
    }
    catch (...) {
      delete newNode;
      deleted.insert(newNode);
      throw;
    }

    if (node == m_begin) { m_begin = newNode; }
    else {
      prev->next = newNode;
    }

    node->prev = newNode;
    ++m_size;

    iterator it{newNode};
    return it;
  }

  iterator erase(const_iterator pos)
  {
    Node* node{pos.m_it.m_node};
    Node* next{node->next};

    if (node == m_begin) {
      m_begin    = next;
      next->prev = nullptr;
    }
    else {
      node->prev->next = next;
      next->prev       = node->prev;
    }

    --m_size;
    delete node;
    deleted.insert(node);
    return iterator{next};
  }

  template<typename UnaryPredicate>
  size_type remove_if(UnaryPredicate unaryPredicate)
  {
    size_type elementsRemoved{0};

    iterator it{begin()};

    while (it != end()) {
      if (std::invoke(unaryPredicate, *it)) {
        it = erase(it);
        ++elementsRemoved;
      }
      else {
        ++it;
      }
    }

    return elementsRemoved;
  }

  size_type remove(const_reference value)
  {
    return remove_if(
      [&value](const_reference element) { return element == value; });
  }

  void resize(size_type count, const value_type& value)
  {
    while (count > size()) { push_back(value); }

    while (count < size()) { pop_back(); }
  }

  void resize(size_type count) { resize(count, value_type{}); }

  void clear()
  {
    destroy();
    initialize();
  }

  void swap(this_type& other) noexcept
  {
    std::swap(m_begin, other.m_begin);
    std::swap(m_end, other.m_end);
    std::swap(m_size, other.m_size);
  }

private:
  void initialize()
  {
    try {
      m_begin = new Node{value_type{}, nullptr, nullptr};
      newed.insert(m_begin);
      m_end = m_begin;
    }
    catch (...) {
      delete m_begin;
      deleted.insert(m_begin);
      throw;
    }
  }

  void destroy()
  {
    Node* node{m_begin};

    while (node != m_end) {
      node = node->next;
      delete node->prev;
      deleted.insert(node->prev);
    }

    delete m_end;
    deleted.insert(m_end);

    m_begin = nullptr;
    m_end   = nullptr;
    m_size  = 0;
  }

  Node*     m_begin;
  Node*     m_end;
  size_type m_size;
};

template<typename Ty>
void swap(List<Ty>& lhs, List<Ty>& rhs) noexcept
{
  lhs.swap(rhs);
}
#endif // INCG_LIST_HPP
