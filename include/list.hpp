#pragma once
#include <cstddef>

// TODO: Ensure copying value_type will be exception safe.
// TODO: Check for memory leaks
// TODO: Make it be more like std::list
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
    explicit iterator(Node* node) : m_node{node} {}

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
      this->   operator++();
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
      this->   operator--();
      return it;
    }

  private:
    Node* m_node;
  };

  class const_iterator {
  public:
    explicit const_iterator(iterator it) : m_it{it} {}

    friend bool operator==(const iterator& lhs, const iterator& rhs)
    {
      return lhs.m_it == rhs.m_it;
    }

    friend bool operator!=(const iterator& lhs, const iterator& rhs)
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
      this->         operator++();
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
      this->         operator--();
      return it;
    }

  private:
    iterator m_it;
  };

  List()
    : m_begin{new Node{value_type{}, nullptr, nullptr}}
    , m_end{m_begin}
    , m_size{0}
  {
  }

  ~List()
  {
    Node* node{m_begin};

    while (node != m_end) {
      node = node->next;
      delete node->prev;
    }

    delete m_end;
  }

  size_type size() const { return m_size; }

  [[nodiscard]] bool empty() const { return size() == 0; }

  iterator begin() { return iterator{m_begin}; }

  const_iterator begin() const { return const_cast<this_type*>(this)->begin(); }

  iterator end() { return iterator{m_end}; }

  const_iterator end() const { return const_cast<this_type*>(this)->end(); }

  void push_back(const_reference element)
  {
    if (empty()) {
      addFirstNode(element);
      return;
    }

    Node* newNode{new Node{element, m_end->prev, m_end}};
    m_end->prev->next = newNode;
    m_end->prev       = newNode;
    ++m_size;
  }

  void push_front(const_reference element)
  {
    if (empty()) {
      addFirstNode(element);
      return;
    }

    Node* newNode{new Node{element, nullptr, m_begin}};
    m_begin = newNode;
    ++m_size;
  }

  void pop_back()
  {
    if (empty()) { return; }

    Node* toRemove{m_end->prev};
    Node* prev{toRemove->prev};
    prev->next  = m_end;
    m_end->prev = prev;
    delete toRemove;
  }

  void pop_front()
  {
    if (empty()) { return; }

    Node* toRemove{m_begin};
    Node* next{m_begin->next};
    next->prev = nullptr;
    m_begin    = next;
    delete toRemove;
  }

private:
  void addFirstNode(const_reference element)
  {
    m_begin     = new Node{element, nullptr, m_end};
    m_end->prev = m_begin;
    ++m_size;
  }

  Node*     m_begin;
  Node*     m_end;
  size_type m_size;
};
