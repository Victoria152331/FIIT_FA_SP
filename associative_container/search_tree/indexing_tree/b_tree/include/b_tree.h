#ifndef MP_OS_B_TREE_H
#define MP_OS_B_TREE_H

#include <iterator>
#include <utility>
#include <boost/container/static_vector.hpp>
#include <stack>
#include <pp_allocator.h>
#include <search_tree.h>
#include <initializer_list>
#include <logger_guardant.h>

template <typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5>
class B_tree final : private logger_guardant, private compare
{
public:

    using tree_data_type = std::pair<tkey, tvalue>;
    using tree_data_type_const = std::pair<const tkey, tvalue>;
    using value_type = tree_data_type_const;

private:

    static constexpr const size_t minimum_keys_in_node = t - 1;
    static constexpr const size_t maximum_keys_in_node = 2 * t - 1;

    // region comparators declaration

    inline bool compare_keys(const tkey& lhs, const tkey& rhs) const;
    inline bool compare_pairs(const tree_data_type& lhs, const tree_data_type& rhs) const;

    // endregion comparators declaration


    struct btree_node
    {
        boost::container::static_vector<tree_data_type, maximum_keys_in_node + 1> _keys;
        boost::container::static_vector<btree_node*, maximum_keys_in_node + 2> _pointers;
        btree_node() noexcept;
//        std::vector<tree_data_type, pp_allocator<tree_data_type>> _keys;
//        std::vector<btree_node*, pp_allocator<btree_node*>> _pointers;

//        btree_node(pp_allocator<value_type> al);
    };

    pp_allocator<value_type> _allocator;
    logger* _logger;
    btree_node* _root;
    size_t _size;

    logger* get_logger() const noexcept override;
    pp_allocator<value_type> get_allocator() const noexcept;

public:

    // region constructors declaration

