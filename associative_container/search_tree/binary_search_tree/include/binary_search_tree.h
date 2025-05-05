#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_BINARY_SEARCH_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_BINARY_SEARCH_TREE_H

#include <stack>
#include <vector>
#include <logger.h>
#include <not_implemented.h>
#include <search_tree.h>
#include <ranges>
#include <pp_allocator.h>
#include <utility> // для std::forward

namespace __detail
{
    template<typename tkey, typename tvalue, typename compare, typename tag>
    class bst_impl;

    class BST_TAG;
}


template<typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, typename tag = __detail::BST_TAG>
class binary_search_tree : private compare
{
public:

    using value_type = std::pair<const tkey, tvalue>;

    friend class __detail::bst_impl<tkey, tvalue, compare, tag>;

protected:
    
    struct node
    {
    
    public:
        
        value_type data;

        node* parent;
        node* left_subtree;
        node* right_subtree;

        template<class ...Args>
        explicit node(node* parent, Args&& ...args);


        virtual ~node() =default;
    };

    inline bool compare_keys(const tkey& lhs, const tkey& rhs) const;

    inline bool compare_pairs(const value_type & lhs, const value_type & rhs) const;

public:


    // region iterators definition

    class prefix_iterator;
    class prefix_const_iterator;
    class prefix_reverse_iterator;
    class prefix_const_reverse_iterator;

    class infix_iterator;
    class infix_const_iterator;
    class infix_reverse_iterator;
    class infix_const_reverse_iterator;

    class postfix_iterator;
    class postfix_const_iterator;
    class postfix_reverse_iterator;
    class postfix_const_reverse_iterator;

    /** @brief Watch about behavior of reverse iterators.
     *
     * @example Arrow is common iterator
     *  1 2 3 -> 4 5 6 7
     *  *it == 4.
     *
     *  @example But reverse:
     *  1 2 3 \<- 4 5 6 7
     *  *rit == 3
     */

    class prefix_iterator
    {
    protected:

        node* _data;

        /** If iterator == end or before_begin _data points to nullptr, _backup to last node
         *
         */
        node* _backup;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        
        explicit prefix_iterator(node* data = nullptr);

        virtual ~prefix_iterator() =default;

        bool operator==(
            prefix_iterator const &other) const noexcept;
        
        bool operator!=(
            prefix_iterator const &other) const noexcept;
        
        prefix_iterator &operator++() & noexcept;
        
        prefix_iterator operator++(int not_used) noexcept;

        prefix_iterator &operator--() & noexcept;

        prefix_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;
        
    };
    
    class prefix_const_iterator
    {
    protected:

        prefix_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = const value_type&;
        using pointer = value_type* const;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit prefix_const_iterator(const node* data = nullptr);

        prefix_const_iterator(const prefix_iterator&) noexcept;

        virtual ~prefix_const_iterator() =default;

        bool operator==(
                prefix_const_iterator const &other) const noexcept;

        bool operator!=(
                prefix_const_iterator const &other) const noexcept;

        prefix_const_iterator &operator++() & noexcept;

        prefix_const_iterator operator++(int not_used) noexcept;

        prefix_const_iterator &operator--() & noexcept;

        prefix_const_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;
        
    };
    
    class prefix_reverse_iterator
    {
    protected:

        prefix_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit prefix_reverse_iterator(node* data = nullptr);

        prefix_reverse_iterator(const prefix_iterator&) noexcept;
        operator prefix_iterator() const noexcept;

        prefix_iterator base() const noexcept;

        virtual ~prefix_reverse_iterator() =default;

        bool operator==(prefix_reverse_iterator const &other) const noexcept;

        bool operator!=(prefix_reverse_iterator const &other) const noexcept;

        prefix_reverse_iterator &operator++() & noexcept;

        prefix_reverse_iterator operator++(int not_used) noexcept;

        prefix_reverse_iterator &operator--() & noexcept;

        prefix_reverse_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;
        
    };
    
    class prefix_const_reverse_iterator
    {
    protected:

        prefix_const_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = const value_type&;
        using pointer = value_type* const;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit prefix_const_reverse_iterator(const node* data = nullptr);

        prefix_const_reverse_iterator(const prefix_const_iterator&) noexcept;

        operator prefix_const_iterator() const noexcept;
        prefix_const_iterator base() const noexcept;

        virtual ~prefix_const_reverse_iterator() =default;

        bool operator==(prefix_const_reverse_iterator const &other) const noexcept;

        bool operator!=(prefix_const_reverse_iterator const &other) const noexcept;

        prefix_const_reverse_iterator &operator++() & noexcept;

        prefix_const_reverse_iterator operator++(int not_used) noexcept;

        prefix_const_reverse_iterator &operator--() & noexcept;

        prefix_const_reverse_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;
        
    };


    class infix_iterator
    {
    friend class binary_search_tree;
    protected:

        node* _data;

        /** If iterator == end or before_begin _data points to nullptr, _backup to last node
         *
         */
        node* _backup;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit infix_iterator(node* data = nullptr);

        virtual ~infix_iterator() =default;

        bool operator==(
                infix_iterator const &other) const noexcept;

        bool operator!=(
                infix_iterator const &other) const noexcept;

        infix_iterator &operator++() & noexcept;

        infix_iterator operator++(int not_used) noexcept;

        infix_iterator &operator--() & noexcept;

        infix_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;

    };

    class infix_const_iterator
    {
    friend class binary_search_tree;
    protected:

        infix_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = const value_type&;
        using pointer = value_type* const;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit infix_const_iterator(const node* data = nullptr);

        infix_const_iterator(const infix_iterator&) noexcept;

        virtual ~infix_const_iterator() =default;

        bool operator==(
                infix_const_iterator const &other) const noexcept;

        bool operator!=(
                infix_const_iterator const &other) const noexcept;

        infix_const_iterator &operator++() & noexcept;

        infix_const_iterator operator++(int not_used) noexcept;

        infix_const_iterator &operator--() & noexcept;

        infix_const_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;

    };

    class infix_reverse_iterator
    {
    protected:

        infix_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit infix_reverse_iterator(node* data = nullptr);

        infix_reverse_iterator(const infix_iterator&) noexcept;
        operator infix_iterator() const noexcept;

        infix_iterator base() const noexcept;

        virtual ~infix_reverse_iterator() =default;

        bool operator==(infix_reverse_iterator const &other) const noexcept;

        bool operator!=(infix_reverse_iterator const &other) const noexcept;

        infix_reverse_iterator &operator++() & noexcept;

        infix_reverse_iterator operator++(int not_used) noexcept;

        infix_reverse_iterator &operator--() & noexcept;

        infix_reverse_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;

    };

    class infix_const_reverse_iterator
    {
    protected:

        infix_const_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = const value_type&;
        using pointer = value_type* const;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit infix_const_reverse_iterator(const node* data = nullptr);

        infix_const_reverse_iterator(const infix_const_iterator&) noexcept;

        operator infix_const_iterator() const noexcept;
        infix_const_iterator base() const noexcept;

        virtual ~infix_const_reverse_iterator() =default;

        bool operator==(infix_const_reverse_iterator const &other) const noexcept;

        bool operator!=(infix_const_reverse_iterator const &other) const noexcept;

        infix_const_reverse_iterator &operator++() & noexcept;

        infix_const_reverse_iterator operator++(int not_used) noexcept;

        infix_const_reverse_iterator &operator--() & noexcept;

        infix_const_reverse_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;

    };


    class postfix_iterator
    {
    protected:

        node* _data;

        /** If iterator == end or before_begin _data points to nullptr, _backup to last node
         *
         */
        node* _backup;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit postfix_iterator(node* data = nullptr);

        virtual ~postfix_iterator() =default;

        bool operator==(
                postfix_iterator const &other) const noexcept;

        bool operator!=(
                postfix_iterator const &other) const noexcept;

        postfix_iterator &operator++() & noexcept;

        postfix_iterator operator++(int not_used) noexcept;

        postfix_iterator &operator--() & noexcept;

        postfix_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;

    };

    class postfix_const_iterator
    {
    protected:

