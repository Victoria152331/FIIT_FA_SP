#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_AVL_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_AVL_TREE_H

#include <binary_search_tree.h>

namespace __detail
{
    class AVL_TAG;

    template<typename tkey, typename tvalue, typename compare>
    class bst_impl<tkey, tvalue, compare, AVL_TAG>
    {
        friend class binary_search_tree<tkey, tvalue, compare, AVL_TAG>;

        template<class ...Args>
        static binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node* create_node(binary_search_tree<tkey, tvalue, compare, AVL_TAG>& cont, Args&& ...args);

        static void delete_node(binary_search_tree<tkey, tvalue, compare, AVL_TAG>& cont, binary_search_tree <tkey, tvalue, compare, AVL_TAG>::node *node);

        //Does not invalidate node*, needed for splay tree
        static void post_search(binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node**){}

        //Does not invalidate node*
        static void post_insert(binary_search_tree<tkey, tvalue, compare, AVL_TAG>& cont, binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node**);

        static void erase(binary_search_tree<tkey, tvalue, compare, AVL_TAG>& cont, binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node**, int ind);

        static void swap(binary_search_tree<tkey, tvalue, compare, AVL_TAG>& lhs, binary_search_tree<tkey, tvalue, compare, AVL_TAG>& rhs) noexcept;
    };
}

