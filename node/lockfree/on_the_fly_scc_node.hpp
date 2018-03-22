#include <bits/stdc++.h>

namespace parallel_union_find::noed::lockfree
{
using namespace std;

template<typename R>
struct Node;

template<typename R>
struct UnionNode;

template<typename R>
struct ListNode;

template<typename R>
struct Node
{
public:
    Node(R * const);

    Node<R>* find_set(); // waitfree
    bool     same_set(Node<R>* other); // waitfree
    bool     union_set(Node<R>* other); // lockfree

    void     add_mask(uint64_t mask); // waitfree
    uint64_t get_mask(); // waitfree

    size_t   active_list_length(); // waitfree
    Node<R>* get_random_active_node(size_t seed); // waitfree

    R*       get_representative();
    void     mark_as_dead();

public:
    UnionNode<R> _union_node;
    ListNode<R>  _list_node;

    std::atomic<bool> _spin_lock;
    R * const         _representative;

private:
    bool lock();
    void unlock();

private:
    static_assert(true);
};

template<typename R>
struct UnionNode
{
    UnionNode(Node<R>*);

    Node<R>* find_set();              // waitfree
    bool     same_set(Node<R>*);      // waitfree
    void     merge_set(Node<R>*);     // atomicity and correct parameters must be guaranteed by caller
    void     add_mask(uint64_t mask); // waitfree

    std::atomic<Node<R>*> _parent;
    std::atomic<uint64_t> _mask;
    uint64_t              _size;
};

template<typename R>
struct ListNode
{
    ListNode(Node<R>*);

    std::atomic<Node<R>*> _start_node; // nullptr for no start
    std::atomic<Node<R>*> _last_node;  // nullptr for no end
    std::atomic<Node<R>*> _next_node;  // nullptr for no next
    std::atomic<Node<R>*> _prev_node;  // nullptr for no prev
    std::atomic<bool>     _dead;       // if marked as true, can be popped from linked list
    std::atomic<size_t>   _length;

    void append_list(Node<R>* other); // atomicity and correct parameters must be guaranteed by caller
    static Node<R>* shift_to_next_active(Node<R>* head_node, Node<R>*); // if act is active, no shift is done, returns nullptr if such node does not exists, also tries to pop dead nodes
    static Node<R>* get_node(Node<R>* head_node, size_t n); // waitfree, returns nth active node in list, or nullptr if n is bigger or equal to number of nodes in list. Also pops dead nodes, but only if head_node is still head node. Returns nullptr if there's no active node. 0 indexed
};

#include "on_the_fly_scc_node.tpp.tpp"
}
