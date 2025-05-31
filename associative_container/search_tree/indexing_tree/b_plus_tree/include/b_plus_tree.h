#include <iterator>
#include <utility>
#include <vector>
#include <boost/container/static_vector.hpp>
#include <concepts>
#include <stack>
#include <pp_allocator.h>
#include <search_tree.h>
#include <initializer_list>
#include <logger_guardant.h>

#ifndef MP_OS_B_PLUS_TREE_H
#define MP_OS_B_PLUS_TREE_H

template <typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5>
class BP_tree final : private logger_guardant, private compare
{
public:

    using tree_data_type = std::pair<tkey, tvalue>;
    using tree_data_type_const = std::pair<const tkey, tvalue>;
    using value_type = tree_data_type;

private:

    static constexpr const size_t minimum_keys_in_node = t - 1;
    static constexpr const size_t maximum_keys_in_node = 2 * t - 1;

    // region comparators declaration

    inline bool compare_keys(const tkey& lhs, const tkey& rhs) const;
    inline bool compare_pairs(const tree_data_type& lhs, const tree_data_type& rhs) const;

    // endregion comparators declaration

    struct bptree_node_base
    {
        bool _is_terminate;

        bptree_node_base() noexcept;
        virtual ~bptree_node_base() =default;
    };

    struct bptree_node_term : public bptree_node_base
    {
        bptree_node_term* _next;
//        std::vector<tree_data_type, pp_allocator<tree_data_type>> _data;
//
//        bptree_node_term(pp_allocator<tree_data_type> al);

        boost::container::static_vector<tree_data_type, maximum_keys_in_node + 1> _data;
        bptree_node_term() noexcept;
    };

    struct bptree_node_middle : public bptree_node_base
    {
//        std::vector<tkey, pp_allocator<tkey>> _keys;
//        std::vector<bptree_node_base*, pp_allocator<bptree_node_base*>> _pointers;
//
//        bptree_node_middle(pp_allocator<tkey> al);


        boost::container::static_vector<tkey, maximum_keys_in_node + 1> _keys;
        boost::container::static_vector<bptree_node_base*, maximum_keys_in_node + 2> _pointers;
        bptree_node_middle() noexcept;
    };

    pp_allocator<value_type> _allocator;
    logger* _logger;
    bptree_node_base* _root;
    size_t _size;

    logger* get_logger() const noexcept override;
    pp_allocator<value_type> get_allocator() const noexcept;

public:

    // region constructors declaration