template<typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>>
class AVL_tree final:
    public binary_search_tree<tkey, tvalue, compare, __detail::AVL_TAG>
{
    using parent = binary_search_tree<tkey, tvalue, compare, __detail::AVL_TAG>;
    friend class __detail::bst_impl<tkey, tvalue, compare, __detail::AVL_TAG>;
private:
    
    struct node final: public parent::node
    {
        size_t height;

        void recalculate_height() noexcept;

        /*
         * Returns positive if right subtree is bigger
         */
        short get_balance() const noexcept;

        template<class ...Args>
        node(parent::node* par, Args&&... args);

        ~node() noexcept override =default;
    };

public:

    using value_type = parent::value_type;

    // region iterator definition

    class prefix_iterator : public parent::prefix_iterator
    {
    public:

        using value_type = parent::prefix_iterator::value_type;
        using difference_type = parent::prefix_iterator::difference_type;
        using pointer = parent::prefix_iterator::pointer;
        using reference = parent::prefix_iterator::reference;
        using iterator_category = parent::prefix_iterator::iterator_category;

        explicit prefix_iterator(parent::node* n = nullptr) noexcept;
        prefix_iterator(parent::prefix_iterator) noexcept;

        size_t get_height() const noexcept;
        size_t get_balance() const noexcept;

        using parent::prefix_iterator::depth;
        using parent::prefix_iterator::operator*;
        using parent::prefix_iterator::operator==;
        using parent::prefix_iterator::operator!=;
        using parent::prefix_iterator::operator++;
        using parent::prefix_iterator::operator--;
        using parent::prefix_iterator::operator->;
    };

    class prefix_const_iterator : public parent::prefix_const_iterator
    {
    public:

        using value_type = parent::prefix_const_iterator::value_type;
        using difference_type = parent::prefix_const_iterator::difference_type;
        using pointer = parent::prefix_const_iterator::pointer;
        using reference = parent::prefix_const_iterator::reference;
        using iterator_category = parent::prefix_const_iterator::iterator_category;

        explicit prefix_const_iterator(parent::node* n = nullptr) noexcept;
        prefix_const_iterator(parent::prefix_const_iterator) noexcept;

        size_t get_height() const noexcept;
        size_t get_balance() const noexcept;

        prefix_const_iterator(prefix_iterator) noexcept;

        using parent::prefix_const_iterator::depth;
        using parent::prefix_const_iterator::operator*;
        using parent::prefix_const_iterator::operator==;
        using parent::prefix_const_iterator::operator!=;
        using parent::prefix_const_iterator::operator++;
        using parent::prefix_const_iterator::operator--;
        using parent::prefix_const_iterator::operator->;
    };

    class prefix_reverse_iterator : public parent::prefix_reverse_iterator
    {
    public:

        using value_type = parent::prefix_reverse_iterator::value_type;
        using difference_type = parent::prefix_reverse_iterator::difference_type;
        using pointer = parent::prefix_reverse_iterator::pointer;
        using reference = parent::prefix_reverse_iterator::reference;
        using iterator_category = parent::prefix_reverse_iterator::iterator_category;

        explicit prefix_reverse_iterator(parent::node* n = nullptr) noexcept;
        prefix_reverse_iterator(parent::prefix_reverse_iterator) noexcept;

        size_t get_height() const noexcept;
        size_t get_balance() const noexcept;

        prefix_reverse_iterator(prefix_iterator) noexcept;
        operator prefix_iterator() const noexcept;
        prefix_iterator base() const noexcept;

        using parent::prefix_reverse_iterator::depth;
        using parent::prefix_reverse_iterator::operator*;
        using parent::prefix_reverse_iterator::operator==;
        using parent::prefix_reverse_iterator::operator!=;
        using parent::prefix_reverse_iterator::operator++;
        using parent::prefix_reverse_iterator::operator--;
        using parent::prefix_reverse_iterator::operator->;
    };

    class prefix_const_reverse_iterator : public parent::prefix_const_reverse_iterator
    {
    public:

        using value_type = parent::prefix_const_reverse_iterator::value_type;
        using difference_type = parent::prefix_const_reverse_iterator::difference_type;
        using pointer = parent::prefix_const_reverse_iterator::pointer;
        using reference = parent::prefix_const_reverse_iterator::reference;
        using iterator_category = parent::prefix_const_reverse_iterator::iterator_category;

        explicit prefix_const_reverse_iterator(parent::node* n = nullptr) noexcept;
        prefix_const_reverse_iterator(parent::prefix_const_reverse_iterator) noexcept;

        size_t get_height() const noexcept;
        size_t get_balance() const noexcept;

        prefix_const_reverse_iterator(prefix_const_iterator) noexcept;
        operator prefix_const_iterator() const noexcept;
        prefix_const_iterator base() const noexcept;

        using parent::prefix_const_reverse_iterator::depth;
        using parent::prefix_const_reverse_iterator::operator*;
        using parent::prefix_const_reverse_iterator::operator==;
        using parent::prefix_const_reverse_iterator::operator!=;
        using parent::prefix_const_reverse_iterator::operator++;
        using parent::prefix_const_reverse_iterator::operator--;
        using parent::prefix_const_reverse_iterator::operator->;
    };

    class infix_iterator : public parent::infix_iterator
    {
    public:

        using value_type = parent::infix_iterator::value_type;
        using difference_type = parent::infix_iterator::difference_type;
        using pointer = parent::infix_iterator::pointer;
        using reference = parent::infix_iterator::reference;
        using iterator_category = parent::infix_iterator::iterator_category;

        explicit infix_iterator(parent::node* n = nullptr) noexcept;
        infix_iterator(parent::infix_iterator) noexcept;

        size_t get_height() const noexcept;
        size_t get_balance() const noexcept;

        using parent::infix_iterator::depth;
        using parent::infix_iterator::operator*;
        using parent::infix_iterator::operator==;
        using parent::infix_iterator::operator!=;
        using parent::infix_iterator::operator++;
        using parent::infix_iterator::operator--;
        using parent::infix_iterator::operator->;
    };

    class infix_const_iterator : parent::infix_const_iterator
    {
    public:

        using value_type = parent::infix_const_iterator::value_type;
        using difference_type = parent::infix_const_iterator::difference_type;
        using pointer = parent::infix_const_iterator::pointer;
        using reference = parent::infix_const_iterator::reference;
        using iterator_category = parent::infix_const_iterator::iterator_category;

        explicit infix_const_iterator(parent::node* n = nullptr) noexcept;
        infix_const_iterator(parent::infix_const_iterator) noexcept;

        size_t get_height() const noexcept;
        size_t get_balance() const noexcept;

        infix_const_iterator(infix_iterator) noexcept;

        using parent::infix_const_iterator::depth;
        using parent::infix_const_iterator::operator*;
        using parent::infix_const_iterator::operator==;
        using parent::infix_const_iterator::operator!=;
        using parent::infix_const_iterator::operator++;
        using parent::infix_const_iterator::operator--;
        using parent::infix_const_iterator::operator->;
    };

    class infix_reverse_iterator : public parent::infix_reverse_iterator
    {
    public:

        using value_type = parent::infix_reverse_iterator::value_type;
        using difference_type = parent::infix_reverse_iterator::difference_type;
        using pointer = parent::infix_reverse_iterator::pointer;
        using reference = parent::infix_reverse_iterator::reference;
        using iterator_category = parent::infix_reverse_iterator::iterator_category;

        explicit infix_reverse_iterator(parent::node* n = nullptr) noexcept;
        infix_reverse_iterator(parent::infix_reverse_iterator) noexcept;

        size_t get_height() const noexcept;
        size_t get_balance() const noexcept;

        infix_reverse_iterator(infix_iterator) noexcept;
        operator infix_iterator() const noexcept;
        infix_iterator base() const noexcept;

        using parent::infix_reverse_iterator::depth;
        using parent::infix_reverse_iterator::operator*;
        using parent::infix_reverse_iterator::operator==;
        using parent::infix_reverse_iterator::operator!=;
        using parent::infix_reverse_iterator::operator++;
        using parent::infix_reverse_iterator::operator--;
        using parent::infix_reverse_iterator::operator->;
    };

    class infix_const_reverse_iterator : public parent::infix_const_reverse_iterator
    {
    public:

        using value_type = parent::infix_const_reverse_iterator::value_type;
        using difference_type = parent::infix_const_reverse_iterator::difference_type;
        using pointer = parent::infix_const_reverse_iterator::pointer;
        using reference = parent::infix_const_reverse_iterator::reference;
        using iterator_category = parent::infix_const_reverse_iterator::iterator_category;

        explicit infix_const_reverse_iterator(parent::node* n = nullptr) noexcept;
        infix_const_reverse_iterator(parent::infix_const_reverse_iterator) noexcept;

        size_t get_height() const noexcept;
        size_t get_balance() const noexcept;

        infix_const_reverse_iterator(infix_const_iterator) noexcept;
        operator infix_const_iterator() const noexcept;
        infix_const_iterator base() const noexcept;

        using parent::infix_const_reverse_iterator::depth;
        using parent::infix_const_reverse_iterator::operator*;
        using parent::infix_const_reverse_iterator::operator==;
        using parent::infix_const_reverse_iterator::operator!=;
        using parent::infix_const_reverse_iterator::operator++;
        using parent::infix_const_reverse_iterator::operator--;
        using parent::infix_const_reverse_iterator::operator->;
    };

    class postfix_iterator : public parent::postfix_iterator
    {
    public:

        using value_type = parent::postfix_iterator::value_type;
        using difference_type = parent::postfix_iterator::difference_type;
        using pointer = parent::postfix_iterator::pointer;
        using reference = parent::postfix_iterator::reference;
        using iterator_category = parent::postfix_iterator::iterator_category;

        explicit postfix_iterator(parent::node* n = nullptr) noexcept;
        postfix_iterator(parent::postfix_iterator) noexcept;

        size_t get_height() const noexcept;
        size_t get_balance() const noexcept;

        using parent::postfix_iterator::depth;
        using parent::postfix_iterator::operator*;
        using parent::postfix_iterator::operator==;
        using parent::postfix_iterator::operator!=;
        using parent::postfix_iterator::operator++;
        using parent::postfix_iterator::operator--;
        using parent::postfix_iterator::operator->;
    };

    class postfix_const_iterator : public parent::postfix_const_iterator
    {
    public:

        using value_type = parent::postfix_const_iterator::value_type;
        using difference_type = parent::postfix_const_iterator::difference_type;
        using pointer = parent::postfix_const_iterator::pointer;
        using reference = parent::postfix_const_iterator::reference;
        using iterator_category = parent::postfix_const_iterator::iterator_category;

        explicit postfix_const_iterator(parent::node* n = nullptr) noexcept;
        postfix_const_iterator(parent::postfix_const_iterator) noexcept;

        size_t get_height() const noexcept;
        size_t get_balance() const noexcept;

        postfix_const_iterator(postfix_iterator) noexcept;

        using parent::postfix_const_iterator::depth;
        using parent::postfix_const_iterator::operator*;
        using parent::postfix_const_iterator::operator==;
        using parent::postfix_const_iterator::operator!=;
        using parent::postfix_const_iterator::operator++;
        using parent::postfix_const_iterator::operator--;
        using parent::postfix_const_iterator::operator->;
    };

    class postfix_reverse_iterator : public parent::postfix_reverse_iterator
    {
    public:

        using value_type = parent::postfix_reverse_iterator::value_type;
        using difference_type = parent::postfix_reverse_iterator::difference_type;
        using pointer = parent::postfix_reverse_iterator::pointer;
        using reference = parent::postfix_reverse_iterator::reference;
        using iterator_category = parent::postfix_reverse_iterator::iterator_category;

        explicit postfix_reverse_iterator(parent::node* n = nullptr) noexcept;
        postfix_reverse_iterator(parent::postfix_reverse_iterator) noexcept;

        size_t get_height() const noexcept;
        size_t get_balance() const noexcept;

        postfix_reverse_iterator(postfix_iterator) noexcept;
        operator postfix_iterator() const noexcept;
        postfix_iterator base() const noexcept;

        using parent::postfix_reverse_iterator::depth;
        using parent::postfix_reverse_iterator::operator*;
        using parent::postfix_reverse_iterator::operator==;
        using parent::postfix_reverse_iterator::operator!=;
        using parent::postfix_reverse_iterator::operator++;
        using parent::postfix_reverse_iterator::operator--;
        using parent::postfix_reverse_iterator::operator->;
    };

    class postfix_const_reverse_iterator : public parent::postfix_const_reverse_iterator
    {
    public:

        using value_type = parent::postfix_const_reverse_iterator::value_type;
        using difference_type = parent::postfix_const_reverse_iterator::difference_type;
        using pointer = parent::postfix_const_reverse_iterator::pointer;
        using reference = parent::postfix_const_reverse_iterator::reference;
        using iterator_category = parent::postfix_const_reverse_iterator::iterator_category;

        explicit postfix_const_reverse_iterator(parent::node* n = nullptr) noexcept;
        postfix_const_reverse_iterator(parent::postfix_const_reverse_iterator) noexcept;

        size_t get_height() const noexcept;
        size_t get_balance() const noexcept;

        postfix_const_reverse_iterator(postfix_const_iterator) noexcept;
        operator postfix_const_iterator() const noexcept;
        postfix_const_iterator base() const noexcept;

        using parent::postfix_const_reverse_iterator::depth;
        using parent::postfix_const_reverse_iterator::operator*;
        using parent::postfix_const_reverse_iterator::operator==;
        using parent::postfix_const_reverse_iterator::operator!=;
        using parent::postfix_const_reverse_iterator::operator++;
        using parent::postfix_const_reverse_iterator::operator--;
        using parent::postfix_const_reverse_iterator::operator->;

    };

    // endregion iterator definition
    
    // region iterator requests declaration

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
    
    // endregion iterator requests declaration

    explicit AVL_tree(
            const compare& comp = compare(),
            pp_allocator<value_type> alloc = pp_allocator<value_type>(),
            logger *log = nullptr);

    explicit AVL_tree(
            pp_allocator<value_type> alloc,
            const compare& comp = compare(),
            logger *log = nullptr);

    template<input_iterator_for_pair<tkey, tvalue> iterator>
    explicit AVL_tree(iterator begin, iterator end, const compare& cmp = compare(),
                                pp_allocator<value_type> alloc = pp_allocator<value_type>(),
                                logger* log = nullptr);

    template<std::ranges::input_range Range>
    explicit AVL_tree(Range&& range, const compare& cmp = compare(),
            pp_allocator<value_type> alloc = pp_allocator<value_type>(),
            logger* log = nullptr);


    AVL_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(),
            pp_allocator<value_type> alloc = pp_allocator<value_type>(),
            logger* log = nullptr);