        postfix_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = const value_type&;
        using pointer = value_type* const;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit postfix_const_iterator(const node* data = nullptr);

        postfix_const_iterator(const postfix_iterator&) noexcept;

        virtual ~postfix_const_iterator() =default;

        bool operator==(
                postfix_const_iterator const &other) const noexcept;

        bool operator!=(
                postfix_const_iterator const &other) const noexcept;

        postfix_const_iterator &operator++() & noexcept;

        postfix_const_iterator operator++(int not_used) noexcept;

        postfix_const_iterator &operator--() & noexcept;

        postfix_const_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;

    };

    class postfix_reverse_iterator
    {
    protected:

        postfix_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit postfix_reverse_iterator(node* data = nullptr);

        postfix_reverse_iterator(const postfix_iterator&) noexcept;
        operator postfix_iterator() const noexcept;

        postfix_iterator base() const noexcept;

        virtual ~postfix_reverse_iterator() =default;

        bool operator==(postfix_reverse_iterator const &other) const noexcept;

        bool operator!=(postfix_reverse_iterator const &other) const noexcept;

        postfix_reverse_iterator &operator++() & noexcept;

        postfix_reverse_iterator operator++(int not_used) noexcept;

        postfix_reverse_iterator &operator--() & noexcept;

        postfix_reverse_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;

    };

    class postfix_const_reverse_iterator
    {
    protected:

        postfix_const_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = const value_type&;
        using pointer = value_type* const;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit postfix_const_reverse_iterator(const node* data = nullptr);

        postfix_const_reverse_iterator(const postfix_const_iterator&) noexcept;

        operator postfix_const_iterator() const noexcept;
        postfix_const_iterator base() const noexcept;

        virtual ~postfix_const_reverse_iterator() =default;

        bool operator==(postfix_const_reverse_iterator const &other) const noexcept;

        bool operator!=(postfix_const_reverse_iterator const &other) const noexcept;

        postfix_const_reverse_iterator &operator++() & noexcept;

        postfix_const_reverse_iterator operator++(int not_used) noexcept;

        postfix_const_reverse_iterator &operator--() & noexcept;

        postfix_const_reverse_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;
        size_t depth() const noexcept;

    };



    friend class prefix_iterator;
    friend class prefix_const_iterator;
    friend class prefix_reverse_iterator;
    friend class prefix_const_reverse_iterator;

    friend class postfix_iterator;
    friend class postfix_const_iterator;
    friend class postfix_reverse_iterator;
    friend class postfix_const_reverse_iterator;

    friend class infix_iterator;
    friend class infix_const_iterator;
    friend class infix_reverse_iterator;
    friend class infix_const_reverse_iterator;

    // endregion iterators definition


protected:
    
    node *_root;
    logger* _logger;
    size_t _size;
    
    /** You should use coercion ctor or template methods of allocator
     */
    pp_allocator<value_type> _allocator;

public:
    explicit binary_search_tree(
            const compare& comp = compare(),
            pp_allocator<value_type> alloc = pp_allocator<value_type>(),
            logger *log = nullptr);

    explicit binary_search_tree(
            pp_allocator<value_type> alloc,
            const compare& comp = compare(),
            logger *log = nullptr);


    template<input_iterator_for_pair<tkey, tvalue> iterator>
    explicit binary_search_tree(iterator begin, iterator end, const compare& cmp = compare(), 
                                pp_allocator<value_type> alloc = pp_allocator<value_type>(), 
                                logger* log = nullptr);

    template<std::ranges::input_range Range>
    explicit binary_search_tree(Range&& range, const compare& cmp = compare(),
                                pp_allocator<value_type> alloc = pp_allocator<value_type>(),
                                logger* log = nullptr);


    binary_search_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(),
                       pp_allocator<value_type> alloc = pp_allocator<value_type>(), 
                       logger* log = nullptr);

public:
    
    binary_search_tree(const binary_search_tree &other);
    
    binary_search_tree(binary_search_tree &&other) noexcept;
    
    binary_search_tree &operator=(const binary_search_tree &other);
    
    binary_search_tree &operator=(binary_search_tree &&other) noexcept;
    
    virtual ~binary_search_tree();

    void delete_subtree (node* n);

public:

    //region CRUD definition

    tvalue& at(const tkey& key);
    const tvalue& at(const tkey& key) const;

    tvalue& operator[](const tkey& key);
    tvalue& operator[](tkey&& key);

    bool empty() const noexcept;

    size_t size() const noexcept;

    void clear() noexcept;

    std::pair<infix_iterator, bool> insert(const value_type&);
    std::pair<infix_iterator, bool> insert(value_type&&);

    template<std::input_iterator InputIt>
    void insert(InputIt first, InputIt last);

    template<std::ranges::input_range R>
    void insert_range( R&& rg );

    template<class ...Args>
    std::pair<infix_iterator, bool> emplace(Args&&...args);

    infix_iterator insert_or_assign(const value_type&);
    infix_iterator insert_or_assign(value_type&&);

    template<std::input_iterator InputIt >
    void insert_or_assign(InputIt first, InputIt last);


    template<class ...Args>
    infix_iterator emplace_or_assign(Args&&...args);

    virtual void swap(binary_search_tree& other) noexcept;

    bool contains(const tkey& key) const;

    infix_iterator find(const tkey&);
    infix_const_iterator find(const tkey&) const;


    infix_iterator lower_bound(const tkey&);
    infix_const_iterator lower_bound(const tkey&) const;

    infix_iterator upper_bound(const tkey&);
    infix_const_iterator upper_bound(const tkey&) const;

    infix_iterator erase(infix_iterator pos);
    infix_iterator erase(infix_const_iterator pos);

    infix_iterator erase(infix_iterator first, infix_iterator last);
    infix_iterator erase(infix_const_iterator first, infix_const_iterator last);

    size_t erase(const tkey& key);

public:
    
    // region iterators requests definition

    infix_iterator begin() noexcept;

    infix_iterator end() noexcept;

    infix_const_iterator begin() const noexcept;

    infix_const_iterator end() const noexcept;

    infix_const_iterator cbegin() const noexcept;

    infix_const_iterator cend() const noexcept;

    infix_reverse_iterator rbegin() noexcept;

    infix_reverse_iterator rend() noexcept;

    infix_const_reverse_iterator rbegin() const noexcept;

    infix_const_reverse_iterator rend() const noexcept;

    infix_const_reverse_iterator crbegin() const noexcept;

    infix_const_reverse_iterator crend() const noexcept;

    
    prefix_iterator begin_prefix() noexcept;
    
    prefix_iterator end_prefix() noexcept;

    prefix_const_iterator begin_prefix() const noexcept;

    prefix_const_iterator end_prefix() const noexcept;
    
    prefix_const_iterator cbegin_prefix() const noexcept;
    
    prefix_const_iterator cend_prefix() const noexcept;
    
    prefix_reverse_iterator rbegin_prefix() noexcept;
    
    prefix_reverse_iterator rend_prefix() noexcept;

    prefix_const_reverse_iterator rbegin_prefix() const noexcept;

    prefix_const_reverse_iterator rend_prefix() const noexcept;
    
    prefix_const_reverse_iterator crbegin_prefix() const noexcept;
    
    prefix_const_reverse_iterator crend_prefix() const noexcept;

    
    infix_iterator begin_infix() noexcept;
    
    infix_iterator end_infix() noexcept;

    infix_const_iterator begin_infix() const noexcept;

    infix_const_iterator end_infix() const noexcept;
    
    infix_const_iterator cbegin_infix() const noexcept;
    
    infix_const_iterator cend_infix() const noexcept;
    
    infix_reverse_iterator rbegin_infix() noexcept;
    
    infix_reverse_iterator rend_infix() noexcept;

    infix_const_reverse_iterator rbegin_infix() const noexcept;

    infix_const_reverse_iterator rend_infix() const noexcept;
    
    infix_const_reverse_iterator crbegin_infix() const noexcept;
    
    infix_const_reverse_iterator crend_infix() const noexcept;

    
    postfix_iterator begin_postfix() noexcept;
    
    postfix_iterator end_postfix() noexcept;

    postfix_const_iterator begin_postfix() const noexcept;

    postfix_const_iterator end_postfix() const noexcept;
    
    postfix_const_iterator cbegin_postfix() const noexcept;
    
    postfix_const_iterator cend_postfix() const noexcept;
    
    postfix_reverse_iterator rbegin_postfix() noexcept;
    
    postfix_reverse_iterator rend_postfix() noexcept;

    postfix_const_reverse_iterator rbegin_postfix() const noexcept;

    postfix_const_reverse_iterator rend_postfix() const noexcept;
    
    postfix_const_reverse_iterator crbegin_postfix() const noexcept;
    
    postfix_const_reverse_iterator crend_postfix() const noexcept;
    
    // endregion iterators requests definition

