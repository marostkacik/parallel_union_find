#include <bits/stdc++.h>

namespace parallel_union_find::node::lockfree
{
using namespace std;

template<typename R>
class Node;

template<typename R>
class UnionNode;

template<typename R>
class ListNode;

template<typename R>
class Node
{
public: // methods
    Node(R * const);

    Node<R>* find_set(); // waitfree
    bool     same_set(Node<R>* other); // waitfree
    bool     union_set(Node<R>* other); // lockfree

    void     add_mask(uint64_t mask); // waitfree
    uint64_t get_mask(); // waitfree

    size_t   active_list_length(); // waitfree
    Node<R>* get_random_active_node(size_t seed); // waitfree

    R*       get_representative(); // waitfree
    void     mark_as_dead(); // waitfree

private: // data
    UnionNode<R> _union_node;
    ListNode<R>  _list_node;

    std::atomic<bool> _spin_lock;
    R * const         _representative;

private: // private methods
    bool lock();
    void unlock();

private: // friends
    friend UnionNode<R>;
    friend ListNode<R>;

private: // requirements for template parameter
    static_assert(true);
};

template<typename R>
class UnionNode
{
public:
    UnionNode(Node<R>*);

    Node<R>* find_set();              // waitfree
    bool     same_set(Node<R>*);      // waitfree
    void     merge_set(Node<R>*);     // atomicity and correct parameters must be guaranteed by caller
    void     add_mask(uint64_t mask); // waitfree

private:
    std::atomic<Node<R>*> _parent;
    std::atomic<uint64_t> _mask;
    uint64_t              _size;
};

template<typename R>
class ListNode
{
    ListNode(Node<R>*);

    std::atomic<Node<R>*> _start_node; // nullptr for no start
    std::atomic<Node<R>*> _last_node;  // nullptr for no end
    std::atomic<Node<R>*> _next_node;  // nullptr for no next
    std::atomic<Node<R>*> _prev_node;  // nullptr for no prev
    std::atomic<bool>     _dead;       // if marked as true, can be popped from linked list
    std::atomic<size_t>   _length;

    void append_list(Node<R>* other); // atomicity and correct parameters must be guaranteed by caller
    static Node<R>* shift_to_next_active(Node<R>* head_node, Node<R>*);
    static Node<R>* get_node(Node<R>* head_node, size_t n);
};

#include "on_the_fly_scc_node.tpp.tpp"
}