    explicit BP_tree(const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    explicit BP_tree(pp_allocator<value_type> alloc, const compare& comp = compare(), logger *logger = nullptr);

    template<input_iterator_for_pair<tkey, tvalue> iterator>
    explicit BP_tree(iterator begin, iterator end, const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    BP_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    // endregion constructors declaration

    // region five declaration

    BP_tree(const BP_tree& other);

    BP_tree(BP_tree&& other) noexcept;

    BP_tree& operator=(const BP_tree& other);

    BP_tree& operator=(BP_tree&& other) noexcept;

    ~BP_tree() noexcept override;

    void delete_subtree(bptree_node_base* root);

    // endregion five declaration

    // region iterators declaration

    class bptree_iterator;
    class bptree_const_iterator;

    class bptree_iterator final
    {
        bptree_node_term* _node;
        size_t _index;

    public:
        using value_type = tree_data_type;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = bptree_iterator;

        friend class BP_tree;
        friend class bptree_const_iterator;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t current_node_keys_count() const noexcept;
        size_t index() const noexcept;

        explicit bptree_iterator(bptree_node_term* node = nullptr, size_t index = 0);

    };

    class bptree_const_iterator final
    {
        const bptree_node_term* _node;
        size_t _index;

    public:

        using value_type = tree_data_type;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = bptree_const_iterator;

        friend class BP_tree;
        friend class bptree_iterator;

        bptree_const_iterator(const bptree_iterator& it) noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t current_node_keys_count() const noexcept;
        size_t index() const noexcept;

        explicit bptree_const_iterator(const bptree_node_term* node = nullptr, size_t index = 0);
    };

    friend class btree_iterator;
    friend class btree_const_iterator;

    // endregion iterators declaration

    // region element access declaration

    /*
     * Returns a reference to the mapped value of the element with specified key. If no such element exists, an exception of type std::out_of_range is thrown.
     */
    tvalue& at(const tkey&);
    const tvalue& at(const tkey&) const;

    /*
     * If key not exists, makes default initialization of value
     */
    tvalue& operator[](const tkey& key);
    tvalue& operator[](tkey&& key);

    // endregion element access declaration
    
    // region iterator begins declaration

    bptree_iterator begin();
    bptree_iterator end();

    bptree_const_iterator begin() const;
    bptree_const_iterator end() const;

    bptree_const_iterator cbegin() const;
    bptree_const_iterator cend() const;

    // endregion iterator begins declaration

    // region lookup declaration

    size_t size() const noexcept;
    bool empty() const noexcept;

    /*
     * Returns end() if not exist
     */

    bptree_iterator find(const tkey& key);
    bptree_const_iterator find(const tkey& key) const;

    bptree_iterator lower_bound(const tkey& key);
    bptree_const_iterator lower_bound(const tkey& key) const;

    bptree_iterator upper_bound(const tkey& key);
    bptree_const_iterator upper_bound(const tkey& key) const;

    bool contains(const tkey& key) const;

    // endregion lookup declaration

    // region modifiers declaration

    void clear() noexcept;

    /*
     * Does nothing if key exists, delegates to emplace.
     * Second return value is true, when inserted
     */
    std::pair<bptree_iterator, bool> insert(const tree_data_type& data);
    std::pair<bptree_iterator, bool> insert(tree_data_type&& data);

    template <typename ...Args>
    std::pair<bptree_iterator, bool> emplace(Args&&... args);

    /*
     * Updates value if key exists, delegates to emplace.
     */
    bptree_iterator insert_or_assign(const tree_data_type& data);
    bptree_iterator insert_or_assign(tree_data_type&& data);

    template <typename ...Args>
    bptree_iterator emplace_or_assign(Args&&... args);

    /*
     * Return iterator to node next ro removed or end() if key not exists
     */
    bptree_iterator erase(bptree_iterator pos);
    bptree_iterator erase(bptree_const_iterator pos);

    bptree_iterator erase(bptree_iterator beg, bptree_iterator en);
    bptree_iterator erase(bptree_const_iterator beg, bptree_const_iterator en);


    bptree_iterator erase(const tkey& key);

    // endregion modifiers declaration
};

// template magic

template<std::input_iterator iterator, compator<typename std::iterator_traits<iterator>::value_type::first_type> compare = std::less<typename std::iterator_traits<iterator>::value_type::first_type>,
        std::size_t t = 5, typename U>
BP_tree(iterator begin, iterator end, const compare &cmp = compare(), pp_allocator<U> = pp_allocator<U>(),
        logger *logger = nullptr) -> BP_tree<typename std::iterator_traits<iterator>::value_type::first_type, typename std::iterator_traits<iterator>::value_type::second_type, compare, t>;

template<typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5, typename U>
BP_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare &cmp = compare(), pp_allocator<U> = pp_allocator<U>(),
        logger *logger = nullptr) -> BP_tree<tkey, tvalue, compare, t>;

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool BP_tree<tkey, tvalue, compare, t>::compare_pairs(const BP_tree::tree_data_type &lhs,
                                                     const BP_tree::tree_data_type &rhs) const
{
    return compare_keys(lhs.first, rhs.first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::bptree_node_base::bptree_node_base() noexcept
    : _is_terminate(true)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::bptree_node_term::bptree_node_term() noexcept
    : _next(nullptr)
{
    bptree_node_base::_is_terminate = true;
    _data.clear();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::bptree_node_middle::bptree_node_middle() noexcept
{
    bptree_node_base::_is_terminate = false;
    _keys.clear();
    _pointers.clear();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
logger * BP_tree<tkey, tvalue, compare, t>::get_logger() const noexcept
{
    return _logger;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
pp_allocator<typename BP_tree<tkey, tvalue, compare, t>::value_type> BP_tree<tkey, tvalue, compare, t>::
get_allocator() const noexcept
{
    return _allocator;
}

// + iterator

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator::reference BP_tree<tkey, tvalue, compare, t>::
bptree_iterator::operator*() const noexcept
{
    return _node->_data[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator::pointer BP_tree<tkey, tvalue, compare, t>::bptree_iterator
::operator->() const noexcept
{
    if (_node == nullptr) {
        return nullptr;
    }
    return &(_node->_data[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator::self & BP_tree<tkey, tvalue, compare, t>::bptree_iterator::
operator++()
{   
    if (_node == nullptr) {
        return *this;
    }
    if (_index == current_node_keys_count() - 1) {
        _index = 0;
        _node = _node->_next;
    } else {
        _index++;
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator::self BP_tree<tkey, tvalue, compare, t>::bptree_iterator::
operator++(int)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool BP_tree<tkey, tvalue, compare, t>::bptree_iterator::operator==(const self &other) const noexcept
{
    return (_node == other._node) && (_index == other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool BP_tree<tkey, tvalue, compare, t>::bptree_iterator::operator!=(const self &other) const noexcept
{
    return (_node != other._node) || (_index != other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t BP_tree<tkey, tvalue, compare, t>::bptree_iterator::current_node_keys_count() const noexcept
{
    if (_node == nullptr) {
        return 0;
    }
    return _node->_data.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t BP_tree<tkey, tvalue, compare, t>::bptree_iterator::index() const noexcept
{
    return _index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::bptree_iterator::bptree_iterator(bptree_node_term *node, size_t index)
    : _node(node)
    , _index(index)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator::bptree_const_iterator(const bptree_iterator &it) noexcept
    : _node(it._node)
    , _index(it._index)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator::reference BP_tree<tkey, tvalue, compare, t>::
bptree_const_iterator::operator*() const noexcept
{
    return _node->_data[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator::pointer BP_tree<tkey, tvalue, compare, t>::
bptree_const_iterator::operator->() const noexcept
{
    if (_node == nullptr) {
        return nullptr;
    }
    return &(_node->_data[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator::self & BP_tree<tkey, tvalue, compare, t>::
bptree_const_iterator::operator++()
{
    if (_node == nullptr) {
        return *this;
    }
    if (_index == current_node_keys_count() - 1) {
        _index = 0;
        _node = _node->_next;
    } else {
        _index++;
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator::self BP_tree<tkey, tvalue, compare, t>::
bptree_const_iterator::operator++(int)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator::operator==(const self &other) const noexcept
{
    return (_node == other._node) && (_index == other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator::operator!=(const self &other) const noexcept
{
    return (_node != other._node) && (_index != other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator::current_node_keys_count() const noexcept
{
    if (_node == nullptr) {
        return 0;
    }
    return _node->_data.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator::index() const noexcept
{
    return _index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator::bptree_const_iterator(const bptree_node_term *node, size_t index)
    : _node(node)
    , _index(index)
{}

// access

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue & BP_tree<tkey, tvalue, compare, t>::at(const tkey &key)
{
    auto it = find(key);
    return it->second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
const tvalue & BP_tree<tkey, tvalue, compare, t>::at(const tkey &key) const
{
    auto it = find(key);
    return it->second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue & BP_tree<tkey, tvalue, compare, t>::operator[](const tkey &key)
{
    auto it = find(key);
    return it->second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue & BP_tree<tkey, tvalue, compare, t>::operator[](tkey &&key)
{
    auto it = find(key);
    return it->second;
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool BP_tree<tkey, tvalue, compare, t>::compare_keys(const tkey &lhs, const tkey &rhs) const
{
    return compare::operator()(lhs, rhs);
}

// + constructor

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::BP_tree(const compare& cmp, pp_allocator<value_type> alloc, logger* logger)
    : compare(cmp)
    , _root(nullptr)
    , _logger(logger)
    , _size(0)
    , _allocator(alloc)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::BP_tree(pp_allocator<value_type> alloc, const compare& cmp, logger* logger)
    : compare(cmp)
    , _root(nullptr)
    , _logger(logger)
    , _size(0)
    , _allocator(alloc)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<input_iterator_for_pair<tkey, tvalue> iterator>
BP_tree<tkey, tvalue, compare, t>::BP_tree(iterator begin, iterator end, const compare& cmp, pp_allocator<value_type> alloc, logger* logger)
    : compare(cmp)
    , _root(nullptr)
    , _logger(logger)
    , _size(0)
    , _allocator(alloc)
{
    for(;begin != end; begin++) {
        insert(*begin);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::BP_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp, pp_allocator<value_type> alloc, logger* logger)
    : compare(cmp)
    , _root(nullptr)
    , _logger(logger)
    , _size(0)
    , _allocator(alloc)
{
    for (auto &p : data) {
        insert(p);
    }
}

// + rule of 5

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::BP_tree(const BP_tree& other)
    : compare(other)
    , _root(nullptr)
    , _logger(other._logger)
    , _size(0)
    , _allocator(other._allocator)
{
    for (auto it = other.begin(); it != other.end(); ++it) {
        insert(*it);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::BP_tree(BP_tree&& other) noexcept
    : compare(std::move(other))
    , _root(std::move(other._root))
    , _logger(std::move(other._logger))
    , _size(std::move(other._size))
    , _allocator(std::move(other._allocator))
{
    other._root = nullptr;
    other._size = 0;
    other._logger = nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>& BP_tree<tkey, tvalue, compare, t>::operator=(const BP_tree& other)
{
    if (this != &other) {
        delete_subtree(_root);

        compare::operator=(other);
        _allocator = other._allocator;
        _logger = other._logger;
        _root = nullptr;
        _size = 0;

        for (auto it = other.begin(); it != other.end(); ++it) {
            insert(*it);
        }
    }
    return *this;

}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>& BP_tree<tkey, tvalue, compare, t>::operator=(BP_tree&& other) noexcept
{
    if (this != &other) {

        delete_subtree(_root);

        compare::operator=(std::move(other));
        _root = std::move(other._root);
        _size = std::move(other._size);
        _logger = std::move(other._logger);
        _allocator = std::move(other._allocator);

        other._root = nullptr;
        other._size = 0;
        other._logger = nullptr;
    }
    return *this; 
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::~BP_tree() noexcept
{
    delete_subtree(_root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void BP_tree<tkey, tvalue, compare, t>::delete_subtree(BP_tree<tkey, tvalue, compare, t>::bptree_node_base* root)
{
    if (!root) return;
    
    if (root->_is_terminate) {
        // std::cout << "term" << static_cast<bptree_node_term*>(root)->_data[0].first << std::endl;
        _allocator.delete_object(static_cast<bptree_node_term*>(root));
    } else {
        auto mid = static_cast<bptree_node_middle*>(root);
        // std::cout << "mid"  << mid->_keys[0] << std::endl;

        for (auto cur: mid->_pointers) {
            delete_subtree(cur);
        }

        _allocator.delete_object(mid);
    }
}

// + iterator begin end

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::begin()
{
    if (_root == nullptr) {
        return bptree_iterator(nullptr, 0);
    }
    bptree_node_base* cur_node = _root;
    while (!cur_node->_is_terminate) {
        cur_node = (static_cast<bptree_node_middle*>(cur_node))->_pointers[0];
    }
    return bptree_iterator(static_cast<bptree_node_term*>(cur_node), 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::end()
{
    return bptree_iterator(nullptr, 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator BP_tree<tkey, tvalue, compare, t>::begin() const
{
    if (_root == nullptr) {
        return bptree_const_iterator(nullptr, 0);
    }
    bptree_node_base* cur_node = _root;
    while (!cur_node->_is_terminate) {
        cur_node = (static_cast<bptree_node_middle*>(cur_node))->_pointers[0];
    }
    return bptree_const_iterator(static_cast<bptree_node_term*>(cur_node), 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator BP_tree<tkey, tvalue, compare, t>::end() const
{
    return bptree_const_iterator(nullptr, 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator BP_tree<tkey, tvalue, compare, t>::cbegin() const
{
    if (_root == nullptr) {
        return bptree_const_iterator(nullptr, 0);
    }
    bptree_node_base* cur_node = _root;
    while (!cur_node->_is_terminate) {
        cur_node = (static_cast<bptree_node_middle*>(cur_node))->_pointers[0];
    }
    return bptree_const_iterator(static_cast<bptree_node_term*>(cur_node), 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator BP_tree<tkey, tvalue, compare, t>::cend() const
{
    return bptree_const_iterator(nullptr, 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t BP_tree<tkey, tvalue, compare, t>::size() const noexcept
{
    return _size;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool BP_tree<tkey, tvalue, compare, t>::empty() const noexcept
{
    return _size == 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void BP_tree<tkey, tvalue, compare, t>::clear() noexcept
{
    delete_subtree(_root);
    _size = 0;
}

// + find

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::find(const tkey& key)
{
    if (_root == nullptr) {
        return end();
    }
    bptree_node_base* cur_node_b = _root;
    bptree_node_middle* cur_node;
    while (!cur_node_b->_is_terminate) {
        cur_node = static_cast<bptree_node_middle*>(cur_node_b);
        size_t low = 0;
        size_t high = cur_node->_keys.size() + 1;
        size_t mid;
        while (low + 1 < high) {
            mid = low + (high - low) / 2;
            if (compare_keys(key, cur_node->_keys[mid - 1])) {
                high = mid;
            } else {
                low = mid;
            }
        }
        cur_node_b = cur_node->_pointers[low];
    }
    bptree_node_term* leaf = static_cast<bptree_node_term*>(cur_node_b);
    size_t low = 0;
    size_t high = leaf->_data.size() + 1;
    size_t mid;
    while (low + 1 < high) {
        mid = low + (high - low) / 2;
        if (compare_keys(key, leaf->_data[mid - 1].first)) {
            high = mid;
        } else if (compare_keys(leaf->_data[mid - 1].first, key)) {
            low = mid;
        } else {
            return bptree_iterator(leaf, mid - 1);
        }   
    }
    return end();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator BP_tree<tkey, tvalue, compare, t>::find(const tkey& key) const
{
    if (_root == nullptr) {
        return end();
    }
    bptree_node_base* cur_node_b = _root;
    bptree_node_middle* cur_node;
    while (!cur_node_b->_is_terminate) {
        cur_node = static_cast<bptree_node_middle*>(cur_node_b);
        size_t low = 0;
        size_t high = cur_node->_keys.size() + 1;
        size_t mid;
        while (low + 1 < high) {
            mid = low + (high - low) / 2;
            if (compare_keys(key, cur_node->_keys[mid - 1])) {
                high = mid;
            } else {
                low = mid;
            }
        }
        cur_node_b = cur_node->_pointers[low];
    }
    bptree_node_term* leaf = static_cast<bptree_node_term*>(cur_node_b);
    size_t low = 0;
    size_t high = leaf->_data.size() + 1;
    size_t mid;
    while (low + 1 < high) {
        mid = low + (high - low) / 2;
        if (compare_keys(key, leaf->_data[mid - 1].first)) {
            high = mid;
        } else if (compare_keys(leaf->_data[mid - 1].first, key)) {
            low = mid;
        } else {
            return bptree_iterator(leaf, mid - 1);
        }   
    }
    return end();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool BP_tree<tkey, tvalue, compare, t>::contains(const tkey& key) const
{
    if (_root == nullptr) {
        return end();
    }
    bptree_node_base* cur_node_b = _root;
    bptree_node_middle* cur_node;
    while (!cur_node_b->_is_terminate) {
        cur_node = static_cast<bptree_node_middle*>(cur_node_b);
        size_t low = 0;
        size_t high = cur_node->_keys.size() + 1;
        size_t mid;
        while (low + 1 < high) {
            mid = low + (high - low) / 2;
            if (compare_keys(key, cur_node->_keys[mid - 1])) {
                high = mid;
            } else {
                low = mid;
            }
        }
        cur_node_b = cur_node->_pointers[low];
    }
    bptree_node_term* leaf = static_cast<bptree_node_term*>(cur_node_b);
    size_t low = 0;
    size_t high = leaf->_data.size();
    size_t mid;
    while (low + 1 < high) {
        mid = low + (high - low) / 2;
        if (compare_keys(key, leaf->_data[mid - 1].first)) {
            high = mid;
        } else if (compare_keys(leaf->_data[mid - 1].first, key)) {
            low = mid;
        } else {
            return true;
        }   
    }
    return false;
}

// + insert

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator, bool> BP_tree<tkey, tvalue, compare, t>::insert(
    const tree_data_type &data)
{
    // std::cout << "insert 0\n";

    std::stack<std::pair<bptree_node_middle*, size_t>> path {};
    if (_root == nullptr) {
        bptree_node_term* new_root = _allocator.template new_object<bptree_node_term>();
        new_root->_is_terminate = true;
        _root = static_cast<bptree_node_base*>(new_root);
        new_root->_next = nullptr;
        new_root->_data.push_back(data);
        _size++;
        return {bptree_iterator(new_root, 0), true};
    }

    // move to leaf

    // std::cout << "insert 1\n";

    bptree_node_base* cur_node_b = _root;
    bptree_node_middle* cur_node;

    while (!cur_node_b->_is_terminate) {
        cur_node = static_cast<bptree_node_middle*>(cur_node_b);
        size_t low = 0;
        size_t high = cur_node->_keys.size() + 1;
        size_t mid;
        while (low + 1 < high) {
            mid = low + (high - low) / 2;
            if (compare_keys(data.first, cur_node->_keys[mid - 1])) {
                high = mid;
            } else {
                low = mid;
            }
        }
        path.push(std::make_pair(cur_node, low));
        cur_node_b = cur_node->_pointers[low];
    }

    // find place

    // std::cout << "insert 2\n";

    size_t pos;

    bptree_node_term* leaf = static_cast<bptree_node_term*>(cur_node_b);
    size_t low = 0;
    size_t high = leaf->_data.size() + 1;
    size_t mid;
    while (low + 1 < high) {
        mid = low + (high - low) / 2;
        if (compare_keys(data.first, leaf->_data[mid - 1].first)) {
            high = mid;
        } else if (compare_keys(leaf->_data[mid - 1].first, data.first)) {
            low = mid;
        } else {
            return {bptree_iterator(leaf, mid - 1), false};
        }   
    }

    pos = low;

    leaf->_data.insert(leaf->_data.begin() + pos, data);
    _size++;

    // fix rule

    if (leaf->_data.size() <= maximum_keys_in_node) {
        return {bptree_iterator(leaf, pos), true};
    }

    // std::cout << "insert 3\n";

    const size_t split = t;

    // split leaf

    bptree_node_term* right_leaf = _allocator.template new_object<bptree_node_term>();
    right_leaf->_is_terminate = true;

    tkey cur_key = leaf->_data[split].first;
    right_leaf->_data.insert(
        right_leaf->_data.end(),
        leaf->_data.begin() + split,
        leaf->_data.end()
    );
    leaf->_data.erase(
        leaf->_data.begin() + split,
        leaf->_data.end()
    );

    right_leaf->_next = leaf->_next;
    leaf->_next = right_leaf;

    size_t index;

    if (path.empty()) {
        cur_node = _allocator.template new_object<bptree_node_middle>();
        cur_node->_is_terminate = false;
        cur_node->_pointers.push_back(static_cast<bptree_node_base*>(leaf));
        _root = static_cast<bptree_node_base*>(cur_node);
        index = 0;
    } else {
        cur_node = path.top().first;
        index = path.top().second;
    }

    cur_node->_keys.insert(
        cur_node->_keys.begin() + index,
        cur_key
    );
    cur_node->_pointers.insert(
        cur_node->_pointers.begin() + index + 1,
        static_cast<bptree_node_base*>(right_leaf)
    );

    std::cout << cur_node->_pointers[0]->_is_terminate << " " << cur_node->_pointers[1]->_is_terminate << std::endl; 

    // split middle

    // std::cout << "insert 4\n";

    bptree_node_middle *left_part, *right_part;
    while (cur_node->_keys.size() == maximum_keys_in_node) {
        cur_key = cur_node->_keys[split];
        path.pop();
        left_part = cur_node;
        if (path.empty()) {
            cur_node = _allocator.template new_object<bptree_node_middle>();
            cur_node->_is_terminate = false;
            cur_node->_pointers.push_back(static_cast<bptree_node_base*>(left_part));
            _root = cur_node;
            index = 0;
        } else {
            cur_node = path.top().first;
            index = path.top().second;
        }
        right_part = _allocator.template new_object<bptree_node_middle>();
        right_part->_is_terminate = false;
        right_part->_keys.insert(
            right_part->_keys.end(),
            left_part->_keys.begin() + split + 1,
            left_part->_keys.end()
        );
        right_part->_pointers.insert(
            right_part->_pointers.end(),
            left_part->_pointers.begin() + split + 1, 
            left_part->_pointers.end()
        );

        left_part->_keys.erase(
            left_part->_keys.begin() + split,
            left_part->_keys.end()
        );
        left_part->_pointers.erase(
            left_part->_pointers.begin() + split + 1,
            left_part->_pointers.end()
        );

        cur_node->_keys.insert(cur_node->_keys.begin() + index, cur_key);
        cur_node->_pointers.insert(cur_node->_pointers.begin() + index + 1, 1, static_cast<bptree_node_base*>(right_part));
       
    }
    // std::cout << "insert 5\n";
    return {bptree_iterator(leaf, pos), true};
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator, bool> BP_tree<tkey, tvalue, compare, t>::insert(tree_data_type&& data)
{
    // std::cout << "insert 0\n";

    std::stack<std::pair<bptree_node_middle*, size_t>> path {};
    if (_root == nullptr) {
        bptree_node_term* new_root = _allocator.template new_object<bptree_node_term>();
        new_root->_is_terminate = true;
        _root = static_cast<bptree_node_base*>(new_root);
        new_root->_next = nullptr;
        new_root->_data.push_back(data);
        _size++;
        return {bptree_iterator(new_root, 0), true};
    }

    // move to leaf

    // std::cout << "insert 1\n";

    bptree_node_base* cur_node_b = _root;
    bptree_node_middle* cur_node;

    while (!cur_node_b->_is_terminate) {
        cur_node = static_cast<bptree_node_middle*>(cur_node_b);
        size_t low = 0;
        size_t high = cur_node->_keys.size() + 1;
        size_t mid;
        while (low + 1 < high) {
            mid = low + (high - low) / 2;
            if (compare_keys(data.first, cur_node->_keys[mid - 1])) {
                high = mid;
            } else {
                low = mid;
            }
        }
        path.push(std::make_pair(cur_node, low));
        cur_node_b = cur_node->_pointers[low];
    }

    // find place

    // std::cout << "insert 2\n";

    size_t pos;

    bptree_node_term* leaf = static_cast<bptree_node_term*>(cur_node_b);
    size_t low = 0;
    size_t high = leaf->_data.size() + 1;
    size_t mid;
    while (low + 1 < high) {
        mid = low + (high - low) / 2;
        if (compare_keys(data.first, leaf->_data[mid - 1].first)) {
            high = mid;
        } else if (compare_keys(leaf->_data[mid - 1].first, data.first)) {
            low = mid;
        } else {
            return {bptree_iterator(leaf, mid - 1), false};
        }   
    }

    pos = low;

    leaf->_data.insert(leaf->_data.begin() + pos, data);
    _size++;

    // fix rule

    if (leaf->_data.size() <= maximum_keys_in_node) {
        return {bptree_iterator(leaf, pos), true};
    }

    // std::cout << "insert 3\n";

    const size_t split = t;

    // split leaf

    bptree_node_term* right_leaf = _allocator.template new_object<bptree_node_term>();
    right_leaf->_is_terminate = true;

    tkey cur_key = leaf->_data[split].first;
    right_leaf->_data.insert(
        right_leaf->_data.end(),
        leaf->_data.begin() + split,
        leaf->_data.end()
    );
    leaf->_data.erase(
        leaf->_data.begin() + split,
        leaf->_data.end()
    );

    right_leaf->_next = leaf->_next;
    leaf->_next = right_leaf;

    size_t index;

    if (path.empty()) {
        cur_node = _allocator.template new_object<bptree_node_middle>();
        cur_node->_is_terminate = false;
        cur_node->_pointers.push_back(static_cast<bptree_node_base*>(leaf));
        _root = static_cast<bptree_node_base*>(cur_node);
        index = 0;
    } else {
        cur_node = path.top().first;
        index = path.top().second;
    }

    cur_node->_keys.insert(
        cur_node->_keys.begin() + index,
        cur_key
    );
    cur_node->_pointers.insert(
        cur_node->_pointers.begin() + index + 1,
        static_cast<bptree_node_base*>(right_leaf)
    );

    // std::cout << cur_node->_pointers[0]->_is_terminate << " " << cur_node->_pointers[1]->_is_terminate << std::endl; 

    // split middle

    // std::cout << "insert 4\n";

    bptree_node_middle *left_part, *right_part;
    while (cur_node->_keys.size() == maximum_keys_in_node) {
        cur_key = cur_node->_keys[split];
        path.pop();
        left_part = cur_node;
        if (path.empty()) {
            cur_node = _allocator.template new_object<bptree_node_middle>();
            cur_node->_is_terminate = false;
            cur_node->_pointers.push_back(static_cast<bptree_node_base*>(left_part));
            _root = cur_node;
            index = 0;
        } else {
            cur_node = path.top().first;
            index = path.top().second;
        }
        right_part = _allocator.template new_object<bptree_node_middle>();
        right_part->_is_terminate = false;
        right_part->_keys.insert(
            right_part->_keys.end(),
            left_part->_keys.begin() + split + 1,
            left_part->_keys.end()
        );
        right_part->_pointers.insert(
            right_part->_pointers.end(),
            left_part->_pointers.begin() + split + 1, 
            left_part->_pointers.end()
        );

        left_part->_keys.erase(
            left_part->_keys.begin() + split,
            left_part->_keys.end()
        );
        left_part->_pointers.erase(
            left_part->_pointers.begin() + split + 1,
            left_part->_pointers.end()
        );

        cur_node->_keys.insert(cur_node->_keys.begin() + index, cur_key);
        cur_node->_pointers.insert(cur_node->_pointers.begin() + index + 1, 1, static_cast<bptree_node_base*>(right_part));
       
    }
    // std::cout << "insert 5\n";
    return {bptree_iterator(leaf, pos), true};
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template <typename ...Args>
std::pair<typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator, bool> BP_tree<tkey, tvalue, compare, t>::emplace(Args&&... args)
{
    tree_data_type data (std::forward<Args>(args) ...);
    return insert(std::move(data));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::insert_or_assign(const tree_data_type& data)
{
    // std::cout << "insert 0\n";

    std::stack<std::pair<bptree_node_middle*, size_t>> path {};
    if (_root == nullptr) {
        bptree_node_term* new_root = _allocator.template new_object<bptree_node_term>();
        new_root->_is_terminate = true;
        _root = static_cast<bptree_node_base*>(new_root);
        new_root->_next = nullptr;
        new_root->_data.push_back(data);
        _size++;
        return bptree_iterator(new_root, 0);
    }

    // move to leaf

    // std::cout << "insert 1\n";

    bptree_node_base* cur_node_b = _root;
    bptree_node_middle* cur_node;

    while (!cur_node_b->_is_terminate) {
        cur_node = static_cast<bptree_node_middle*>(cur_node_b);
        size_t low = 0;
        size_t high = cur_node->_keys.size() + 1;
        size_t mid;
        while (low + 1 < high) {
            mid = low + (high - low) / 2;
            if (compare_keys(data.first, cur_node->_keys[mid - 1])) {
                high = mid;
            } else {
                low = mid;
            }
        }
        path.push(std::make_pair(cur_node, low));
        cur_node_b = cur_node->_pointers[low];
    }

    // find place

    // std::cout << "insert 2\n";

    size_t pos;

    bptree_node_term* leaf = static_cast<bptree_node_term*>(cur_node_b);
    size_t low = 0;
    size_t high = leaf->_data.size() + 1;
    size_t mid;
    while (low + 1 < high) {
        mid = low + (high - low) / 2;
        if (compare_keys(data.first, leaf->_data[mid - 1].first)) {
            high = mid;
        } else if (compare_keys(leaf->_data[mid - 1].first, data.first)) {
            low = mid;
        } else {
            leaf->_data[mid - 1].second = data.second;
            return bptree_iterator(leaf, mid - 1);
        }   
    }

    pos = low;

    leaf->_data.insert(leaf->_data.begin() + pos, data);
    _size++;

    // fix rule

    if (leaf->_data.size() <= maximum_keys_in_node) {
        return bptree_iterator(leaf, pos);
    }

    // std::cout << "insert 3\n";

    const size_t split = t;

    // split leaf

    bptree_node_term* right_leaf = _allocator.template new_object<bptree_node_term>();
    right_leaf->_is_terminate = true;

    tkey cur_key = leaf->_data[split].first;
    right_leaf->_data.insert(
        right_leaf->_data.end(),
        leaf->_data.begin() + split,
        leaf->_data.end()
    );
    leaf->_data.erase(
        leaf->_data.begin() + split,
        leaf->_data.end()
    );

    right_leaf->_next = leaf->_next;
    leaf->_next = right_leaf;

    size_t index;

    if (path.empty()) {
        cur_node = _allocator.template new_object<bptree_node_middle>();
        cur_node->_is_terminate = false;
        cur_node->_pointers.push_back(static_cast<bptree_node_base*>(leaf));
        _root = static_cast<bptree_node_base*>(cur_node);
        index = 0;
    } else {
        cur_node = path.top().first;
        index = path.top().second;
    }

    cur_node->_keys.insert(
        cur_node->_keys.begin() + index,
        cur_key
    );
    cur_node->_pointers.insert(
        cur_node->_pointers.begin() + index + 1,
        static_cast<bptree_node_base*>(right_leaf)
    );

    std::cout << cur_node->_pointers[0]->_is_terminate << " " << cur_node->_pointers[1]->_is_terminate << std::endl; 

    // split middle

    // std::cout << "insert 4\n";

    bptree_node_middle *left_part, *right_part;
    while (cur_node->_keys.size() == maximum_keys_in_node) {
        cur_key = cur_node->_keys[split];
        path.pop();
        left_part = cur_node;
        if (path.empty()) {
            cur_node = _allocator.template new_object<bptree_node_middle>();
            cur_node->_is_terminate = false;
            cur_node->_pointers.push_back(static_cast<bptree_node_base*>(left_part));
            _root = cur_node;
            index = 0;
        } else {
            cur_node = path.top().first;
            index = path.top().second;
        }
        right_part = _allocator.template new_object<bptree_node_middle>();
        right_part->_is_terminate = false;
        right_part->_keys.insert(
            right_part->_keys.end(),
            left_part->_keys.begin() + split + 1,
            left_part->_keys.end()
        );
        right_part->_pointers.insert(
            right_part->_pointers.end(),
            left_part->_pointers.begin() + split + 1, 
            left_part->_pointers.end()
        );

        left_part->_keys.erase(
            left_part->_keys.begin() + split,
            left_part->_keys.end()
        );
        left_part->_pointers.erase(
            left_part->_pointers.begin() + split + 1,
            left_part->_pointers.end()
        );

        cur_node->_keys.insert(cur_node->_keys.begin() + index, cur_key);
        cur_node->_pointers.insert(cur_node->_pointers.begin() + index + 1, 1, static_cast<bptree_node_base*>(right_part));
       
    }
    // std::cout << "insert 5\n";
    return bptree_iterator(leaf, pos);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::insert_or_assign(tree_data_type&& data)
{
        // std::cout << "insert 0\n";

    std::stack<std::pair<bptree_node_middle*, size_t>> path {};
    if (_root == nullptr) {
        bptree_node_term* new_root = _allocator.template new_object<bptree_node_term>();
        new_root->_is_terminate = true;
        _root = static_cast<bptree_node_base*>(new_root);
        new_root->_next = nullptr;
        new_root->_data.push_back(data);
        _size++;
        return bptree_iterator(new_root, 0);
    }

    // move to leaf

    // std::cout << "insert 1\n";

    bptree_node_base* cur_node_b = _root;
    bptree_node_middle* cur_node;

    while (!cur_node_b->_is_terminate) {
        cur_node = static_cast<bptree_node_middle*>(cur_node_b);
        size_t low = 0;
        size_t high = cur_node->_keys.size() + 1;
        size_t mid;
        while (low + 1 < high) {
            mid = low + (high - low) / 2;
            if (compare_keys(data.first, cur_node->_keys[mid - 1])) {
                high = mid;
            } else {
                low = mid;
            }
        }
        path.push(std::make_pair(cur_node, low));
        cur_node_b = cur_node->_pointers[low];
    }

    // find place

    // std::cout << "insert 2\n";

    size_t pos;

    bptree_node_term* leaf = static_cast<bptree_node_term*>(cur_node_b);
    size_t low = 0;
    size_t high = leaf->_data.size() + 1;
    size_t mid;
    while (low + 1 < high) {
        mid = low + (high - low) / 2;
        if (compare_keys(data.first, leaf->_data[mid - 1].first)) {
            high = mid;
        } else if (compare_keys(leaf->_data[mid - 1].first, data.first)) {
            low = mid;
        } else {
            leaf->_data[mid - 1].second = data.second;
            return bptree_iterator(leaf, mid - 1);
        }   
    }

    pos = low;

    leaf->_data.insert(leaf->_data.begin() + pos, data);
    _size++;

    // fix rule

    if (leaf->_data.size() <= maximum_keys_in_node) {
        return bptree_iterator(leaf, pos);
    }

    // std::cout << "insert 3\n";

    const size_t split = t;

    // split leaf

    bptree_node_term* right_leaf = _allocator.template new_object<bptree_node_term>();
    right_leaf->_is_terminate = true;

    tkey cur_key = leaf->_data[split].first;
    right_leaf->_data.insert(
        right_leaf->_data.end(),
        leaf->_data.begin() + split,
        leaf->_data.end()
    );
    leaf->_data.erase(
        leaf->_data.begin() + split,
        leaf->_data.end()
    );

    right_leaf->_next = leaf->_next;
    leaf->_next = right_leaf;

    size_t index;

    if (path.empty()) {
        cur_node = _allocator.template new_object<bptree_node_middle>();
        cur_node->_is_terminate = false;
        cur_node->_pointers.push_back(static_cast<bptree_node_base*>(leaf));
        _root = static_cast<bptree_node_base*>(cur_node);
        index = 0;
    } else {
        cur_node = path.top().first;
        index = path.top().second;
    }

    cur_node->_keys.insert(
        cur_node->_keys.begin() + index,
        cur_key
    );
    cur_node->_pointers.insert(
        cur_node->_pointers.begin() + index + 1,
        static_cast<bptree_node_base*>(right_leaf)
    );

    std::cout << cur_node->_pointers[0]->_is_terminate << " " << cur_node->_pointers[1]->_is_terminate << std::endl; 

    // split middle

    // std::cout << "insert 4\n";

    bptree_node_middle *left_part, *right_part;
    while (cur_node->_keys.size() == maximum_keys_in_node) {
        cur_key = cur_node->_keys[split];
        path.pop();
        left_part = cur_node;
        if (path.empty()) {
            cur_node = _allocator.template new_object<bptree_node_middle>();
            cur_node->_is_terminate = false;
            cur_node->_pointers.push_back(static_cast<bptree_node_base*>(left_part));
            _root = cur_node;
            index = 0;
        } else {
            cur_node = path.top().first;
            index = path.top().second;
        }
        right_part = _allocator.template new_object<bptree_node_middle>();
        right_part->_is_terminate = false;
        right_part->_keys.insert(
            right_part->_keys.end(),
            left_part->_keys.begin() + split + 1,
            left_part->_keys.end()
        );
        right_part->_pointers.insert(
            right_part->_pointers.end(),
            left_part->_pointers.begin() + split + 1, 
            left_part->_pointers.end()
        );

        left_part->_keys.erase(
            left_part->_keys.begin() + split,
            left_part->_keys.end()
        );
        left_part->_pointers.erase(
            left_part->_pointers.begin() + split + 1,
            left_part->_pointers.end()
        );

        cur_node->_keys.insert(cur_node->_keys.begin() + index, cur_key);
        cur_node->_pointers.insert(cur_node->_pointers.begin() + index + 1, 1, static_cast<bptree_node_base*>(right_part));
       
    }
    // std::cout << "insert 5\n";
    return bptree_iterator(leaf, pos);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template <typename ...Args>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::emplace_or_assign(Args&&... args)
{
    tree_data_type data (std::forward<Args>(args) ...);
    return insert_or_assign(std::move(data));
}

//erase

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::erase(bptree_iterator pos)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::erase(bptree_iterator pos)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::erase(bptree_const_iterator pos)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::erase(bptree_const_iterator pos)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::erase(bptree_iterator beg, bptree_iterator en)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::erase(bptree_iterator beg, bptree_iterator en)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::erase(bptree_const_iterator beg, bptree_const_iterator en)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::erase(bptree_const_iterator beg, bptree_const_iterator en)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::erase(const tkey& key)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::erase(const tkey& key)", "your code should be here...");
}

#endif