protected:



    // region subtree rotations definition
    
    static void small_left_rotation(node *&subtree_root) noexcept;

    static void small_right_rotation(node *&subtree_root) noexcept;

    static void big_left_rotation(node *&subtree_root) noexcept;

    static void big_right_rotation(node *&subtree_root) noexcept;

    static void double_left_rotation(node *&subtree_root) noexcept;

    static void double_right_rotation(node *&subtree_root) noexcept;
    
    // endregion subtree rotations definition
    
};

namespace __detail
{
    template<typename tkey, typename tvalue, typename compare, typename tag>
    class bst_impl
    {
        friend class binary_search_tree<tkey, tvalue, compare, tag>;
        template<class ...Args>
        static binary_search_tree<tkey, tvalue, compare, tag>::node* create_node(binary_search_tree<tkey, tvalue, compare, tag>& cont, Args&& ...args);

        static void delete_node(binary_search_tree<tkey, tvalue, compare, tag>& cont, typename binary_search_tree<tkey, tvalue, compare, tag>::node* node);

        //Does not invalidate node*, needed for splay tree
        static void post_search(binary_search_tree<tkey, tvalue, compare, tag>::node**){}

        //Does not invalidate node*
        static void post_insert(binary_search_tree<tkey, tvalue, compare, tag>& cont, binary_search_tree<tkey, tvalue, compare, tag>::node**){}

        static void erase(binary_search_tree<tkey, tvalue, compare, tag>& cont, binary_search_tree<tkey, tvalue, compare, tag>::node**);

        static void swap(binary_search_tree<tkey, tvalue, compare, tag>& lhs, binary_search_tree<tkey, tvalue, compare, tag>& rhs) noexcept;
    };
}