public:
    
    ~AVL_tree() noexcept final =default;
    
    AVL_tree(AVL_tree const &other);
    
    AVL_tree &operator=(AVL_tree const &other);
    
    AVL_tree(AVL_tree &&other) noexcept =default;
    
    AVL_tree &operator=(AVL_tree &&other) noexcept =default;

    void swap(parent& other) noexcept override;


    /** Only rebinds iterators
     */
    std::pair<infix_iterator, bool> insert(const value_type&);
    std::pair<infix_iterator, bool> insert(value_type&&);

    template<class ...Args>
    std::pair<infix_iterator, bool> emplace(Args&&...args);

    infix_iterator insert_or_assign(const value_type&);
    infix_iterator insert_or_assign(value_type&&);

    template<class ...Args>
    infix_iterator emplace_or_assign(Args&&...args);

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

    using parent::erase;
    using parent::insert;
    using parent::insert_or_assign;
};

// 3 штуки, благодаря которым можно не писать явно шаблонные параметры, они будут опредляться по аргументам конструктора

template<typename compare, typename U, typename iterator>
explicit AVL_tree(iterator begin, iterator end, const compare& cmp = compare(),
                            pp_allocator<U> alloc = pp_allocator<U>(),
                            logger* log = nullptr) -> AVL_tree<const typename std::iterator_traits<iterator>::value_type::first_type, typename std::iterator_traits<iterator>::value_type::second_type, compare>;

template<typename compare, typename U, std::ranges::forward_range Range>
explicit AVL_tree(Range&& range, const compare& cmp = compare(),
                            pp_allocator<U> alloc = pp_allocator<U>(),
                            logger* log = nullptr) -> AVL_tree<const typename std::iterator_traits<typename std::ranges::iterator_t<Range>>::value_type::first_type, typename std::iterator_traits<typename std::ranges::iterator_t<Range>>::value_type::second_type, compare> ;