    explicit B_tree(const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    explicit B_tree(pp_allocator<value_type> alloc, const compare& comp = compare(), logger *logger = nullptr);

    template<input_iterator_for_pair<tkey, tvalue> iterator>
    explicit B_tree(iterator begin, iterator end, const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    B_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    // endregion constructors declaration

    // region five declaration

    B_tree(const B_tree& other);

    B_tree(B_tree&& other) noexcept;

    B_tree& operator=(const B_tree& other);

    B_tree& operator=(B_tree&& other) noexcept;

    ~B_tree() noexcept override;

    void delete_subtree(btree_node* root);

    // endregion five declaration

    // region iterators declaration

    class btree_iterator;
    class btree_reverse_iterator;
    class btree_const_iterator;
    class btree_const_reverse_iterator;

    class btree_iterator final
    {
        std::stack<std::pair<btree_node**, size_t>> _path;
        size_t _index;

    public:
        using value_type = tree_data_type;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_const_iterator;
        friend class btree_const_reverse_iterator;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_iterator(const std::stack<std::pair<btree_node**, size_t>>& path = std::stack<std::pair<btree_node**, size_t>>(), size_t index = 0);

    };

    class btree_const_iterator final
    {
        std::stack<std::pair<btree_node* const*, size_t>> _path;
        size_t _index;

    public:

        using value_type = tree_data_type;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_const_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_iterator;
        friend class btree_const_reverse_iterator;

        btree_const_iterator(const btree_iterator& it) noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_const_iterator(const std::stack<std::pair<btree_node* const*, size_t>>& path = std::stack<std::pair<btree_node* const*, size_t>>(), size_t index = 0);
    };

    class btree_reverse_iterator final
    {
        std::stack<std::pair<btree_node**, size_t>> _path;
        size_t _index;

    public:

        using value_type = tree_data_type_const;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_reverse_iterator;

        friend class B_tree;
        friend class btree_iterator;
        friend class btree_const_iterator;
        friend class btree_const_reverse_iterator;

        btree_reverse_iterator(const btree_iterator& it) noexcept;
        operator btree_iterator() const noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_reverse_iterator(const std::stack<std::pair<btree_node**, size_t>>& path = std::stack<std::pair<btree_node**, size_t>>(), size_t index = 0);
    };

    class btree_const_reverse_iterator final
    {
        std::stack<std::pair<btree_node* const*, size_t>> _path;
        size_t _index;

    public:

        using value_type = tree_data_type_const;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_const_reverse_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_const_iterator;
        friend class btree_iterator;

        btree_const_reverse_iterator(const btree_reverse_iterator& it) noexcept;
        operator btree_const_iterator() const noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_const_reverse_iterator(const std::stack<std::pair<btree_node* const*, size_t>>& path = std::stack<std::pair<btree_node* const*, size_t>>(), size_t index = 0);
    };

    friend class btree_iterator;
    friend class btree_const_iterator;
    friend class btree_reverse_iterator;
    friend class btree_const_reverse_iterator;

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

    btree_iterator begin();
    btree_iterator end();

    btree_const_iterator begin() const;
    btree_const_iterator end() const;

    btree_const_iterator cbegin() const;
    btree_const_iterator cend() const;

    btree_reverse_iterator rbegin();
    btree_reverse_iterator rend();

    btree_const_reverse_iterator rbegin() const;
    btree_const_reverse_iterator rend() const;

    btree_const_reverse_iterator crbegin() const;
    btree_const_reverse_iterator crend() const;

    // endregion iterator begins declaration

    // region lookup declaration

    size_t size() const noexcept;
    bool empty() const noexcept;

    /*
     * Returns end() if not exist
     */

    btree_iterator find(const tkey& key);
    btree_const_iterator find(const tkey& key) const;

    btree_iterator lower_bound(const tkey& key);
    btree_const_iterator lower_bound(const tkey& key) const;

    btree_iterator upper_bound(const tkey& key);
    btree_const_iterator upper_bound(const tkey& key) const;

    bool contains(const tkey& key) const;

    // endregion lookup declaration

    // region modifiers declaration

    void clear() noexcept;

    /*
     * Does nothing if key exists, delegates to emplace.
     * Second return value is true, when inserted
     */
    std::pair<btree_iterator, bool> insert(const tree_data_type& data);
    std::pair<btree_iterator, bool> insert(tree_data_type&& data);

    template <typename ...Args>
    std::pair<btree_iterator, bool> emplace(Args&&... args);

    /*
     * Updates value if key exists, delegates to emplace.
     */
    btree_iterator insert_or_assign(const tree_data_type& data);
    btree_iterator insert_or_assign(tree_data_type&& data);

    template <typename ...Args>
    btree_iterator emplace_or_assign(Args&&... args);

    /*
     * Return iterator to node next ro removed or end() if key not exists
     */
    btree_iterator erase(btree_iterator pos);
    btree_iterator erase(btree_const_iterator pos);

    btree_iterator erase(btree_iterator beg, btree_iterator en);
    btree_iterator erase(btree_const_iterator beg, btree_const_iterator en);


    btree_iterator erase(const tkey& key);

    // endregion modifiers declaration
};

template<std::input_iterator iterator, compator<typename std::iterator_traits<iterator>::value_type::first_type> compare = std::less<typename std::iterator_traits<iterator>::value_type::first_type>,
        std::size_t t = 5, typename U>
B_tree(iterator begin, iterator end, const compare &cmp = compare(), pp_allocator<U> = pp_allocator<U>(),
       logger *logger = nullptr) -> B_tree<typename std::iterator_traits<iterator>::value_type::first_type, typename std::iterator_traits<iterator>::value_type::second_type, compare, t>;

template<typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5, typename U>
B_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare &cmp = compare(), pp_allocator<U> = pp_allocator<U>(),
       logger *logger = nullptr) -> B_tree<tkey, tvalue, compare, t>;


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_node::btree_node() noexcept
{
    _keys.clear();
    _pointers.clear();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
logger* B_tree<tkey, tvalue, compare, t>::get_logger() const noexcept
{
    return _logger;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
pp_allocator<typename B_tree<tkey, tvalue, compare, t>::value_type> B_tree<tkey, tvalue, compare, t>::get_allocator() const noexcept
{
    return _allocator;
}

// + region constructors implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* logger)
    : compare(cmp)
    , _root(nullptr)
    , _logger(logger)
    , _size(0)
    , _allocator(alloc)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(
        pp_allocator<value_type> alloc,
        const compare& cmp,
        logger* logger)
    : compare(cmp)
    , _root(nullptr)
    , _logger(logger)
    , _size(0)
    , _allocator(alloc)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<input_iterator_for_pair<tkey, tvalue> iterator>
B_tree<tkey, tvalue, compare, t>::B_tree(
        iterator begin,
        iterator end,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* logger)
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
B_tree<tkey, tvalue, compare, t>::B_tree(
        std::initializer_list<std::pair<tkey, tvalue>> data,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* logger)
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

// endregion constructors implementation

// + region five implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::~B_tree() noexcept
{
    delete_subtree(_root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::delete_subtree(btree_node* root)
{
    if (!root) return;

    for (auto cur: root->_pointers) {
        delete_subtree(cur);
    }

    _allocator.delete_object(root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(const B_tree& other)
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
B_tree<tkey, tvalue, compare, t>& B_tree<tkey, tvalue, compare, t>::operator=(const B_tree& other)
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
B_tree<tkey, tvalue, compare, t>::B_tree(B_tree&& other) noexcept
    : compare(other)
    , _root(other._root)
    , _logger(other._logger)
    , _size(other._size)
    , _allocator(std::move(other._allocator))
{
    other._root = nullptr;
    other._size = 0;
    other._logger = nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>& B_tree<tkey, tvalue, compare, t>::operator=(B_tree&& other) noexcept
{
    if (this != &other) {

        delete_subtree(_root);

        compare::operator=(other);
        _root = other._root;
        _size = other._size;
        _logger = other._logger;
        _allocator = std::move(other._allocator);

        other._root = nullptr;
        other._size = 0;
        other._logger = nullptr;
    }
    return *this; 
}

// endregion five implementation

// region iterators implementation

    // btree_iterator

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_iterator::btree_iterator(
        const std::stack<std::pair<btree_node**, size_t>>& path, size_t index)
    : _path(path)
    , _index(index)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator*() const noexcept
{
    return (**(_path.top().first))._keys[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator->() const noexcept
{
    btree_node* node = *(_path.top().first);
    if (node == nullptr) {
        return nullptr;
    }
    return &((*node)._keys[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator&
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator++()
{
    btree_node* node = *(_path.top().first);
    if (node == nullptr) { // before begin or end
        if (_path.top().second == 0) { // means before begin
            _index = _path.top().second;
            _path.pop();
        }
        return *this;
    }
    size_t k = node->_keys.size();
    if (node->_pointers[0] != nullptr) {
        _path.push(std::make_pair(&(node->_pointers[_index + 1]), _index + 1));
        btree_node* cur_node = node->_pointers[_index + 1];
        _index = 0;
        while (cur_node->_pointers[0] != nullptr) {
            _path.push(std::make_pair(&(cur_node->_pointers[0]), 0));
            cur_node = cur_node->_pointers[0];
        }
        return *this;
    } else if (_index + 1 < k) {
        _index++;
        return *this;
    } else {
        btree_node* cur_node = node;
        while (_path.size() > 1) {
            _index = _path.top().second;
            _path.pop();
            cur_node = *(_path.top().first);
            k = cur_node->_keys.size();
            if (_index < k) {
                return *this;
            }
        }
        
        //create end
        _index = 0;
        while (cur_node != nullptr) {
            k = cur_node->_keys.size();
            _path.push(std::make_pair(&(cur_node->_pointers[k]), k));
            cur_node = cur_node->_pointers[k];
        }
        return *this;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator++(int)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator&
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator--()
{
    btree_node* node = *(_path.top().first);
    if (node == nullptr) { // before begin or end
        if (_path.top().second != 0) { // means end
            _index = _path.top().second;
            _path.pop();
        }
        return *this;
    }

    size_t k = node->_keys.size();
    if (node->_pointers[0] != nullptr) {
        _path.push(std::make_pair(&(node->_pointers[_index]), _index));
        btree_node* cur_node = node->_pointers[_index];
        k = cur_node->_keys.size();
        _index = k - 1;
        while (cur_node->_pointers[k] != nullptr) {
            _path.push(std::make_pair(&(cur_node->_pointers[k]), k));
            cur_node = cur_node->_pointers[k];
            k = cur_node->_keys.size();
            _index = k - 1;
        }
        return *this;
    } else if (_index != 0) {
        _index--;
        return *this;
    } else {
        while (_path.size() > 1) {
            _index = _path.top().second;
            _path.pop();
            if (_index != 0) {
                _index--;
                return *this;
            }
        }
        
        //create before begin
        btree_node* cur_node = *(_path.top().first);
        while (cur_node != nullptr) {
            _path.push(std::make_pair(&(cur_node->_pointers[0]), 0));
            cur_node = cur_node->_pointers[0];
        }
        return *this;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator--(int)
{
    auto tmp = *this;
    --(*this);
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::operator==(const self& other) const noexcept
{
    return (_path == other._path) && (_index == other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::operator!=(const self& other) const noexcept
{
    return (_path != other._path) || (_index != other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::depth() const noexcept
{
    return _path.size() - 1;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::current_node_keys_count() const noexcept
{
    btree_node* node = *(_path.top().first);
    return node._keys.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::is_terminate_node() const noexcept
{
    btree_node* node = *(_path.top().first);
    if ((node == nullptr)) {
        return true;
    }
    return false;

}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::index() const noexcept
{
    return _index;
}

    // btree_const_iterator

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::btree_const_iterator(
        const std::stack<std::pair<btree_node* const*, size_t>>& path, size_t index)
    : _path(path)
    , _index(index)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::btree_const_iterator(
        const btree_iterator& it) noexcept
    : _index(it._index)
{
    auto buf = std::stack<std::pair<btree_node* const*, size_t>>();
    while (!it._path.empty()) {
        buf.push(it._path.pop());
    }
    while (!buf.empty()) {
        _path.push(buf.pop());
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator*() const noexcept
{
    return (**(_path.top().first))._keys[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator->() const noexcept
{
    btree_node* const node = *(_path.top().first);
    if (node == nullptr) {
        return nullptr;
    }
    return &((*node)._keys[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator++()
{
    btree_node* node = *(_path.top().first);
    if (node == nullptr) { // before begin or end
        if (_path.top().second == 0) { // means before begin
            _index = _path.top().second;
            _path.pop();
        }
        return *this;
    }
    size_t k = node->_keys.size();
    if (node->_pointers[0] != nullptr) {
        _path.push(std::make_pair(&(node->_pointers[_index + 1]), _index + 1));
        btree_node* cur_node = node->_pointers[_index + 1];
        _index = 0;
        while (cur_node->_pointers[0] != nullptr) {
            _path.push(std::make_pair(&(cur_node->_pointers[0]), 0));
            cur_node = cur_node->_pointers[0];
        }
        return *this;
    } else if (_index + 1 < k) {
        _index++;
        return *this;
    } else {
        btree_node* cur_node = node;
        while (_path.size() > 1) {
            _index = _path.top().second;
            _path.pop();
            cur_node = *(_path.top().first);
            k = cur_node->_keys.size();
            if (_index < k) {
                return *this;
            }
        }
        
        //create end
        while (cur_node != nullptr) {
            k = cur_node->_keys.size();
            _path.push(std::make_pair(&(cur_node->_pointers[k]), k));
            cur_node = cur_node->_pointers[k];
        }
        return *this;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator++(int)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator--()
{
    btree_node* node = *(_path.top().first);
    if (node == nullptr) { // before begin or end
        if (_path.top().second != 0) { // means end
            _index = _path.top().second;
            _path.pop();
        }
        return *this;
    }

    size_t k = node->_keys.size();
    if (node->_pointers[0] != nullptr) {
        _path.push(std::make_pair(&(node->_pointers[_index]), _index));
        btree_node* cur_node = node->_pointers[_index];
        k = cur_node->_keys.size();
        _index = k - 1;
        while (cur_node->_pointers[k] != nullptr) {
            _path.push(std::make_pair(&(cur_node->_pointers[k]), k));
            cur_node = cur_node->_pointers[k];
            k = cur_node->_keys.size();
            _index = k - 1;
        }
        return *this;
    } else if (_index != 0) {
        _index--;
        return *this;
    } else {
        while (_path.size() > 1) {
            _index = _path.top().second;
            _path.pop();
            if (_index != 0) {
                _index--;
                return *this;
            }
        }
        
        //create before begin
        btree_node* cur_node = *(_path.top().first);
        while (cur_node != nullptr) {
            _path.push(std::make_pair(&(cur_node->_pointers[0]), 0));
            cur_node = cur_node->_pointers[0];
        }
        return *this;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator--(int)
{
    auto tmp = *this;
    --(*this);
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator==(const self& other) const noexcept
{
    return (_path == other._path) && (_index == other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator!=(const self& other) const noexcept
{
    return (_path != other._path) || (_index != other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::depth() const noexcept
{
    return _path.size() - 1;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::current_node_keys_count() const noexcept
{
    btree_node* node = *(_path.top().first);
    return node._keys.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::is_terminate_node() const noexcept
{
    btree_node* node = *(_path.top().first);
    if ((node == nullptr)) {
        return true;
    }
    return false;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::index() const noexcept
{
    return _index;
}

    // btree_reverse_iterator

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::btree_reverse_iterator(
    const std::stack<std::pair<btree_node**, size_t>>& path, size_t index)
    : _path(path)
    , _index(index)
{}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::btree_reverse_iterator(
    const btree_iterator& it) noexcept
{
    auto tmp = it;
    if (tmp._path.empty()) {
        _index = 0;
        return;
    }
    
    // Convert to reverse iterator by finding the end and then decrementing
    while (!tmp._path.empty() && !tmp.is_terminate_node()) {
        ++tmp;
    }
    if (!tmp._path.empty()) {
        --tmp;
        _path = tmp._path;
        _index = tmp._index;
    }
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator B_tree<tkey, tvalue, compare, t>::btree_iterator() const noexcept
{
    btree_iterator it;
    it._path = _path;
    it._index = _index;
    return it;
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator*() const noexcept
{
    return (**(_path.top().first))._keys[_index];
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator->() const noexcept
{
    btree_node* node = *(_path.top().first);
    if (node == nullptr) {
        return nullptr;
    }
    return &((*node)._keys[_index]);
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator++()
{
    btree_node* node = *(_path.top().first);
    if (node == nullptr) { // before begin or end
        if (_path.top().second != 0) { // means end
            _index = _path.top().second;
            _path.pop();
        }
        return *this;
    }


    if (_index > 0) {
        _index--;
        return *this;
    } else if (node->_pointers[0] != nullptr) {
        _path.push(std::make_pair(&(node->_pointers[0]), 0));
        btree_node* cur_node = node->_pointers[0];
        size_t k = cur_node->_keys.size();
        _index = k - 1;
        while (cur_node->_pointers[k] != nullptr) {
            _path.push(std::make_pair(&(cur_node->_pointers[k]), k));
            cur_node = cur_node->_pointers[k];
            k = cur_node->_keys.size();
            _index = k - 1;
        }
        return *this;
    } else {
        while (_path.size() > 1) {
            _index = _path.top().second;
            _path.pop();
            node = *(_path.top().first);
            if (_index > 0) {
                _index--;
                return *this;
            }
        }
        
        // create before begin
        _index = 0;
        while (node != nullptr) {
            _path.push(std::make_pair(&(node->_pointers[0]), 0));
            node = node->_pointers[0];
        }
        return *this;
    }
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator++(int)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator--()
{
    btree_node* node = *(_path.top().first);
    if (node == nullptr) { // before begin or end
        if (_path.top().second == 0) { // means before begin
            _index = _path.top().second;
            _path.pop();
        }
        return *this;
    }


    size_t k = node->_keys.size();
    if (node->_pointers[0] != nullptr) {
        _path.push(std::make_pair(&(node->_pointers[_index + 1]), _index + 1));
        btree_node* cur_node = node->_pointers[_index + 1];
        _index = 0;
        while (cur_node->_pointers[0] != nullptr) {
            _path.push(std::make_pair(&(cur_node->_pointers[0]), 0));
            cur_node = cur_node->_pointers[0];
        }
        return *this;
    } else if (_index + 1 < k) {
        _index++;
        return *this;
    } else {
        while (_path.size() > 1) {
            _index = _path.top().second;
            _path.pop();
            node = *(_path.top().first);
            k = node->_keys.size();
            if (_index < k) {
                return *this;
            }
        }
        
        // create end
        _index = 0;
        while (node != nullptr) {
            k = node->_keys.size();
            _path.push(std::make_pair(&(node->_pointers[k]), k));
            node = node->_pointers[k];
        }
        return *this;
    }
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator--(int)
{
    auto tmp = *this;
    --(*this);
    return tmp;
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator==(const self& other) const noexcept
{
    return (_path == other._path) && (_index == other._index);
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator!=(const self& other) const noexcept
{
    return (_path != other._path) || (_index != other._index);
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::depth() const noexcept
{
    return _path.size() - 1;
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::current_node_keys_count() const noexcept
{
    btree_node* node = *(_path.top().first);
    return node->_keys.size();
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::is_terminate_node() const noexcept
{
    btree_node* node = *(_path.top().first);
    return (node == nullptr);
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::index() const noexcept
{
    return _index;
}

    // btree_const_reverse_iterator

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::btree_const_reverse_iterator(
    const std::stack<std::pair<btree_node* const*, size_t>>& path, size_t index)
    : _path(path)
    , _index(index)
{}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::btree_const_reverse_iterator(
    const btree_reverse_iterator& it) noexcept
{
    auto buf = std::stack<std::pair<btree_node* const*, size_t>>();
    auto temp = it._path;
    while (!temp.empty()) {
        buf.push(std::make_pair(const_cast<btree_node* const*>(temp.top().first), temp.top().second));
        temp.pop();
    }
    while (!buf.empty()) {
        _path.push(buf.top());
        buf.pop();
    }
    _index = it._index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator B_tree<tkey, tvalue, compare, t>::btree_const_iterator() const noexcept
{
    btree_const_iterator it;
    it._path = _path;
    it._index = _index;
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator*() const noexcept
{
    return (**(_path.top().first))._keys[_index];
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator->() const noexcept
{
    btree_node* const node = *(_path.top().first);
    if (node == nullptr) {
        return nullptr;
    }
    return &((*node)._keys[_index]);
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator++()
{
    btree_node* node = *(_path.top().first);
    if (node == nullptr) { // before begin or end
        if (_path.top().second != 0) { // means end
            _index = _path.top().second;
            _path.pop();
        }
        return *this;
    }


    if (_index > 0) {
        _index--;
        return *this;
    } else if (node->_pointers[0] != nullptr) {
        _path.push(std::make_pair(&(node->_pointers[0]), 0));
        btree_node* cur_node = node->_pointers[0];
        size_t k = cur_node->_keys.size();
        _index = k - 1;
        while (cur_node->_pointers[k] != nullptr) {
            _path.push(std::make_pair(&(cur_node->_pointers[k]), k));
            cur_node = cur_node->_pointers[k];
            k = cur_node->_keys.size();
            _index = k - 1;
        }
        return *this;
    } else {
        while (_path.size() > 1) {
            _index = _path.top().second;
            _path.pop();
            node = *(_path.top().first);
            if (_index > 0) {
                _index--;
                return *this;
            }
        }
        
        // create before begin
        _index = 0;
        while (node != nullptr) {
            _path.push(std::make_pair(&(node->_pointers[0]), 0));
            node = node->_pointers[0];
        }
        return *this;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator++(int)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator--()
{
    btree_node* node = *(_path.top().first);
    if (node == nullptr) { // before begin or end
        if (_path.top().second == 0) { // means before begin
            _index = _path.top().second;
            _path.pop();
        }
        return *this;
    }


    size_t k = node->_keys.size();
    if (node->_pointers[0] != nullptr) {
        _path.push(std::make_pair(&(node->_pointers[_index + 1]), _index + 1));
        btree_node* cur_node = node->_pointers[_index + 1];
        _index = 0;
        while (cur_node->_pointers[0] != nullptr) {
            _path.push(std::make_pair(&(cur_node->_pointers[0]), 0));
            cur_node = cur_node->_pointers[0];
        }
        return *this;
    } else if (_index + 1 < k) {
        _index++;
        return *this;
    } else {
        while (_path.size() > 1) {
            _index = _path.top().second;
            _path.pop();
            node = *(_path.top().first);
            k = node->_keys.size();
            if (_index < k) {
                return *this;
            }
        }
        
        // create end
        _index = 0;
        while (node != nullptr) {
            k = node->_keys.size();
            _path.push(std::make_pair(&(node->_pointers[k]), k));
            node = node->_pointers[k];
        }
        return *this;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator--(int)
{
    auto tmp = *this;
    --(*this);
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator==(const self& other) const noexcept
{
    return (_path == other._path) && (_index == other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator!=(const self& other) const noexcept
{
    return (_path != other._path) || (_index != other._index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::depth() const noexcept
{
    return _path.size() - 1;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::current_node_keys_count() const noexcept
{
    btree_node* node = *(_path.top().first);
    return node->_keys.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::is_terminate_node() const noexcept
{
    btree_node* node = *(_path.top().first);
    return (node == nullptr);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::index() const noexcept
{
    return _index;
}

// endregion iterators implementation

// region element access implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::at(const tkey& key)
{
    auto it = find(key);
    return (*it).second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
const tvalue& B_tree<tkey, tvalue, compare, t>::at(const tkey& key) const
{
    auto it = find(key);
    return (*it).second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::operator[](const tkey& key)
{
    auto it = find(key);
    return (*it).second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::operator[](tkey&& key)
{
    auto it = find(key);
    return (*it).second;
}

// endregion element access implementation

// region iterator begins implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::begin()
{
    auto path = std::stack<std::pair<btree_node**, size_t>>();
    path.push(std::make_pair(&_root, 0));
    if (_root == nullptr) {
        return btree_iterator(path, 0);
    }
    btree_node* cur_node = _root;
    while (cur_node->_pointers[0] != nullptr) {
        path.push(std::make_pair(&(cur_node->_pointers[0]), 0));
        cur_node = cur_node->_pointers[0];
    }
    return btree_iterator(path, 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::end()
{
    auto path = std::stack<std::pair<btree_node**, size_t>>();
    path.push(std::make_pair(&_root, 0));
    if (_root == nullptr) {
        return btree_iterator(path, 0);
    }
    btree_node* cur_node = _root;
    size_t k = cur_node->_keys.size();
    while (cur_node->_pointers[k] != nullptr) {
        path.push(std::make_pair(&(cur_node->_pointers[k]), k));
        cur_node = cur_node->_pointers[k];
        k = cur_node->_keys.size();
    }
    path.push(std::make_pair(&(cur_node->_pointers[k]), k));
    return btree_iterator(path, 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::begin() const
{
    auto path = std::stack<std::pair<btree_node* const*, size_t>>();
    path.push(std::make_pair(&_root, 0));
    if (_root == nullptr) {
        return btree_const_iterator(path, 0);
    }
    btree_node* cur_node = _root;
    while (cur_node->_pointers[0] != nullptr) {
        path.push(std::make_pair(&(cur_node->_pointers[0]), 0));
        cur_node = cur_node->_pointers[0];
    }
    return btree_const_iterator(path, 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::end() const
{
    auto path = std::stack<std::pair<btree_node* const*, size_t>>();
    path.push(std::make_pair(&_root, 0));
    if (_root == nullptr) {
        return btree_const_iterator(path, 0);
    }
    btree_node* cur_node = _root;
    size_t k = cur_node->_keys.size();
    while (cur_node->_pointers[k] != nullptr) {
        path.push(std::make_pair(&(cur_node->_pointers[k]), k));
        cur_node = cur_node->_pointers[k];
        k = cur_node->_keys.size();
    }
    path.push(std::make_pair(&(cur_node->_pointers[k]), k));
    return btree_const_iterator(path, 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::cbegin() const
{
    auto path = std::stack<std::pair<btree_node* const*, size_t>>();
    path.push(std::make_pair(&_root, 0));
    if (_root == nullptr) {
        return btree_const_iterator(path, 0);
    }
    btree_node* cur_node = _root;
    while (cur_node->_pointers[0] != nullptr) {
        path.push(std::make_pair(&(cur_node->_pointers[0]), 0));
        cur_node = cur_node->_pointers[0];
    }
    return btree_const_iterator(path, 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::cend() const
{
    auto path = std::stack<std::pair<btree_node* const*, size_t>>();
    path.push(std::make_pair(&_root, 0));
    if (_root == nullptr) {
        return btree_const_iterator(path, 0);
    }
    btree_node* cur_node = _root;
    size_t k = cur_node->_keys.size();
    while (cur_node->_pointers[k] != nullptr) {
        path.push(std::make_pair(&(cur_node->_pointers[k]), k));
        cur_node = cur_node->_pointers[k];
        k = cur_node->_keys.size();
    }
    path.push(std::make_pair(&(cur_node->_pointers[k]), k));
    return btree_const_iterator(path, 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin()
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin()", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend()
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend()", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin() const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin() const", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend() const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend() const", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crbegin() const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crbegin() const", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crend() const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crend() const", "your code should be here...");
}

// endregion iterator begins implementation

// region lookup implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::size() const noexcept
{
    return _size;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::empty() const noexcept
{
    return (_size == 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::find(const tkey& key)
{
    std::stack<std::pair<btree_node**, size_t>> path {};
    path.push(std::make_pair(&_root, 0));
    if (_root == nullptr) {
        return end();
    }
    btree_node* cur_node = _root;
    size_t k;
    while (cur_node != nullptr) {
        k = cur_node->_keys.size();
        // std::cout << "pupupu" << std::endl;
        // for (auto& i : cur_node->_keys) {
        //     std::cout << i.first << " ";
        // }
        // std::cout << std::endl;
        size_t low = 0;
        size_t high = k + 1;
        size_t mid;
        while (low + 1 < high) {
            mid = low + (high - low) / 2;
            if (compare_keys(key, cur_node->_keys[mid - 1].first)) {
                high = mid;
            } else if (compare_keys(cur_node->_keys[mid - 1].first, key)) {
                low = mid;
            } else {
                return btree_iterator(path, mid - 1);
            }
        }
        path.push(std::make_pair(&(cur_node->_pointers[low]), low));
        cur_node = cur_node->_pointers[low];
        
    }
    return end();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::find(const tkey& key) const
{
    std::stack<std::pair<btree_node**, size_t>> path {};
    path.push(std::make_pair(&_root, 0));
    if (_root == nullptr) {
        return end();
    }
    btree_node* cur_node = _root;
    size_t k;
    while (cur_node != nullptr) {
        k = cur_node->_keys.size();
        size_t low = 0;
        size_t high = k + 1;
        size_t mid;
        while (low + 1 < high) {
            mid = low + (high - low) / 2;
            if (compare_keys(key, cur_node->_keys[mid - 1].first)) {
                high = mid;
            } else if (compare_keys(cur_node->_keys[mid - 1].first, key)) {
                low = mid;
            } else {
                return btree_iterator(path, mid - 1);
            }
        }
        path.push(std::make_pair(&(cur_node->_pointers[low]), low));
        cur_node = cur_node->_pointers[low];
        
    }
    return end();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::contains(const tkey& key) const
{
    return find(key) != end();
}

// endregion lookup implementation

// region modifiers implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::clear() noexcept
{
    delete_subtree(_root);
    _root = nullptr;
    _size = 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::insert(const tree_data_type& data)
{
    std::stack<std::pair<btree_node**, size_t>> path {};
    path.push(std::make_pair(&_root, 0));
    if (_root == nullptr) {
        _root = _allocator.template new_object<btree_node>();
        _root->_keys.push_back(data);
        _root->_pointers.push_back(nullptr);
        _root->_pointers.push_back(nullptr);
        _size++;
        return std::make_pair(btree_iterator(path, 0), true);
    }
    btree_node* cur_node = _root;
    size_t k = cur_node->_keys.size();

    // move to leaf

    while (cur_node->_pointers[0] != nullptr) {
        
        size_t low = 0;
        size_t high = k + 1;
        size_t mid;
        while (low + 1 < high) {
            mid = low + (high - low) / 2;
            if (compare_keys(data.first, cur_node->_keys[mid -1].first)) {
                high = mid;
            } else if (compare_keys(cur_node->_keys[mid -1].first, data.first)) {
                low = mid;
            } else {
                return std::make_pair(btree_iterator(path, mid - 1), false);
            }
        }
        path.push(std::make_pair(&(cur_node->_pointers[low]), low));
        cur_node = cur_node->_pointers[low];
        
        k = cur_node->_keys.size();
    }

    // find place

    size_t pos;

    size_t low = 0;
    size_t high = k + 1;
    size_t mid;
    while (low + 1 < high) {
        mid = low + (high - low) / 2;
        if (compare_keys(data.first, cur_node->_keys[mid - 1].first)) {
            high = mid;
        } else if (compare_keys(cur_node->_keys[mid - 1].first, data.first)) {
            low = mid;
        } else {
            return std::make_pair(btree_iterator(path, mid - 1), false);
        }
    }
    pos = low;

    cur_node->_keys.insert(cur_node->_keys.begin() + pos, data);
    cur_node->_pointers.push_back(nullptr);

    // fix rule

    if (k < maximum_keys_in_node) {
        _size++;
        return std::make_pair(btree_iterator(path, pos), true);
    }

    tree_data_type cur_data;
    size_t split = (maximum_keys_in_node + 1) / 2, i;
    btree_node *left_part, *right_part;
    while (k == maximum_keys_in_node) {
        cur_data = cur_node->_keys[split];
        i = path.top().second;
        path.pop();
        left_part = cur_node;
        if (path.empty()) {
            cur_node = _allocator.template new_object<btree_node>();
            cur_node->_pointers.push_back(left_part);
            _root = cur_node;
        } else {
            cur_node = *(path.top().first);
        }
        right_part = _allocator.template new_object<btree_node>();
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

        cur_node->_keys.insert(cur_node->_keys.begin() + i, cur_data);
        cur_node->_pointers.insert(cur_node->_pointers.begin() + i + 1, 1, right_part);
        k = cur_node->_keys.size();
    }
    _size++;
    return std::make_pair(find(data.first), true);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::insert(tree_data_type&& data)
{
    std::stack<std::pair<btree_node**, size_t>> path {};
    path.push(std::make_pair(&_root, 0));
    if (_root == nullptr) {
        _root = _allocator.template new_object<btree_node>();
        _root->_keys.push_back(data);
        _root->_pointers.push_back(nullptr);
        _root->_pointers.push_back(nullptr);
        _size++;
        return std::make_pair(btree_iterator(path, 0), true);
    }
    btree_node* cur_node = _root;
    size_t k = cur_node->_keys.size();

    // move to leaf

    while (cur_node->_pointers[0] != nullptr) {
        size_t low = 0;
        size_t high = k + 1;
        size_t mid;
        while (low + 1 < high) {
            mid = low + (high - low) / 2;
            if (compare_keys(data.first, cur_node->_keys[mid -1].first)) {
                high = mid;
            } else if (compare_keys(cur_node->_keys[mid - 1].first, data.first)) {
                low = mid;
            } else {
                return std::make_pair(btree_iterator(path, mid - 1), false);
            }
        }
        path.push(std::make_pair(&(cur_node->_pointers[low]), low));
        cur_node = cur_node->_pointers[low];

        k = cur_node->_keys.size();
    }

    // find place

    size_t pos;
    
    size_t low = 0;
    size_t high = k + 1;
    size_t mid;
    while (low + 1 < high) {
        mid = low + (high - low) / 2;
        if (compare_keys(data.first, cur_node->_keys[mid - 1].first)) {
            high = mid;
        } else if (compare_keys(cur_node->_keys[mid -1].first, data.first)) {
            low = mid;
        } else {
            return std::make_pair(btree_iterator(path, mid - 1), false);
        }
    }
    pos = low;
    
    cur_node->_keys.insert(cur_node->_keys.begin() + pos, data);
    cur_node->_pointers.push_back(nullptr);

    // for (auto& i : cur_node->_keys) {
    //     std::cout << i.first << " ";
    // }
    // std::cout << std::endl;

    // fix rule

    if (k < maximum_keys_in_node) {
        _size++;
        return std::make_pair(btree_iterator(path, pos), true);
    }

    tree_data_type cur_data;
    size_t split = (maximum_keys_in_node + 1) / 2, i;
    btree_node *left_part, *right_part;
    while (k == maximum_keys_in_node) {
        cur_data = cur_node->_keys[split];
        i = path.top().second;
        path.pop();
        left_part = cur_node;
        if (path.empty()) {
            cur_node = _allocator.template new_object<btree_node>();
            cur_node->_pointers.push_back(left_part);
            _root = cur_node;
        } else {
            cur_node = *(path.top().first);
        }
        right_part = _allocator.template new_object<btree_node>();
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

        cur_node->_keys.insert(cur_node->_keys.begin() + i, cur_data);
        cur_node->_pointers.insert(cur_node->_pointers.begin() + i + 1, 1, right_part);
        k = cur_node->_keys.size();
    }
    _size++;
    return std::make_pair(find(data.first), true);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<typename... Args>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::emplace(Args&&... args)
{
    tree_data_type data (std::forward<Args>(args) ...);
    return insert(std::move(data));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::insert_or_assign(const tree_data_type& data)
{
    std::stack<std::pair<btree_node**, size_t>> path {};
    path.push(std::make_pair(&_root, 0));
    if (_root == nullptr) {
        _root = _allocator.template new_object<btree_node>();
        _root->_keys.push_back(data);
        _root->_pointers.push_back(nullptr);
        _root->_pointers.push_back(nullptr);
        _size++;
        return btree_iterator(path, 0);
    }
    btree_node* cur_node = _root;
    size_t k = cur_node->_keys.size();

    // move to leaf

    while (cur_node->_pointers[0] != nullptr) {
            size_t low = 0;
            size_t high = k + 1;
            size_t mid;
            while (low + 1 < high) {
                mid = low + (high - low) / 2;
                if (compare_keys(data.first, cur_node->_keys[mid - 1].first)) {
                    high = mid;
                } else if (compare_keys(cur_node->_keys[mid - 1].first, data.first)) {
                    low = mid;
                } else {
                    cur_node->_keys[mid - 1].second = data.second;
                    return btree_iterator(path, mid - 1);
                }
            }
            path.push(std::make_pair(&(cur_node->_pointers[low]), low));
            cur_node = cur_node->_pointers[low];
        
        k = cur_node->_keys.size();
    }

    // find place

    size_t pos;

    size_t low = 0;
    size_t high = k + 1;
    size_t mid;
    while (low + 1 < high) {
        mid = low + (high - low) / 2;
        if (compare_keys(data.first, cur_node->_keys[mid - 1].first)) {
            high = mid;
        } else if (compare_keys(cur_node->_keys[mid - 1].first, data.first)) {
            low = mid;
        } else {
            cur_node->_keys[mid - 1].second = data.second;
            return btree_iterator(path, mid - 1);
        }
    }
    pos = low;
    
    cur_node->_keys.insert(cur_node->_keys.begin() + pos, data);
    cur_node->_pointers.push_back(nullptr);

    // fix rule

    if (k < maximum_keys_in_node) {
        _size++;
        return btree_iterator(path, pos);
    }

    tree_data_type cur_data;
    size_t split = (maximum_keys_in_node + 1) / 2, i;
    btree_node *left_part, *right_part;
    while (k == maximum_keys_in_node) {
        cur_data = cur_node->_keys[split];
        i = path.top().second;
        path.pop();
        left_part = cur_node;
        if (path.empty()) {
            cur_node = _allocator.template new_object<btree_node>();
            cur_node->_pointers.push_back(left_part);
            _root = cur_node;
        } else {
            cur_node = *(path.top().first);
        }
        right_part = _allocator.template new_object<btree_node>();
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

        cur_node->_keys.insert(cur_node->_keys.begin() + i, cur_data);
        cur_node->_pointers.insert(cur_node->_pointers.begin() + i + 1, 1, right_part);
        k = cur_node->_keys.size();
    }
    _size++;
    return find(data.first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::insert_or_assign(tree_data_type&& data)
{
    std::stack<std::pair<btree_node**, size_t>> path {};
    path.push(std::make_pair(&_root, 0));
    if (_root == nullptr) {
        _root = _allocator.template new_object<btree_node>();
        _root->_keys.push_back(data);
        _root->_pointers.push_back(nullptr);
        _root->_pointers.push_back(nullptr);
        _size++;
        return btree_iterator(path, 0);
    }
    btree_node* cur_node = _root;
    size_t k = cur_node->_keys.size();

    // move to leaf

    while (cur_node->_pointers[0] != nullptr) {
        
        size_t low = 0;
        size_t high = k + 1;
        size_t mid;
        while (low + 1 < high) {
            mid = low + (high - low) / 2;
            if (compare_keys(data.first, cur_node->_keys[mid - 1].first)) {
                high = mid;
            } else if (compare_keys(cur_node->_keys[mid - 1].first, data.first)) {
                low = mid;
            } else {
                cur_node->_keys[mid - 1].second = data.second;
                return btree_iterator(path, mid - 1);
            }
        }
        path.push(std::make_pair(&(cur_node->_pointers[low]), low));
        cur_node = cur_node->_pointers[low];
        
        k = cur_node->_keys.size();
    }

    // find place

    size_t pos;

    size_t low = 0;
    size_t high = k + 1;
    size_t mid;
    while (low + 1 < high) {
        mid = low + (high - low) / 2;
        if (compare_keys(data.first, cur_node->_keys[mid - 1].first)) {
            high = mid;
        } else if (compare_keys(cur_node->_keys[mid - 1].first, data.first)) {
            low = mid;
        } else {
            cur_node->_keys[mid - 1].second = data.second;
            return btree_iterator(path, mid - 1);
        }
    }
    pos = low;

    cur_node->_keys.insert(cur_node->_keys.begin() + pos, data);
    cur_node->_pointers.push_back(nullptr);

    // fix rule

    if (k < maximum_keys_in_node) {
        _size++;
        return btree_iterator(path, pos);
    }

    tree_data_type cur_data;
    size_t split = (maximum_keys_in_node + 1) / 2, i;
    btree_node *left_part, *right_part;
    while (k == maximum_keys_in_node) {
        cur_data = cur_node->_keys[split];
        i = path.top().second;
        path.pop();
        left_part = cur_node;
        if (path.empty()) {
            cur_node = _allocator.template new_object<btree_node>();
            cur_node->_pointers.push_back(left_part);
            _root = cur_node;
        } else {
            cur_node = *(path.top().first);
        }
        right_part = _allocator.template new_object<btree_node>();
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

        cur_node->_keys.insert(cur_node->_keys.begin() + i, cur_data);
        cur_node->_pointers.insert(cur_node->_pointers.begin() + i + 1, 1, right_part);
        k = cur_node->_keys.size();
    }
    _size++;
    return find(data.first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<typename... Args>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::emplace_or_assign(Args&&... args)
{
    tree_data_type data (std::forward<Args>(args) ...);
    return insert_or_assign(std::move(data));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_iterator pos)
{
    if (pos == end()) {
        return end();
    }
    
    auto next = pos;
    next++;
    bool flag = false;
    tkey next_key;
    if (next != end()) {
        flag = true;
        next_key = (*(next)).first;
    }

    auto path = pos._path;
    btree_node* cur_node = *(path.top().first);
    size_t index = pos._index;
    if (cur_node->_pointers[0] != nullptr) {
        pos--;
        tree_data_type buf = cur_node->_keys[index];
        cur_node->_keys[index] = (*(pos._path.top().first))->_keys[pos._index];
        (*(pos._path.top().first))->_keys[pos._index] = buf;

        path = pos._path;
        cur_node = *(path.top().first);
        index = pos._index;
    }

    cur_node->_keys.erase(cur_node->_keys.begin() + index);
    cur_node->_pointers.pop_back();

    _size--;

    while (cur_node->_keys.size() < minimum_keys_in_node) {
        if (cur_node == _root) {
            if (cur_node->_keys.size() == 0) {
                _root = cur_node->_pointers[0];
                _allocator.delete_object(cur_node);
            }
            break;
        }
        index = path.top().second;
        path.pop();
        btree_node* parent = *(path.top().first);
        size_t k_cur = cur_node->_keys.size();
        size_t k_left = (index != 0) ?(parent->_pointers[index - 1])->_keys.size() : 0;
        size_t k_right = (parent->_pointers.size() > index + 1) ? (parent->_pointers[index + 1])->_keys.size() : 0;
        if (k_left > minimum_keys_in_node) {
            cur_node->_keys.insert(cur_node->_keys.begin(), parent->_keys[index - 1]);
            cur_node->_pointers.insert(cur_node->_pointers.begin(), parent->_pointers[index - 1]->_pointers[k_left]);

            parent->_keys[index - 1] = parent->_pointers[index - 1]->_keys[k_left - 1];

            parent->_pointers[index - 1]->_keys.pop_back();
            parent->_pointers[index - 1]->_pointers.pop_back();
        } else if (k_right > minimum_keys_in_node) {
            cur_node->_keys.push_back(parent->_keys[index]);
            cur_node->_pointers.push_back(parent->_pointers[index + 1]->_pointers[0]);

            parent->_keys[index] = parent->_pointers[index + 1]->_keys[0];

            parent->_pointers[index + 1]->_keys.erase(parent->_pointers[index + 1]->_keys.begin());
            parent->_pointers[index + 1]->_pointers.erase(parent->_pointers[index + 1]->_pointers.begin());
        } else if (index != 0) {
            btree_node* merged = parent->_pointers[index - 1];
            merged->_keys.push_back(parent->_keys[index - 1]);
            merged->_keys.insert(merged->_keys.end(), cur_node->_keys.begin(), cur_node->_keys.end());
            merged->_pointers.insert(merged->_pointers.end(), cur_node->_pointers.begin(), cur_node->_pointers.end());

            parent->_keys.erase(parent->_keys.begin() + index - 1);
            parent->_pointers.erase(parent->_pointers.begin() + index);

            _allocator.delete_object(cur_node);
        } else { // index == 0
            btree_node* bro = parent->_pointers[1];
            cur_node->_keys.push_back(parent->_keys[0]);
            cur_node->_keys.insert(cur_node->_keys.end(), bro->_keys.begin(), bro->_keys.end());
            cur_node->_pointers.insert(cur_node->_pointers.end(), bro->_pointers.begin(), bro->_pointers.end());

            parent->_keys.erase(parent->_keys.begin());
            parent->_pointers.erase(parent->_pointers.begin() + 1);

            _allocator.delete_object(bro);
        }

        cur_node = parent;
    }

    if (flag) {
        return find(next_key);
    } else {
        return end();
    }
    
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_const_iterator pos)
{
    auto buf = std::stack<std::pair<btree_node**, size_t>>();
    auto path = std::stack<std::pair<btree_node**, size_t>>();
    while (!pos._path.empty()) {
        buf.push(pos._path.pop());
    }
    while (!buf.empty()) {
        path.push(buf.pop());
    }
    btree_iterator it (path, pos._index);
    return erase(it);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_iterator beg, btree_iterator en)
{
    for (btree_iterator i = beg; i != en; i++) {
        erase(i);
    }
    return en;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_const_iterator beg, btree_const_iterator en)
{
    for (btree_const_iterator i = beg; i != en; i++) {
        erase(i);
    }
    return en;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(const tkey& key)
{
    btree_iterator it = find(key);
    return erase(it);
}

// endregion modifiers implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool compare_pairs(const typename B_tree<tkey, tvalue, compare, t>::tree_data_type &lhs,
                   const typename B_tree<tkey, tvalue, compare, t>::tree_data_type &rhs)
{
    return B_tree<tkey, tvalue, compare, t>::compare_pairs(lhs, rhs);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool compare_keys(const tkey &lhs, const tkey &rhs)
{
    return B_tree<tkey, tvalue, compare, t>::compare_keys(lhs, rhs);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::compare_pairs(const B_tree::tree_data_type &lhs,
                                                     const B_tree::tree_data_type &rhs) const
{
    return compare_keys(lhs.first, rhs.first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::compare_keys(const tkey &lhs, const tkey &rhs) const
{
    return compare::operator()(lhs, rhs);
}

#endif