template<typename tkey, typename tvalue, typename compare, typename tag>
void __detail::bst_impl<tkey, tvalue, compare, tag>::swap(binary_search_tree<tkey, tvalue, compare, tag> &lhs,
                                                binary_search_tree<tkey, tvalue, compare, tag> &rhs) noexcept
{
    std::swap(lhs._root, rhs._root);
    std::swap(lhs._size, rhs._size);
    std::swap(lhs._logger, rhs._logger);
    std::swap(lhs._allocator, rhs._allocator);
    std::swap(static_cast<compare&>(lhs), static_cast<compare&>(rhs));
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<input_iterator_for_pair<tkey, tvalue> iterator>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(iterator begin, iterator end, const compare &cmp,
                                                                   pp_allocator<typename binary_search_tree<tkey, tvalue, compare, tag>::value_type> alloc, logger *log)
    : compare(cmp)
    , _root(nullptr)
    , _logger(log)
    , _size(0)
    , _allocator(alloc)
{
    for (; begin != end; ++begin) {
        insert(*begin);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::compare_pairs(const binary_search_tree::value_type &lhs,
                                                              const binary_search_tree::value_type &rhs) const
{
    return compare_keys(lhs.first, rhs.first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::compare_keys(const tkey &lhs, const tkey &rhs) const
{
    return static_cast<const compare&>(*this)(lhs, rhs);
}


// 3 штуки, благодаря которым можно не писать явно шаблонные параметры, они будут опредляться по аргументам конструктора
template<typename compare, typename U, typename iterator>
explicit binary_search_tree(iterator begin, iterator end, const compare& cmp = compare(),
                            pp_allocator<U> alloc = pp_allocator<U>(),
                            logger* log = nullptr) -> binary_search_tree<const typename std::iterator_traits<iterator>::value_type::first_type, typename std::iterator_traits<iterator>::value_type::second_type, compare>;

template<typename compare, typename U, std::ranges::forward_range Range>
explicit binary_search_tree(Range&& range, const compare& cmp = compare(),
        pp_allocator<U> alloc = pp_allocator<U>(),
        logger* log = nullptr) -> binary_search_tree<const typename std::iterator_traits<typename std::ranges::iterator_t<Range>>::value_type::first_type, typename std::iterator_traits<typename std::ranges::iterator_t<Range>>::value_type::second_type, compare> ;

template<typename tkey, typename tvalue, typename compare, typename U>
binary_search_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(),
        pp_allocator<U> alloc = pp_allocator<U>(),
        logger* log = nullptr) -> binary_search_tree<tkey, tvalue, compare>;


// region node implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<class ...Args>
binary_search_tree<tkey, tvalue, compare, tag>::node::node(node* parent_, Args&& ...args)
    : data(std::forward<Args>(args)...)
    , parent(parent_)
    , left_subtree(nullptr)
    , right_subtree(nullptr)
{}

// endregion node implementation

// + region prefix_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::prefix_iterator(node* data)
    : _data(data)
    , _backup (nullptr)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator==(
        prefix_iterator const &other) const noexcept
{
    return (_data == other._data);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator!=(
        prefix_iterator const &other) const noexcept
{
    return (_data != other._data);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator++() & noexcept
{
    if (!_data) {
        if (_backup == nullptr) {
            return *this;
        }
        if (_backup->parent == nullptr) {
            _data = _backup;
            _backup = nullptr;
        }
        return *this;
    }
    _backup = _data;

    if (_data->left_subtree != nullptr) {
        _data = _data->left_subtree;
    } else if (_data->right_subtree != nullptr) {
        _data = _data->right_subtree;
    } else {
        while ((_data->parent != nullptr) && (_data->parent->left_subtree != _data)) {
            _data = _data->parent;
        }
        if (_data->parent != nullptr) {
            _data = _data->parent->right_subtree;
        } else {
            _data = nullptr;
        }
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator++(int not_used) noexcept
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator--() & noexcept
{
    if (!_data) {
        if (_backup == nullptr) {
            return *this;
        }
        if ((_backup->parent != nullptr) || ((_backup->left_subtree == nullptr) && (_backup->right_subtree == nullptr))) {
            _data = _backup;
            _backup = nullptr;
        }
        return *this;
    }
    _backup = _data;

    if (_data->parent == nullptr) {
        _data = nullptr;
        return *this;
    }

    if ((_data->parent->left_subtree == _data) || (_data->parent->left_subtree == nullptr)) {
        _data = _data->parent;
    } else {
        _data = _data->parent;
        while ((_data->left_subtree != nullptr) || (_data->right_subtree != nullptr)) {
            if (_data->right_subtree != nullptr) {
                _data = _data->right_subtree;
            } else {
                _data = _data->left_subtree;
            }
        }   
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator--(int not_used) const noexcept
{
    auto tmp = *this;
    --(*this);
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator*() const
{
    return _data->data;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator->() noexcept
{
    if (_data == nullptr) {
        return nullptr;
    }
    return &(_data->data);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::depth() const noexcept
{
    if (_data == nullptr) {
        return 0;
    }
    size_t res = 0;
    node* buf = _data;
    while (buf->parent != nullptr) {
        res++;
        buf = buf->parent;
    }
    return res;
}

// endregion prefix_iterator implementation

// + region prefix_const_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::prefix_const_iterator(const node* data)
    : _base(const_cast<node*>(data))
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::prefix_const_iterator(const prefix_iterator& it) noexcept
    : _base(const_cast<node*>(it._data))
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator==(
        prefix_const_iterator const &other) const noexcept
{
    return _base == other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator!=(
        prefix_const_iterator const &other) const noexcept
{
    return _base != other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator++() & noexcept
{
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator++(int not_used) noexcept
{
    auto tmp = *this;
    ++_base;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator--() & noexcept
{
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator--(int not_used) const noexcept
{
    auto tmp = *this;
    --_base;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator*() const
{
    return *_base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator->() noexcept
{
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::depth() const noexcept
{
    return _base.depth();
}

// endregion prefix_const_iterator implementation

// + region prefix_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::prefix_reverse_iterator(node* data)
    :_base(data)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::prefix_reverse_iterator(const prefix_iterator& it) noexcept
    :_base(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator() const noexcept
{
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::base() const noexcept
{
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator==(prefix_reverse_iterator const &other) const noexcept
{
    return _base == other.base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator!=(prefix_reverse_iterator const &other) const noexcept
{
    return _base != other.base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator++() & noexcept
{
    --base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator++(int not_used) noexcept
{
    auto tmp = *this;
    --base;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator--() & noexcept
{
    ++base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator--(int not_used) const noexcept
{
    auto tmp = *this;
    ++base;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator*() const
{
    auto tmp = _base;
    --tmp;
    return *tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator->() noexcept
{
    auto tmp = _base;
    --tmp;
    return tmp.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::depth() const noexcept
{
    auto tmp = _base;
    --tmp;
    return tmp.depth();
}

// endregion prefix_reverse_iterator implementation

// + region prefix_const_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(const node* data)
    : _base(data)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(const prefix_const_iterator& it) noexcept
    :_base(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator() const noexcept
{
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::base() const noexcept
{
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator==(prefix_const_reverse_iterator const &other) const noexcept
{
    return _base == other.base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator!=(prefix_const_reverse_iterator const &other) const noexcept
{
    return _base != other.base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator++() & noexcept
{
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator++(int not_used) noexcept
{
    auto tmp = *this;
    --_base;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator--() & noexcept
{
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator--(int not_used) const noexcept
{
    auto tmp = *this;
    ++_base;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator*() const
{
    auto tmp = _base;
    --tmp;
    return *tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator->() noexcept
{
    auto tmp = _base;
    --tmp;
    return tmp.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::depth() const noexcept
{
    auto tmp = _base;
    --tmp;
    return tmp.depth();
}

// endregion prefix_const_reverse_iterator implementation

// + region infix_iterator implementation
template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::infix_iterator(node* data)
    : _data(data)
    , _backup (nullptr)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator==(infix_iterator const &other) const noexcept
{
    return (_data == other._data);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator!=(infix_iterator const &other) const noexcept
{
    return (_data != other._data);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator++() & noexcept
{
    if (!_data){
        if (_backup == nullptr) {
            return *this;
        }
        if ((_backup->parent == nullptr) || (_backup->parent->left_subtree == _backup)) {
            _data = _backup;
            _backup = nullptr;
        }
        return *this;
    }
    _backup = _data;

    if (_data->right_subtree == nullptr) {
        if (_data->parent == nullptr) {
            _data = nullptr;
            return *this;
        }
        auto buf = _data;
        _data = _data->parent;
        while (_data->right_subtree == buf) {
            if (_data->parent == nullptr) {
                _data = nullptr;
                return *this;
            }
            buf = _data;
            _data = _data->parent;
        }
        return *this;
    } else {
        _data = _data->right_subtree;
        while (_data->left_subtree != nullptr) {
            _data = _data->left_subtree;
        }
        return *this;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator++(int not_used) noexcept
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator--() & noexcept
{
    if (!_data) {
        if (_backup == nullptr) {
            return *this;
        }
        if ((_backup->parent == nullptr) || (_backup->parent->right_subtree == _backup)) {
            _data = _backup;
            _backup = nullptr;
        }
        return *this;
    }
    _backup = _data;

    if (_data->left_subtree == nullptr) {
        if (_data->parent == nullptr) {
            _data = nullptr;
            return *this;
        }
        auto buf = _data;
        _data = _data->parent;
        while (_data->left_subtree == buf) {
            if (_data->parent == nullptr) {
                _data = nullptr;
                return *this;
            }
            buf = _data;
            _data = _data->parent;
        }
        return *this;
    } else {
        _data = _data->left_subtree;
        while (_data->right_subtree != nullptr) {
            _data = _data->right_subtree;
        }
        return *this;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator--(int not_used) const noexcept
{
    auto tmp = *this;
    --(*this);
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator*() const
{
    return _data->data;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator->() noexcept
{
    if (_data == nullptr) {
        return nullptr;
    }
    return &(_data->data);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::depth() const noexcept
{
    if (_data == nullptr) {
        return 0;
    }
    size_t res = 0;
    node* buf = _data;
    while (buf->parent != nullptr) {
        res++;
        buf = buf->parent;
    }
    return res;
}

// endregion infix_iterator implementation

// + region infix_const_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::infix_const_iterator(const node* data)
    : _base(const_cast<node*>(data))
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::infix_const_iterator(const infix_iterator& it) noexcept
    : _base(const_cast<node*>(it._data))
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator==(infix_const_iterator const &other) const noexcept
{
    return _base == other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator!=(infix_const_iterator const &other) const noexcept
{
    return _base != other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator++() & noexcept
{
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator++(int not_used) noexcept
{
    auto tmp = *this;
    ++_base;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator--() & noexcept
{
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator--(int not_used) const noexcept
{
    auto tmp = *this;
    --_base;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator*() const
{
    return *_base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator->() noexcept
{
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::depth() const noexcept
{
    return _base.depth();
}

// endregion infix_const_iterator implementation

// + region infix_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::infix_reverse_iterator(node* data)
    :_base(data)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::infix_reverse_iterator(const infix_iterator& it) noexcept
    :_base(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator() const noexcept
{
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::base() const noexcept
{
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator==(infix_reverse_iterator const &other) const noexcept
{
    return _base == other.base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator!=(infix_reverse_iterator const &other) const noexcept
{
    return _base != other.base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator++() & noexcept
{
    --base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator++(int not_used) noexcept
{
    auto tmp = *this;
    --base;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator--() & noexcept
{
    ++base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator--(int not_used) const noexcept
{
    auto tmp = *this;
    ++base;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator*() const
{
    auto tmp = _base;
    --tmp;
    return *tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator->() noexcept
{
    auto tmp = _base;
    --tmp;
    return tmp.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::depth() const noexcept
{
    auto tmp = _base;
    --tmp;
    return tmp.depth();
}

// endregion infix_reverse_iterator implementation

// + region infix_const_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::infix_const_reverse_iterator(const node* data)
    : _base(data)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::infix_const_reverse_iterator(const infix_const_iterator& it) noexcept
    :_base(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator() const noexcept
{
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::base() const noexcept
{
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator==(infix_const_reverse_iterator const &other) const noexcept
{
    return _base == other.base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator!=(infix_const_reverse_iterator const &other) const noexcept
{
    return _base != other.base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator++() & noexcept
{
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator++(int not_used) noexcept
{
    auto tmp = *this;
    --_base;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator--() & noexcept
{
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator--(int not_used) const noexcept
{
    auto tmp = *this;
    ++_base;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator*() const
{
    auto tmp = _base;
    --tmp;
    return *tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator->() noexcept
{
    auto tmp = _base;
    --tmp;
    return tmp.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::depth() const noexcept
{
    auto tmp = _base;
    --tmp;
    return tmp.depth();
}

// endregion infix_const_reverse_iterator implementation

// + region postfix_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::postfix_iterator(node* data)
    : _data(data)
    , _backup (nullptr)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator==(postfix_iterator const &other) const noexcept
{
    return (_data == other._data);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator!=(postfix_iterator const &other) const noexcept
{
    return (_data != other._data);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator++() & noexcept
{
    if (!_data) {
        if (_backup == nullptr) {
            return *this;
        }
        if ((_backup->parent != nullptr) || ((_backup->left_subtree == nullptr) && (_backup->right_subtree == nullptr))) {
            _data = _backup;
            _backup = nullptr;
        }
        return *this;
    }
    _backup = _data;

    if (_data->parent == nullptr) {
        _data = nullptr;
        return *this;
    }

    if ((_data->parent->right_subtree == _data) || (_data->parent->right_subtree == nullptr)) {
        _data = _data->parent;
    } else {
        _data = _data->parent->right_subtree;
        while ((_data->left_subtree != nullptr) || (_data->right_subtree != nullptr)) {
            if (_data->left_subtree != nullptr) {
                _data = _data->left_subtree;
            } else {
                _data = _data->right_subtree;
            }
        }   
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator++(int not_used) noexcept
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator--() & noexcept
{
    if (!_data) {
        if (_backup == nullptr) {
            return *this;
        }
        if (_backup->parent == nullptr) {
            _data = _backup;
            _backup = nullptr;
        }
        return *this;
    }
    _backup = _data;

    if (_data->right_subtree != nullptr) {
        _data = _data->right_subtree;
    } else if (_data->left_subtree != nullptr) {
        _data = _data->left_subtree;
    } else {
        while ((_data->parent != nullptr) && (_data->parent->right_subtree != _data)) {
            _data = _data->parent;
        }
        if (_data->parent != nullptr) {
            _data = _data->parent->left_subtree;
        } else {
            _data = nullptr;
        }
    }
    return *this;

}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator--(int not_used) const noexcept
{
    auto tmp = *this;
    --(*this);
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator*() const
{
    return _data->data;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator->() noexcept
{
    if (_data == nullptr) {
        return nullptr;
    }
    return &(_data->data);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::depth() const noexcept
{
    if (_data == nullptr) {
        return 0;
    }
    size_t res = 0;
    node* buf = _data;
    while (buf->parent != nullptr) {
        res++;
        buf = buf->parent;
    }
    return res;
}

// endregion postfix_iterator implementation

// + region postfix_const_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::postfix_const_iterator(const node* data)
    : _base(const_cast<node*>(data))
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::postfix_const_iterator(const postfix_iterator& it) noexcept
    : _base(const_cast<node*>(it._data))
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator==(postfix_const_iterator const &other) const noexcept
{
    return _base == other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator!=(postfix_const_iterator const &other) const noexcept
{
    return _base != other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator++() & noexcept
{
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator++(int not_used) noexcept
{
    auto tmp = *this;
    ++_base;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator--() & noexcept
{
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator--(int not_used) const noexcept
{
    auto tmp = *this;
    --_base;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator*() const
{
    return *_base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator->() noexcept
{
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::depth() const noexcept
{
    return _base.depth();
}

// endregion postfix_const_iterator implementation

// + region postfix_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::postfix_reverse_iterator(node* data)
    :_base(data)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::postfix_reverse_iterator(const postfix_iterator& it) noexcept
    :_base(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator() const noexcept
{
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::base() const noexcept
{
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator==(postfix_reverse_iterator const &other) const noexcept
{
    return _base == other.base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator!=(postfix_reverse_iterator const &other) const noexcept
{
    return _base != other.base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator++() & noexcept
{
    --base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator++(int not_used) noexcept
{
    auto tmp = *this;
    --base;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator--() & noexcept
{
    ++base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator--(int not_used) const noexcept
{
    auto tmp = *this;
    ++base;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator*() const
{
    auto tmp = _base;
    --tmp;
    return *tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator->() noexcept
{
    auto tmp = _base;
    --tmp;
    return tmp.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::depth() const noexcept
{
    auto tmp = _base;
    --tmp;
    return tmp.depth();
}

// endregion postfix_reverse_iterator implementation

// + region postfix_const_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(const node* data)
    : _base(data)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(const postfix_const_iterator& it) noexcept
    :_base(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator() const noexcept
{
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::base() const noexcept
{
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator==(postfix_const_reverse_iterator const &other) const noexcept
{
    return _base == other.base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator!=(postfix_const_reverse_iterator const &other) const noexcept
{
    return _base != other.base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator++() & noexcept
{
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator++(int not_used) noexcept
{
    auto tmp = *this;
    --_base;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator--() & noexcept
{
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator--(int not_used) const noexcept
{
    auto tmp = *this;
    ++_base;
    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator*() const
{
    auto tmp = _base;
    --tmp;
    return *tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator->() noexcept
{
    auto tmp = _base;
    --tmp;
    return tmp.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::depth() const noexcept
{
    auto tmp = _base;
    --tmp;
    return tmp.depth();
}

// endregion postfix_const_reverse_iterator implementation


// region binary_search_tree implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(
        const compare& comp,
        pp_allocator<value_type> alloc,
        logger *log)
    : compare(comp)
    , _root(nullptr)
    , _logger(log)
    , _size(0)
    , _allocator(alloc)
{}


template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(
        pp_allocator<value_type> alloc,
        const compare& comp,
        logger *log)
    : compare(comp)
    , _root(nullptr)
    , _logger(log)
    , _size(0)
    , _allocator(alloc)
{}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<std::ranges::input_range Range>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(
        Range&& range,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* log)
    : compare(cmp)
    , _root(nullptr)
    , _logger(log)
    , _size(0)
    , _allocator(alloc)

{
    for (auto &&p : range) {
        insert(p);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(
        std::initializer_list<std::pair<tkey, tvalue>> data,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* log)
    : compare(cmp)
    , _root(nullptr)
    , _logger(log)
    , _size(0)
    , _allocator(alloc)
{
    for (auto &p : data) {
        insert(p);
    }
}


// endregion binary_search_tree implementation

// region binary_search_tree 5_rules implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(const binary_search_tree &other)
    : compare(other)
    , _root(nullptr)
    , _logger(other._logger)
    , _size(0)
    , _allocator(other._allocator)
{
    for (auto it = other.begin_prefix(); it != other.end_prefix(); ++it) {
        insert(*it);
    }
}


template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(binary_search_tree &&other) noexcept
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


template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>&
binary_search_tree<tkey, tvalue, compare, tag>::operator=(const binary_search_tree &other)
{
    if (this != &other) {
        delete_subtree(_root);

        compare::operator=(other);
        _allocator = other._allocator;
        _logger = other._logger;
        _root = nullptr;
        _size = 0;

        for (auto it = other.begin_prefix(); it != other.end_prefix(); ++it) {
            insert(*it);
        }
    }
    return *this;

}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>&
binary_search_tree<tkey, tvalue, compare, tag>::operator=(binary_search_tree &&other) noexcept
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

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::~binary_search_tree()
{
    delete_subtree(_root);    
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::delete_subtree (node* n)
{
    if (!n) return;
    delete_subtree(n->left_subtree);
    delete_subtree(n->right_subtree);
    delete n;
}

// endregion binary_search_tree 5_rules implementation

// region binary_search_tree methods_access implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
tvalue& binary_search_tree<tkey, tvalue, compare, tag>::at(const tkey& key)
{
    node** cur = &_root;
    while (*cur != nullptr) {
        if (compare_keys((*cur)->data.first, key)) {
            cur = &((*cur)->right_subtree);
        } else if (compare_keys(key, (*cur)->data.first)) {
            cur = &((*cur)->left_subtree);
        } else {
            break;
        }
    }
    
    if (*cur == nullptr) {
        throw std::out_of_range("");
    }
    __detail::bst_impl<tkey, tvalue, compare, tag>::post_search(cur);
    return (*cur)->data.second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
const tvalue& binary_search_tree<tkey, tvalue, compare, tag>::at(const tkey& key) const
{
    return at(key);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
tvalue& binary_search_tree<tkey, tvalue, compare, tag>::operator[](const tkey& key)
{
    return at(key);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
tvalue& binary_search_tree<tkey, tvalue, compare, tag>::operator[](tkey&& key)
{
    return at(key);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::empty() const noexcept
{
    return (_size == 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::size() const noexcept
{
    return _size;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::clear() noexcept
{
    if (!(this->empty())) {
        delete_subtree(_root);
    }
}

// endregion binary_search_tree methods_access implementation

// region binary_search_tree methods_insert and methods_emplace implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
std::pair<typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator, bool>
binary_search_tree<tkey, tvalue, compare, tag>::insert(const value_type& value)
{
    node** cur = &_root;
    node* prev = nullptr;

    while (*cur != nullptr) {
        if (compare_keys((*cur)->data.first, value.first)) {
            prev = *cur;
            cur = &((*cur)->right_subtree);
        } else if (compare_keys(value.first, (*cur)->data.first)) {
            prev = *cur;
            cur = &((*cur)->left_subtree);
        } else {
            return std::make_pair(infix_iterator(*cur), false);
        }
    }

    *cur = __detail::bst_impl<tkey, tvalue, compare, tag>::create_node(*this, prev, value);
    __detail::bst_impl<tkey, tvalue, compare, tag>::post_insert(*this, cur);
    return std::make_pair(infix_iterator(*cur), true);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
std::pair<typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator, bool>
binary_search_tree<tkey, tvalue, compare, tag>::insert(value_type&& value)
{   
    node** cur = &_root;
    node* prev = nullptr;

    while (*cur != nullptr) {
        if (compare_keys((*cur)->data.first, value.first)) {
            prev = *cur;
            cur = &((*cur)->right_subtree);
        } else if (compare_keys(value.first, (*cur)->data.first)) {
            prev = *cur;
            cur = &((*cur)->left_subtree);
        } else {
            return std::make_pair(infix_iterator(*cur), false);
        }
    }

    *cur = __detail::bst_impl<tkey, tvalue, compare, tag>::create_node(*this, prev, value);
    __detail::bst_impl<tkey, tvalue, compare, tag>::post_insert(*this, cur);
    return std::make_pair(infix_iterator(*cur), true);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<std::input_iterator InputIt>
void binary_search_tree<tkey, tvalue, compare, tag>::insert(InputIt first, InputIt last)
{
    for (; first != last; ++first) {
        insert(*first);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<std::ranges::input_range R>
void binary_search_tree<tkey, tvalue, compare, tag>::insert_range(R&& rg)
{
    for (auto &&p : rg) {
        insert(p);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<class ...Args>
std::pair<typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator, bool>
binary_search_tree<tkey, tvalue, compare, tag>::emplace(Args&&... args)
{
    node** cur = &_root;
    node* prev = nullptr;

    value_type value (std::forward<Args>(args) ...);
    while (*cur != nullptr) {
        if (compare_keys((*cur)->data.first, value.first)) {
            prev = *cur;
            cur = &((*cur)->right_subtree);
        } else if (compare_keys(value.first, (*cur)->data.first)) {
            prev = *cur;
            cur = &((*cur)->left_subtree);
        } else {
            return std::make_pair(infix_iterator(*cur), false);
        }
    }

    *cur = __detail::bst_impl<tkey, tvalue, compare, tag>::create_node(*this, prev, value);
    __detail::bst_impl<tkey, tvalue, compare, tag>::post_insert(*this, cur);
    return std::make_pair(infix_iterator(*cur), true);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::insert_or_assign(const value_type& value)
{
    node** cur = &_root;
    node* prev = nullptr;

    while (*cur != nullptr) {
        if (compare_keys((*cur)->data.first, value.first)) {
            prev = *cur;
            cur = &((*cur)->right_subtree);
        } else if (compare_keys(value.first, (*cur)->data.first)) {
            prev = *cur;
            cur = &((*cur)->left_subtree);
        } else {
            (*cur)->data.second = value.second;
            infix_iterator(*cur);
        }
    }

    *cur = __detail::bst_impl<tkey, tvalue, compare, tag>::create_node(*this, prev, value);
    __detail::bst_impl<tkey, tvalue, compare, tag>::post_insert(*this, cur);
    return infix_iterator(*cur);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::insert_or_assign(value_type&& value)
{
    node** cur = &_root;
    node* prev = nullptr;

    while (*cur != nullptr) {
        if (compare_keys((*cur)->data.first, value.first)) {
            prev = *cur;
            cur = &((*cur)->right_subtree);
        } else if (compare_keys(value.first, (*cur)->data.first)) {
            prev = *cur;
            cur = &((*cur)->left_subtree);
        } else {
            (*cur)->data = value;
            return infix_iterator(*cur);
        }
    }

    *cur = __detail::bst_impl<tkey, tvalue, compare, tag>::create_node(*this, prev, value);
    __detail::bst_impl<tkey, tvalue, compare, tag>::post_insert(*this, cur);
    return infix_iterator(*cur);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<std::input_iterator InputIt>
void binary_search_tree<tkey, tvalue, compare, tag>::insert_or_assign(InputIt first, InputIt last)
{
    for (; first != last; ++first) {
        insert_or_assign(*first);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<class ...Args>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::emplace_or_assign(Args&&... args)
{
    node** cur = &_root;
    node* prev = nullptr;

    value_type value (std::forward<Args>(args) ...);
    while (*cur != nullptr) {
        if (compare_keys((*cur)->data.first, value.first)) {
            prev = *cur;
            cur = &((*cur)->right_subtree);
        } else if (compare_keys(value.first, (*cur)->data.first)) {
            prev = *cur;
            cur = &((*cur)->left_subtree);
        } else {
            (*cur)->data.second = value.second;
            return infix_iterator(*cur);
        }
    }

    *cur = __detail::bst_impl<tkey, tvalue, compare, tag>::create_node(*this, prev, value);
    __detail::bst_impl<tkey, tvalue, compare, tag>::post_insert(*this, cur);
    return infix_iterator(*cur);
}

// endregion binary_search_tree methods_insert and methods_emplace implementation

// region binary_search_tree swap_method implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::swap(binary_search_tree& other) noexcept
{
    __detail::bst_impl<tkey, tvalue, compare, tag>::swap(*this, other);
}

// endregion binary_search_tree swap_method implementation

// region binary_search_tree methods_search and methods_erase implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::contains(const tkey& key) const
{
    node** cur = &_root;
    bool res = false;
    while ((*cur) != nullptr) {
        if (compare_keys((*cur)->data.first, key)) {
            cur = &((*cur)->right_subtree);
        } else if (compare_keys(key, (*cur)->data.first)) {
            cur = &((*cur)->left_subtree);
        } else {
            res = true;
            break;
        }
    }
    
    if (res) __detail::bst_impl<tkey, tvalue, compare, tag>::post_search(cur);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::find(const tkey& key)
{

    node** cur = &_root;
    while ((*cur) != nullptr) {
        if (compare_keys((*cur)->data.first, key)) {
            cur = &((*cur)->right_subtree);
        } else if (compare_keys(key, (*cur)->data.first)) {
            cur = &((*cur)->left_subtree);
        } else {
            break;
        }
    }
    if (*cur != nullptr) __detail::bst_impl<tkey, tvalue, compare, tag>::post_search(cur);
    return infix_iterator(*cur);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::find(const tkey& key) const
{
    node** cur = &_root;
    while ((*cur) != nullptr) {
        if (compare_keys(cur->data.first, key)) {
            cur = &((*cur)->right_subtree);
        } else if (compare_keys(key, cur->data.first)) {
            cur = &((*cur)->left_subtree);
        } else {
            break;
        }
    }
    __detail::bst_impl<tkey, tvalue, compare, tag>::post_search(cur);
    return infix_iterator(*cur);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::lower_bound(const tkey& key)
{
    node** cur = &_root;
    node* prev = nullptr;
    bool contain = false;
    bool is_left = false;
    while ((*cur) != nullptr) {
        if (compare_keys((*cur)->data.first, key)) {
            prev = *cur;
            cur = &((*cur)->right_subtree);
            is_left = false;
        } else if (compare_keys(key, (*cur)->data.first)) {
            prev = *cur;
            cur = &((*cur)->left_subtree);
            is_left = true;
        } else {
            contain = true;
            break;
        }
    }
    
    if (contain) __detail::bst_impl<tkey, tvalue, compare, tag>::post_search(cur);


    if (contain) {
        return infix_iterator(*cur);
    } else {
        auto res = infix_iterator(prev);
        if (is_left) {
            --res;
        }
        return res;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::lower_bound(const tkey& key) const
{
    node** cur = &_root;
    node* prev = nullptr;
    bool contain = false;
    bool is_left = false;
    while ((*cur) != nullptr) {
        if (compare_keys((*cur)->data.first, key)) {
            prev = *cur;
            cur = &((*cur)->right_subtree);
            is_left = false;
        } else if (compare_keys(key, (*cur)->data.first)) {
            prev = *cur;
            cur = &((*cur)->left_subtree);
            is_left = true;
        } else {
            contain = true;
            break;
        }
    }
    
    if (contain) __detail::bst_impl<tkey, tvalue, compare, tag>::post_search(cur);


    if (contain) {
        return infix_const_iterator(*cur);
    } else {
        auto res = infix_const_iterator(prev);
        if (is_left) {
            --res;
        }
        return res;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::upper_bound(const tkey& key)
{
    node** cur = &_root;
    node* prev = nullptr;
    bool contain = false;
    bool is_right = false;
    while ((*cur) != nullptr) {
        if (compare_keys((*cur)->data.first, key)) {
            prev = *cur;
            cur = &((*cur)->right_subtree);
            is_right = true;
        } else if (compare_keys(key, (*cur)->data.first)) {
            prev = *cur;
            cur = &((*cur)->left_subtree);
            is_right = false;
        } else {
            contain = true;
            break;
        }
    }
    
    if (contain) __detail::bst_impl<tkey, tvalue, compare, tag>::post_search(cur);


    if (contain) {
        return infix_iterator(*cur);
    } else {
        auto res = infix_iterator(prev);
        if (is_right) {
            ++res;
        }
        return res;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::upper_bound(const tkey& key) const
{
    node** cur = &_root;
    node* prev = nullptr;
    bool contain = false;
    bool is_right = false;
    while ((*cur) != nullptr) {
        if (compare_keys((*cur)->data.first, key)) {
            prev = *cur;
            cur = &((*cur)->right_subtree);
            is_right = true;
        } else if (compare_keys(key, (*cur)->data.first)) {
            prev = *cur;
            cur = &((*cur)->left_subtree);
            is_right = false;
        } else {
            contain = true;
            break;
        }
    }
    
    if (contain) __detail::bst_impl<tkey, tvalue, compare, tag>::post_search(cur);


    if (contain) {
        return infix_const_iterator(*cur);
    } else {
        auto res = infix_const_iterator(prev);
        if (is_right) {
            ++res;
        }
        return res;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::erase(infix_iterator pos)
{
    node* to_del = pos._data;
    ++pos;

    if (to_del == nullptr) {
        return pos;
    }

    node** link = nullptr;
    node* target = nullptr;

    if (to_del->parent == nullptr) {
        link = &_root;
        target = _root;
    } else if (to_del->parent->left_subtree == to_del) {
        link = &(to_del->parent->left_subtree);
        target = to_del->parent;
    } else {
        link = &(to_del->parent->right_subtree);
        target = to_del->parent;
    }

    if ((to_del->left_subtree == nullptr) && (to_del->right_subtree == nullptr)) {
        *link = nullptr;
    } 
    else if ((to_del->left_subtree == nullptr) && (to_del->right_subtree != nullptr)) {

        *link = to_del->right_subtree;
        to_del->right_subtree->parent = to_del->parent;
    } 
    else if ((to_del->left_subtree != nullptr) && (to_del->right_subtree == nullptr)) {

        *link = to_del->left_subtree;
        to_del->left_subtree->parent = to_del->parent;
    }
    else {

        node* to_swap = to_del->left_subtree;

        if (to_swap->right_subtree == nullptr) {
            target = to_swap;
            to_swap->parent = to_del->parent;
            *link = to_swap;
            to_swap->right_subtree = to_del->right_subtree;
            to_del->right_subtree->parent = to_swap;
        } else {

            while (to_swap->right_subtree != nullptr) {
                to_swap = to_swap->right_subtree;
            }
    
            target = to_swap->parent;
            to_swap->parent->right_subtree = to_swap->left_subtree;
            if (to_swap->left_subtree) {
                to_swap->left_subtree->parent = to_swap->parent;
            }
    
            to_swap->left_subtree = to_del->left_subtree;
            to_swap->right_subtree = to_del->right_subtree;
            to_swap->parent = to_del->parent;
    
            *link = to_swap;
    
            to_swap->left_subtree->parent = to_swap;
            to_swap->right_subtree->parent = to_swap;
        }
    }
    
    __detail::bst_impl<tkey, tvalue, compare, tag>::erase(*this, &target);
    __detail::bst_impl<tkey, tvalue, compare, tag>::delete_node(*this, to_del);
    return pos;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::erase(infix_const_iterator pos)
{
    return erase(infix_iterator(const_cast<node*>(pos._base._data)));
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::erase(infix_iterator first, infix_iterator last)
{
    for (;first != last; ++first) {
        erase(first);
    }
    return last;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::erase(infix_const_iterator first, infix_const_iterator last)
{
    return erase(
        infix_iterator(const_cast<node*>(first._base._data)),
        infix_iterator(const_cast<node*>(last._base._data))
    );

}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::erase(const tkey& key)
{
    infix_iterator to_del = find(key);
    if (to_del._data == nullptr) {
        return 0;
    }
    erase(to_del);
    return 1;
}

// endregion binary_search_tree methods_search and methods_erase implementation

// region infix_iterators requests implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin() noexcept
{
    return begin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end() noexcept
{
    return end_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin() const noexcept
{
    return begin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end() const noexcept
{
    return end_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cbegin() const noexcept
{
    return cbegin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cend() const noexcept
{
    return cend_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin() noexcept
{
    return rbegin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend() noexcept
{
    return rend_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin() const noexcept
{
    return rbegin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend() const noexcept
{
    return rend_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crbegin() const noexcept
{
    return crbegin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crend() const noexcept
{
    return crend_infix();
}

// endregion infix_iterators requests implementation

// region prefix_iterators requests implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin_prefix() noexcept
{
    return prefix_iterator(_root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end_prefix() noexcept
{
    if(!_root) {
        return prefix_iterator(nullptr);
    }
    node* cur = _root;
    while ((cur->left_subtree != nullptr) || (cur->right_subtree != nullptr)) {
        if (cur->right_subtree != nullptr) {
            cur = cur->right_subtree;
        } else {
            cur = cur->left_subtree;
        }
    }
    return prefix_iterator(cur);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin_prefix() const noexcept
{
    return prefix_const_iterator(_root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end_prefix() const noexcept
{
    if(!_root) {
        return prefix_const_iterator(nullptr);
    }
    node* cur = _root;
    while ((cur->left_subtree != nullptr) || (cur->right_subtree != nullptr)) {
        if (cur->right_subtree != nullptr) {
            cur = cur->right_subtree;
        } else {
            cur = cur->left_subtree;
        }
    }
    return prefix_const_iterator(cur);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cbegin_prefix() const noexcept
{
    return prefix_const_iterator(_root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cend_prefix() const noexcept
{
    if(!_root) {
        return prefix_const_iterator(nullptr);
    }
    node* cur = _root;
    while ((cur->left_subtree != nullptr) || (cur->right_subtree != nullptr)) {
        if (cur->right_subtree != nullptr) {
            cur = cur->right_subtree;
        } else {
            cur = cur->left_subtree;
        }
    }
    return prefix_const_iterator(cur);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin_prefix() noexcept
{
    return prefix_reverse_iterator(end_prefix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend_prefix() noexcept
{
    return prefix_reverse_iterator(begin_prefix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin_prefix() const noexcept
{
    return prefix_const_reverse_iterator(end_prefix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend_prefix() const noexcept
{
    return prefix_const_reverse_iterator(begin_prefix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crbegin_prefix() const noexcept
{
    return prefix_const_reverse_iterator(cend_prefix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crend_prefix() const noexcept
{
    return prefix_const_reverse_iterator(cbegin_prefix());
}

// endregion prefix_iterators requests implementation

// region infix_iterators methods implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin_infix() noexcept
{
    if(!_root) {
        return infix_iterator(nullptr);
    }
    node* cur = _root;
    while (cur->left_subtree != nullptr) {
        cur = cur->left_subtree;
    }
    return infix_iterator(cur);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end_infix() noexcept
{
    if(!_root) {
        return infix_iterator(nullptr);
    }
    node* cur = _root;
    while (cur->right_subtree != nullptr) {
        cur = cur->right_subtree;
    }
    infix_iterator temp (cur);
    return ++temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin_infix() const noexcept
{
    if(!_root) {
        return infix_const_iterator(nullptr);
    }
    node* cur = _root;
    while (cur->left_subtree != nullptr) {
        cur = cur->left_subtree;
    }
    return infix_iterator(cur);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end_infix() const noexcept
{
    if(!_root) {
        return infix_const_iterator(nullptr);
    }
    node* cur = _root;
    while (cur->right_subtree != nullptr) {
        cur = cur->right_subtree;
    }
    infix_const_iterator temp (cur);
    return ++temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cbegin_infix() const noexcept
{
    if(!_root) {
        return infix_const_iterator(nullptr);
    }
    node* cur = _root;
    while (cur->left_subtree != nullptr) {
        cur = cur->left_subtree;
    }
    return infix_const_iterator(cur);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cend_infix() const noexcept
{
    if(!_root) {
        return infix_const_iterator(nullptr);
    }
    node* cur = _root;
    while (cur->right_subtree != nullptr) {
        cur = cur->right_subtree;
    }
    infix_const_iterator temp (cur);
    return ++temp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin_infix() noexcept
{
    return infix_reverse_iterator(end_infix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend_infix() noexcept
{
    return infix_reverse_iterator(begin_infix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin_infix() const noexcept
{
    return infix_const_reverse_iterator(end_infix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend_infix() const noexcept
{
    return infix_const_reverse_iterator(begin_infix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crbegin_infix() const noexcept
{
    return infix_const_reverse_iterator(cend_infix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crend_infix() const noexcept
{
    return infix_const_reverse_iterator(cbegin_infix());
}

// endregion infix_iterators methods implementation

// region postfix_iterators requests implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin_postfix() noexcept
{
    if(!_root) {
        return postfix_iterator(nullptr);
    }
    node* cur = _root;
    while ((cur->left_subtree != nullptr) || (cur->right_subtree != nullptr)) {
        if (cur->left_subtree != nullptr) {
            cur = cur->left_subtree;
        } else {
            cur = cur->right_subtree;
        }
    }
    return postfix_iterator(cur);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end_postfix() noexcept
{
    return postfix_iterator(_root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin_postfix() const noexcept
{
    if(!_root) {
        return postfix_const_iterator(nullptr);
    }
    node* cur = _root;
    while ((cur->left_subtree != nullptr) || (cur->right_subtree != nullptr)) {
        if (cur->left_subtree != nullptr) {
            cur = cur->left_subtree;
        } else {
            cur = cur->right_subtree;
        }
    }
    return postfix_const_iterator(cur);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end_postfix() const noexcept
{
    return postfix_const_iterator(_root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cbegin_postfix() const noexcept
{
    if(!_root) {
        return postfix_const_iterator(nullptr);
    }
    node* cur = _root;
    while ((cur->left_subtree != nullptr) || (cur->right_subtree != nullptr)) {
        if (cur->left_subtree != nullptr) {
            cur = cur->left_subtree;
        } else {
            cur = cur->right_subtree;
        }
    }
    return postfix_const_iterator(cur);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cend_postfix() const noexcept
{
    return postfix_const_iterator(_root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin_postfix() noexcept
{
    return postfix_reverse_iterator(end_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend_postfix() noexcept
{
    return postfix_reverse_iterator(begin_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin_postfix() const noexcept
{
    return postfix_const_reverse_iterator(end_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend_postfix() const noexcept
{
    return postfix_const_reverse_iterator(begin_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crbegin_postfix() const noexcept
{
    return postfix_const_reverse_iterator(cend_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crend_postfix() const noexcept
{
    return postfix_const_reverse_iterator(cbegin_postfix());
}

// endregion postfix_iterators requests implementation

//region subtree rotations implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::small_left_rotation(node *&subtree_root) noexcept
{
    if ((subtree_root == nullptr) || (subtree_root->right_subtree == nullptr)) {
        return;
    }

    node* tmp_root = subtree_root->right_subtree;

    subtree_root->right_subtree = tmp_root->left_subtree;

    if (tmp_root->left_subtree) {
        tmp_root->left_subtree->parent = subtree_root;
    }

    tmp_root->parent = subtree_root->parent;

    subtree_root->parent = tmp_root;
    tmp_root->left_subtree = subtree_root;
    subtree_root = tmp_root;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::small_right_rotation(node *&subtree_root) noexcept
{
    if ((subtree_root == nullptr) || (subtree_root->left_subtree == nullptr)) {
        return;
    }

    node* tmp_root = subtree_root->left_subtree;

    subtree_root->left_subtree = tmp_root->right_subtree;

    if (tmp_root->right_subtree) {
    tmp_root->right_subtree->parent = subtree_root;
    }

    tmp_root->parent = subtree_root->parent;

    subtree_root->parent = tmp_root;
    tmp_root->right_subtree = subtree_root;

    subtree_root = tmp_root;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::big_left_rotation(node *&subtree_root) noexcept
{
    if ((subtree_root == nullptr) || (subtree_root->right_subtree == nullptr) || (subtree_root->right_subtree->left_subtree == nullptr)) {
        return;
    }

    small_right_rotation(subtree_root->right_subtree);
    small_left_rotation(subtree_root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::big_right_rotation(node *&subtree_root) noexcept
{
    if ((subtree_root == nullptr) || (subtree_root->left_subtree == nullptr) || (subtree_root->left_subtree->right_subtree == nullptr)) {
        return;
    }

    small_left_rotation(subtree_root->left_subtree);
    small_right_rotation(subtree_root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::double_left_rotation(node *&subtree_root) noexcept
{
    if ((subtree_root == nullptr) || (subtree_root->right_subtree == nullptr) || (subtree_root->right_subtree->right_subtree == nullptr)) {
        return;
    }

    small_left_rotation(subtree_root);
    small_left_rotation(subtree_root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::double_right_rotation(node *&subtree_root) noexcept
{
    if ((subtree_root == nullptr) || (subtree_root->left_subtree == nullptr) || (subtree_root->left_subtree->left_subtree == nullptr)) {
        return;
    }

    small_right_rotation(subtree_root);
    small_right_rotation(subtree_root);
}

//endregion subtree rotations implementation

namespace __detail {
    template<typename tkey, typename tvalue, typename compare, typename tag>
    template<typename ...Args>
    typename binary_search_tree<tkey, tvalue, compare, tag>::node*
    bst_impl<tkey, tvalue, compare, tag>::create_node(binary_search_tree<tkey, tvalue, compare, tag>& cont, Args&& ...args)
    {
        using node_t = typename binary_search_tree<tkey,tvalue,compare,tag>::node;
        auto* n = cont._allocator.template new_object<node_t>(std::forward<Args>(args) ...);

        ++cont._size;
        return n;
    }

    template<typename tkey, typename tvalue, typename compare, typename tag>
    void bst_impl<tkey, tvalue, compare, tag>::delete_node(binary_search_tree<tkey, tvalue, compare, tag>& cont, typename binary_search_tree<tkey, tvalue, compare, tag>::node* node)
    {
        cont._allocator.delete_object(node);
       --cont._size;
    }

    template<typename tkey, typename tvalue, typename compare, typename tag>
    void bst_impl<tkey, tvalue, compare, tag>::erase(binary_search_tree<tkey, tvalue, compare, tag>& cont, typename binary_search_tree<tkey, tvalue, compare, tag>::node** node_ptr)
    {}
}

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_BINARY_SEARCH_TREE_H