template<typename tkey, typename tvalue, typename compare, typename U>
AVL_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(),
                   pp_allocator<U> alloc = pp_allocator<U>(),
                   logger* log = nullptr) -> AVL_tree<tkey, tvalue, compare>;

namespace __detail
{
    template<typename tkey, typename tvalue, typename compare>
    template<class ...Args>
    binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node *bst_impl<tkey, tvalue, compare, AVL_TAG>::create_node(
            binary_search_tree <tkey, tvalue, compare, AVL_TAG> &cont, Args&& ...args)
    {
        using node_t = typename AVL_tree<tkey,tvalue,compare>::node;
        auto* n = cont._allocator.template new_object<node_t>(std::forward<Args>(args) ...);

        ++cont._size;
        return n;
    }

    template<typename tkey, typename tvalue, typename compare>
    void bst_impl<tkey, tvalue, compare, AVL_TAG>::delete_node(
            binary_search_tree <tkey, tvalue, compare, AVL_TAG> &cont, binary_search_tree <tkey, tvalue, compare, AVL_TAG>::node *node)
    {
        using node_t = typename AVL_tree<tkey,tvalue,compare>::node;
        cont._allocator.delete_object(static_cast<node_t*>(node));
       --cont._size;
    }

    template<typename tkey, typename tvalue, typename compare>
    void bst_impl<tkey, tvalue, compare, AVL_TAG>::post_insert(
            binary_search_tree <tkey, tvalue, compare, AVL_TAG> &cont,
            typename binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node **node)
    {
        using node_AVL = typename AVL_tree<tkey,tvalue,compare>::node;
        using node_BST = typename binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node;

        while (*node != nullptr) {
            static_cast<node_AVL*>(*node)->recalculate_height();

            short root_balance = static_cast<node_AVL*>(*node)->get_balance();
            if (root_balance == 2) {
                short right_balance = static_cast<node_AVL*>((*node)->right_subtree)->get_balance();

                node_BST** link = nullptr;
                if ((*node)->parent == nullptr) {
                    link = &(cont._root);
                } else if ((*node)->parent->left_subtree == (*node)) {
                    link = &((*node)->parent->left_subtree);
                } else {
                    link = &((*node)->parent->right_subtree);
                }

                if (right_balance >= 0) {
                    binary_search_tree<tkey, tvalue, compare, AVL_TAG>::small_left_rotation(*link);
                    node = link;
                    static_cast<node_AVL*>((*node)->left_subtree)->recalculate_height();
                    static_cast<node_AVL*>(*node)->recalculate_height();
                } else {
                    binary_search_tree<tkey, tvalue, compare, AVL_TAG>::big_left_rotation(*link);
                    node = link;
                    static_cast<node_AVL*>((*node)->left_subtree)->recalculate_height();
                    static_cast<node_AVL*>((*node)->right_subtree)->recalculate_height();
                    static_cast<node_AVL*>(*node)->recalculate_height();
                }
            } else if (root_balance == -2) {
                short left_balance = static_cast<node_AVL*>((*node)->left_subtree)->get_balance();

                node_BST** link = nullptr;
                if ((*node)->parent == nullptr) {
                    link = &(cont._root);
                } else if ((*node)->parent->left_subtree == (*node)) {
                    link = &((*node)->parent->left_subtree);
                } else {
                    link = &((*node)->parent->right_subtree);
                }

                if (left_balance <= 0) {
                    binary_search_tree<tkey, tvalue, compare, AVL_TAG>::small_right_rotation(*link);
                    node = link;
                    static_cast<node_AVL*>((*node)->right_subtree)->recalculate_height();
                    static_cast<node_AVL*>(*node)->recalculate_height();
                } else {
                    binary_search_tree<tkey, tvalue, compare, AVL_TAG>::big_right_rotation(*link);
                    node = link;
                    static_cast<node_AVL*>((*node)->right_subtree)->recalculate_height();
                    static_cast<node_AVL*>((*node)->left_subtree)->recalculate_height();
                    static_cast<node_AVL*>(*node)->recalculate_height();
                }
            }
            node = &((*node)->parent);
        }

    }

    template<typename tkey, typename tvalue, typename compare>
    void bst_impl<tkey, tvalue, compare, AVL_TAG>::erase(
            binary_search_tree <tkey, tvalue, compare, AVL_TAG> &cont,
            typename binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node **link, int ind)
    {
        using node_BST = typename binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node;
        node_BST* to_del = *link;
        node_BST* target;
        if (to_del->parent == nullptr) {
            target = cont._root;
        } else if (to_del->parent->left_subtree == to_del) {
            target = to_del->parent;
        } else {
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

            node_BST* to_swap = to_del->left_subtree;

            if (to_swap->right_subtree == nullptr) {
                target = to_swap;
                ind = 1;
                to_swap->parent = to_del->parent;
                *link = to_swap;
                to_swap->right_subtree = to_del->right_subtree;
                to_del->right_subtree->parent = to_swap;
                to_del->parent = to_swap;
            } else {

                while (to_swap->right_subtree != nullptr) {
                    to_swap = to_swap->right_subtree;
                }
        
                target = to_swap->parent;
                ind = 2;
                to_swap->parent->right_subtree = to_swap->left_subtree;
                if (to_swap->left_subtree) {
                    to_swap->left_subtree->parent = to_swap->parent;
                }
        
                std::swap(to_swap->left_subtree, to_del->left_subtree);
                std::swap(to_swap->right_subtree, to_del->right_subtree);
                std::swap(to_swap->parent, to_del->parent);
        
                *link = to_swap;
        
                to_swap->left_subtree->parent = to_swap;
                to_swap->right_subtree->parent = to_swap;
            }
        }
        __detail::bst_impl<tkey, tvalue, compare, AVL_TAG>::delete_node(cont, to_del);
        post_insert(cont, &target);
    }
}

template<typename tkey, typename tvalue, typename compare>
void __detail::bst_impl<tkey, tvalue, compare, __detail::AVL_TAG>::swap(binary_search_tree<tkey, tvalue, compare, AVL_TAG> &lhs,
                                                          binary_search_tree<tkey, tvalue, compare, AVL_TAG> &rhs) noexcept
{
    std::swap(lhs._root, rhs._root);
    std::swap(lhs._size, rhs._size);
    std::swap(lhs._logger, rhs._logger);
    std::swap(lhs._allocator, rhs._allocator);
    std::swap(static_cast<compare&>(lhs), static_cast<compare&>(rhs));
}

// + region node implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
void AVL_tree<tkey, tvalue, compare>::node::recalculate_height() noexcept
{
    size_t r = 0, l = 0;
    if (this->right_subtree != nullptr) {
        r = (static_cast<AVL_tree<tkey, tvalue, compare>::node*>(this->right_subtree))->height;
    }
    if (this->left_subtree != nullptr) {
        l = static_cast<AVL_tree<tkey, tvalue, compare>::node*>(this->left_subtree)->height;
    }

    if (l > r) {
        this->height = l + 1;
    } else {
        this->height = r + 1;
    }
    //std::cout << "height for " << this->data.first << " = " << this->height << "(" << l << " " << r << ")" << std::endl;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
short AVL_tree<tkey, tvalue, compare>::node::get_balance() const noexcept
{
    size_t r = 0, l = 0;
    if (this->right_subtree != nullptr) {
        r = (static_cast<AVL_tree<tkey, tvalue, compare>::node*>(this->right_subtree))->height;
    }
    if (this->left_subtree != nullptr) {
        l = static_cast<AVL_tree<tkey, tvalue, compare>::node*>(this->left_subtree)->height;
    }
    return static_cast<short>(r - l);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
template<class ...Args>
AVL_tree<tkey, tvalue, compare>::node::node(parent::node* par, Args&&... args)
    : parent::node(par, std::forward<Args>(args)...)
    , height(1)
{}

// endregion node implementation

// + region prefix_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_iterator::prefix_iterator(parent::node* n) noexcept
    : parent::prefix_iterator(n)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_iterator::prefix_iterator(parent::prefix_iterator it) noexcept
    : parent::prefix_iterator(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::prefix_iterator::get_height() const noexcept
{
    auto n = static_cast<typename AVL_tree<tkey, tvalue, compare>::node*>(this->_data);
    return n->height;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::prefix_iterator::get_balance() const noexcept
{
    auto n = static_cast<typename AVL_tree<tkey, tvalue, compare>::node*>(this->_data);
    return n->get_balance();
}

// endregion prefix_iterator implementation

// + region prefix_const_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_const_iterator::prefix_const_iterator(parent::node* n) noexcept
    : parent::prefix_const_iterator(n)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_const_iterator::prefix_const_iterator(parent::prefix_const_iterator it) noexcept
    : parent::prefix_const_iterator(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_const_iterator::prefix_const_iterator(prefix_iterator it) noexcept
    : parent::prefix_const_iterator(static_cast<parent::prefix_iterator>(it))
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::prefix_const_iterator::get_height() const noexcept
{
    auto n = static_cast<prefix_iterator>(this->_base);
    return n.get_height();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::prefix_const_iterator::get_balance() const noexcept
{
    auto n = static_cast<prefix_iterator>(this->_base);
    return n.get_balance();
}

// endregion prefix_const_iterator implementation

// + region prefix_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_reverse_iterator::prefix_reverse_iterator(parent::node* n) noexcept
    : parent::prefix_reverse_iterator(n)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_reverse_iterator::prefix_reverse_iterator(parent::prefix_reverse_iterator it) noexcept
    : parent::prefix_reverse_iterator(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::prefix_reverse_iterator::get_height() const noexcept
{
    auto n = static_cast<prefix_iterator>(this->_base);
    return n.get_height();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::prefix_reverse_iterator::get_balance() const noexcept
{
    auto n = static_cast<prefix_iterator>(this->_base);
    return n.get_balance();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_reverse_iterator::prefix_reverse_iterator(prefix_iterator it) noexcept
    : parent::prefix_reverse_iterator(static_cast<parent::prefix_iterator>(it))
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_reverse_iterator::operator AVL_tree<tkey, tvalue, compare>::prefix_iterator() const noexcept
{
    return this->_base;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_iterator
AVL_tree<tkey, tvalue, compare>::prefix_reverse_iterator::base() const noexcept
{
    return this->_base;
}

// endregion prefix_reverse_iterator implementation

// + region prefix_const_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(parent::node* n) noexcept
    : parent::prefix_const_reverse_iterator(n)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(parent::prefix_const_reverse_iterator it) noexcept
    : parent::prefix_const_reverse_iterator(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator::get_height() const noexcept
{
    auto n = static_cast<prefix_iterator>(this->_base);
    return n.get_height();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator::get_balance() const noexcept
{
    auto n = static_cast<prefix_iterator>(this->_base);
    return n.get_balance();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(prefix_const_iterator it) noexcept
    : parent::infix_const_reverse_iterator(static_cast<parent::prefix_iterator>(it))
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator::operator AVL_tree<tkey, tvalue, compare>::prefix_const_iterator() const noexcept
{
    return this->_base;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_const_iterator
AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator::base() const noexcept
{
    return this->_base;
}

// endregion prefix_const_reverse_iterator implementation

// + region infix_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_iterator::infix_iterator(parent::node* n) noexcept
    : parent::infix_iterator(n)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_iterator::infix_iterator(parent::infix_iterator it) noexcept
    : parent::infix_iterator(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::infix_iterator::get_height() const noexcept
{
    auto n = static_cast<typename AVL_tree<tkey, tvalue, compare>::node*>(this->_data);
    return n->height;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::infix_iterator::get_balance() const noexcept
{
    auto n = static_cast<typename AVL_tree<tkey, tvalue, compare>::node*>(this->_data);
    return n->get_balance();
}

// endregion infix_iterator implementation

// + region infix_const_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_const_iterator::infix_const_iterator(parent::node* n) noexcept
    : parent::infix_const_iterator(n)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_const_iterator::infix_const_iterator(parent::infix_const_iterator it) noexcept
    : parent::infix_const_iterator(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::infix_const_iterator::get_height() const noexcept
{
    auto n = static_cast<infix_iterator>(this->_base);
    return n.get_height();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::infix_const_iterator::get_balance() const noexcept
{
    auto n = static_cast<infix_iterator>(this->_base);
    return n.get_balance();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_const_iterator::infix_const_iterator(infix_iterator it) noexcept
    : parent::infix_const_iterator(static_cast<parent::infix_iterator>(it))
{}


// endregion infix_const_iterator implementation

// + region infix_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator::infix_reverse_iterator(parent::node* n) noexcept
    : parent::infix_reverse_iterator(n)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator::infix_reverse_iterator(parent::infix_reverse_iterator it) noexcept
    : parent::infix_reverse_iterator(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator::get_height() const noexcept
{
    auto n = static_cast<infix_iterator>(this->_base);
    return n.get_height();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator::get_balance() const noexcept
{
    auto n = static_cast<infix_iterator>(this->_base);
    return n.get_balance();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator::infix_reverse_iterator(infix_iterator it) noexcept
    : parent::infix_reverse_iterator(static_cast<parent::infix_iterator>(it))
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator::operator AVL_tree<tkey, tvalue, compare>::infix_iterator() const noexcept
{
    return this->_base;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_iterator AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator::base() const noexcept
{
    return this->_base;
}

// endregion infix_reverse_iterator implementation

// + region infix_const_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator::infix_const_reverse_iterator(parent::node* n) noexcept
    : parent::infix_const_reverse_iterator(n)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator::infix_const_reverse_iterator(parent::infix_const_reverse_iterator it) noexcept
    : parent::infix_const_reverse_iterator(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator::get_height() const noexcept
{
    auto n = static_cast<infix_iterator>(this->_base);
    return n.get_height();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator::get_balance() const noexcept
{
    auto n = static_cast<infix_iterator>(this->_base);
    return n.get_balance();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator::infix_const_reverse_iterator(infix_const_iterator it) noexcept
    : parent::infix_const_reverse_iterator(static_cast<parent::infix_iterator>(it))
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator::operator AVL_tree<tkey, tvalue, compare>::infix_const_iterator() const noexcept
{
    return this->_base;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator::base() const noexcept
{
    return this->_base;
}

// endregion infix_const_reverse_iterator implementation

// + region postfix_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_iterator::postfix_iterator(parent::node* n) noexcept
    : parent::postfix_iterator(n)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_iterator::postfix_iterator(parent::postfix_iterator it) noexcept
    : parent::postfix_iterator(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::postfix_iterator::get_height() const noexcept
{
    auto n = static_cast<typename AVL_tree<tkey, tvalue, compare>::node*>(this->_data);
    return n->height;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::postfix_iterator::get_balance() const noexcept
{
    auto n = static_cast<typename AVL_tree<tkey, tvalue, compare>::node*>(this->_data);
    return n->get_balance();
}

// endregion postfix_iterator implementation

// + region postfix_const_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_const_iterator::postfix_const_iterator(parent::node* n) noexcept
    : parent::postfix_const_iterator(n)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_const_iterator::postfix_const_iterator(parent::postfix_const_iterator it) noexcept
    : parent::postfix_const_iterator(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::postfix_const_iterator::get_height() const noexcept
{
    auto n = static_cast<postfix_iterator>(this->_base);
    return n.get_height();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::postfix_const_iterator::get_balance() const noexcept
{
    auto n = static_cast<postfix_iterator>(this->_base);
    return n.get_balance();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_const_iterator::postfix_const_iterator(postfix_iterator it) noexcept
    : parent::postfix_const_iterator(static_cast<parent::postfix_iterator>(it))
{}

// endregion postfix_const_iterator implementation

// + region postfix_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_reverse_iterator::postfix_reverse_iterator(parent::node* n) noexcept
    : parent::postfix_reverse_iterator(n)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_reverse_iterator::postfix_reverse_iterator(parent::postfix_reverse_iterator it) noexcept
    : parent::postfix_reverse_iterator(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::postfix_reverse_iterator::get_height() const noexcept
{
    auto n = static_cast<postfix_iterator>(this->_base);
    return n.get_height();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::postfix_reverse_iterator::get_balance() const noexcept
{
    auto n = static_cast<postfix_iterator>(this->_base);
    return n.get_balance();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_reverse_iterator::postfix_reverse_iterator(postfix_iterator it) noexcept
    : parent::postfix_reverse_iterator(static_cast<parent::postfix_iterator>(it))
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_reverse_iterator::operator AVL_tree<tkey, tvalue, compare>::postfix_iterator() const noexcept
{
    return this->_base;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_iterator AVL_tree<tkey, tvalue, compare>::postfix_reverse_iterator::base() const noexcept
{
    return this->_base;
}

// endregion postfix_reverse_iterator implementation

// + region postfix_const_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(parent::node* n) noexcept
    : parent::postfix_const_reverse_iterator(n)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(parent::postfix_const_reverse_iterator it) noexcept
    : parent::postfix_const_reverse_iterator(it)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator::get_height() const noexcept
{
    auto n = static_cast<postfix_iterator>(this->_base);
    return n.get_height();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator::get_balance() const noexcept
{
    auto n = static_cast<postfix_iterator>(this->_base);
    return n.get_balance();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(postfix_const_iterator it) noexcept
    : parent::postfix_const_reverse_iterator(static_cast<parent::postfix_iterator>(it))
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator::operator AVL_tree<tkey, tvalue, compare>::postfix_const_iterator() const noexcept
{
    return this->_base;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_const_iterator AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator::base() const noexcept
{
    return this->_base;
}

// endregion postfix_const_reverse_iterator implementation

// + region iterator requests implementation

// + region Infix iterators
template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_iterator AVL_tree<tkey, tvalue, compare>::begin() noexcept
{
    return infix_iterator(parent::begin());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_iterator AVL_tree<tkey, tvalue, compare>::end() noexcept
{
    return infix_iterator(parent::end());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator AVL_tree<tkey, tvalue, compare>::begin() const noexcept
{
    return infix_const_iterator(parent::begin());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator AVL_tree<tkey, tvalue, compare>::end() const noexcept
{
    return infix_const_iterator(parent::end());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator AVL_tree<tkey, tvalue, compare>::cbegin() const noexcept
{
    return infix_const_iterator(parent::cbegin());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator AVL_tree<tkey, tvalue, compare>::cend() const noexcept
{
    return infix_const_iterator(parent::cend());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator AVL_tree<tkey, tvalue, compare>::rbegin() noexcept
{
    return infix_reverse_iterator(parent::rbegin());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator AVL_tree<tkey, tvalue, compare>::rend() noexcept
{
    return infix_reverse_iterator(parent::rend());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator AVL_tree<tkey, tvalue, compare>::rbegin() const noexcept
{
    return infix_const_reverse_iterator(parent::rbegin());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator AVL_tree<tkey, tvalue, compare>::rend() const noexcept
{
    return infix_const_reverse_iterator(parent::rend());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator AVL_tree<tkey, tvalue, compare>::crbegin() const noexcept
{
    return infix_const_reverse_iterator(parent::crbegin());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator AVL_tree<tkey, tvalue, compare>::crend() const noexcept
{
    return infix_const_reverse_iterator(parent::crend());
}

// + region prefix iterators

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_iterator AVL_tree<tkey, tvalue, compare>::begin_prefix() noexcept
{
    return prefix_iterator(parent::begin_prefix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_iterator AVL_tree<tkey, tvalue, compare>::end_prefix() noexcept
{
    return prefix_iterator(parent::end_prefix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_const_iterator AVL_tree<tkey, tvalue, compare>::begin_prefix() const noexcept
{
    return prefix_const_iterator(parent::begin_prefix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_const_iterator AVL_tree<tkey, tvalue, compare>::end_prefix() const noexcept
{
    return prefix_const_iterator(parent::end_prefix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_const_iterator AVL_tree<tkey, tvalue, compare>::cbegin_prefix() const noexcept
{
    return prefix_const_iterator(parent::cbegin_prefix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_const_iterator AVL_tree<tkey, tvalue, compare>::cend_prefix() const noexcept
{
    return prefix_const_iterator(parent::cend_prefix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_reverse_iterator AVL_tree<tkey, tvalue, compare>::rbegin_prefix() noexcept
{
    return prefix_reverse_iterator(parent::rbegin_prefix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_reverse_iterator AVL_tree<tkey, tvalue, compare>::rend_prefix() noexcept
{
    return prefix_reverse_iterator(parent::rend_prefix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator AVL_tree<tkey, tvalue, compare>::rbegin_prefix() const noexcept
{
    return prefix_const_reverse_iterator(parent::rbegin_prefix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator AVL_tree<tkey, tvalue, compare>::rend_prefix() const noexcept
{
    return prefix_const_reverse_iterator(parent::rend_prefix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator AVL_tree<tkey, tvalue, compare>::crbegin_prefix() const noexcept
{
    return prefix_const_reverse_iterator(parent::crbegin_prefix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator AVL_tree<tkey, tvalue, compare>::crend_prefix() const noexcept
{
    return prefix_const_reverse_iterator(parent::crend_prefix());
}

// + region infix iterators
template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_iterator AVL_tree<tkey, tvalue, compare>::begin_infix() noexcept
{
    return infix_iterator(parent::begin_infix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_iterator AVL_tree<tkey, tvalue, compare>::end_infix() noexcept
{
    return infix_iterator(parent::end_infix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator AVL_tree<tkey, tvalue, compare>::begin_infix() const noexcept
{
    return infix_const_iterator(parent::begin_infix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator AVL_tree<tkey, tvalue, compare>::end_infix() const noexcept
{
    return infix_const_iterator(parent::end_infix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator AVL_tree<tkey, tvalue, compare>::cbegin_infix() const noexcept
{
    return infix_const_iterator(parent::cbegin_infix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator AVL_tree<tkey, tvalue, compare>::cend_infix() const noexcept
{
    return infix_const_iterator(parent::cend_infix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator AVL_tree<tkey, tvalue, compare>::rbegin_infix() noexcept
{
    return infix_reverse_iterator(parent::rbegin_infix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator AVL_tree<tkey, tvalue, compare>::rend_infix() noexcept
{
    return infix_reverse_iterator(parent::rend_infix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator AVL_tree<tkey, tvalue, compare>::rbegin_infix() const noexcept
{
    return infix_const_reverse_iterator(parent::rbegin_infix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator AVL_tree<tkey, tvalue, compare>::rend_infix() const noexcept
{
    return infix_const_reverse_iterator(parent::rend_infix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator AVL_tree<tkey, tvalue, compare>::crbegin_infix() const noexcept
{
    return infix_const_reverse_iterator(parent::crbegin_infix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator AVL_tree<tkey, tvalue, compare>::crend_infix() const noexcept
{
    return infix_const_reverse_iterator(parent::crend_infix());
}

// + region postfix iterators
template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_iterator AVL_tree<tkey, tvalue, compare>::begin_postfix() noexcept
{
    return postfix_iterator(parent::begin_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_iterator AVL_tree<tkey, tvalue, compare>::end_postfix() noexcept
{
    return postfix_iterator(parent::end_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_const_iterator AVL_tree<tkey, tvalue, compare>::begin_postfix() const noexcept
{
    return postfix_const_iterator(parent::begin_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_const_iterator AVL_tree<tkey, tvalue, compare>::end_postfix() const noexcept
{
    return postfix_const_iterator(parent::end_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_const_iterator AVL_tree<tkey, tvalue, compare>::cbegin_postfix() const noexcept
{
    return postfix_const_iterator(parent::cbegin_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_const_iterator AVL_tree<tkey, tvalue, compare>::cend_postfix() const noexcept
{
    return postfix_const_iterator(parent::cend_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_reverse_iterator AVL_tree<tkey, tvalue, compare>::rbegin_postfix() noexcept
{
    return postfix_reverse_iterator(parent::rbegin_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_reverse_iterator AVL_tree<tkey, tvalue, compare>::rend_postfix() noexcept
{
    return postfix_reverse_iterator(parent::rend_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator AVL_tree<tkey, tvalue, compare>::rbegin_postfix() const noexcept
{
    return postfix_const_reverse_iterator(parent::rbegin_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator AVL_tree<tkey, tvalue, compare>::rend_postfix() const noexcept
{
    return postfix_const_reverse_iterator(parent::rend_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator AVL_tree<tkey, tvalue, compare>::crbegin_postfix() const noexcept
{
    return postfix_const_reverse_iterator(parent::crbegin_postfix());
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator AVL_tree<tkey, tvalue, compare>::crend_postfix() const noexcept
{
    return postfix_const_reverse_iterator(parent::crend_postfix());
}

// endregion iterator requests implementation

// + region AVL_tree constructors

// Constructors
template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::AVL_tree(
        const compare& comp,
        pp_allocator<value_type> alloc,
        logger* log)
    : parent(comp, alloc, log)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::AVL_tree(
        pp_allocator<value_type> alloc,
        const compare& comp,
        logger* log)
    : parent(alloc, comp, log)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
template<input_iterator_for_pair<tkey, tvalue> iterator>
AVL_tree<tkey, tvalue, compare>::AVL_tree(
        iterator begin, iterator end,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* log)
    : parent(begin, end, cmp, alloc, log)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
template<std::ranges::input_range Range>
AVL_tree<tkey, tvalue, compare>::AVL_tree(
        Range&& range,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* log)
    : parent(range, cmp, alloc, log)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::AVL_tree(std::initializer_list<std::pair<tkey, tvalue>> data,
                                          const compare& cmp, pp_allocator<value_type> alloc,
                                          logger* log)
    : parent(data, cmp, alloc, log)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::AVL_tree(const AVL_tree& other)
    :parent(static_cast<parent>(other))
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>& AVL_tree<tkey, tvalue, compare>::operator=(const AVL_tree& other)
{
    parent::operator=(static_cast<parent>(other));
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
void AVL_tree<tkey, tvalue, compare>::swap(parent& other) noexcept
{
    parent::swap(other);
}

// endregion AVL_tree constructors

// + region AVL_tree methods

template<typename tkey, typename tvalue, compator<tkey> compare>
std::pair<typename AVL_tree<tkey, tvalue, compare>::infix_iterator, bool>
AVL_tree<tkey, tvalue, compare>::insert(const value_type& value)
{
    auto [it, ok] = parent::insert(value);
    return std::make_pair(infix_iterator(it), ok);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
std::pair<typename AVL_tree<tkey, tvalue, compare>::infix_iterator, bool>
AVL_tree<tkey, tvalue, compare>::insert(value_type&& value)
{
    auto [it, ok] = parent::insert(value);
    return std::make_pair(infix_iterator(it), ok);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
template<class ...Args>
std::pair<typename AVL_tree<tkey, tvalue, compare>::infix_iterator, bool>
AVL_tree<tkey, tvalue, compare>::emplace(Args&&... args)
{
    auto [it, ok] = parent::emplace(std::forward<Args>(args) ...);
    return std::make_pair(infix_iterator(it), ok);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_iterator
AVL_tree<tkey, tvalue, compare>::insert_or_assign(const value_type& value)
{
    auto it = parent::insert_or_assign(value);
    return infix_iterator(it);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_iterator
AVL_tree<tkey, tvalue, compare>::insert_or_assign(value_type&& value)
{
    auto it = parent::insert_or_assign(value);
    return infix_iterator(it);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
template<class ...Args>
typename AVL_tree<tkey, tvalue, compare>::infix_iterator
AVL_tree<tkey, tvalue, compare>::emplace_or_assign(Args&&... args)
{
    auto it = parent::emplace_or_assign(std::forward<Args>(args) ...);
    return infix_iterator(it);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_iterator
AVL_tree<tkey, tvalue, compare>::find(const tkey& key)
{
    auto it = parent::find(key);
    return infix_iterator(it);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator
AVL_tree<tkey, tvalue, compare>::find(const tkey& key) const
{
    auto it = parent::find(key);
    return infix_const_iterator(it);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_iterator
AVL_tree<tkey, tvalue, compare>::lower_bound(const tkey& key)
{
    auto it = parent::lower_bound(key);
    return infix_iterator(it);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator
AVL_tree<tkey, tvalue, compare>::lower_bound(const tkey& key) const
{
    auto it = parent::lower_bound(key);
    return infix_const_iterator(it);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_iterator
AVL_tree<tkey, tvalue, compare>::upper_bound(const tkey& key)
{
    auto it = parent::upper_bound(key);
    return infix_iterator(it);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator
AVL_tree<tkey, tvalue, compare>::upper_bound(const tkey& key) const
{
    auto it = parent::upper_bound(key);
    return infix_const_iterator(it);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_iterator
AVL_tree<tkey, tvalue, compare>::erase(infix_iterator pos)
{
    auto it = parent::erase(static_cast<parent::infix_iterator>(pos));
    return infix_iterator(it);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_iterator
AVL_tree<tkey, tvalue, compare>::erase(infix_const_iterator pos)
{
    auto it = parent::erase(static_cast<parent::infix_const_iterator>(pos));
    return infix_iterator(it);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_iterator
AVL_tree<tkey, tvalue, compare>::erase(infix_iterator first, infix_iterator last)
{
    auto it = parent::erase(static_cast<parent::infix_iterator>(first), static_cast<parent::infix_iterator>(last));
    return infix_iterator(it);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_iterator
AVL_tree<tkey, tvalue, compare>::erase(infix_const_iterator first, infix_const_iterator last)
{
    auto it = parent::erase(static_cast<parent::infix_const_iterator>(first), static_cast<parent::infix_const_iterator>(last));
    return infix_iterator(it);
}

// endregion AVL_tree methods

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_AVL_TREE_H