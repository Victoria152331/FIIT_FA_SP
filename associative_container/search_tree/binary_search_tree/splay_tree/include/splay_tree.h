#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_SPLAY_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_SPLAY_TREE_H

#include <binary_search_tree.h>

namespace __detail
{
    class SPL_TAG;

    template<typename tkey, typename tvalue, typename compare>
    class bst_impl<tkey, tvalue, compare, SPL_TAG>
    {
        friend class binary_search_tree<tkey, tvalue, compare, SPL_TAG>;
        template<class ...Args>
        static binary_search_tree<tkey, tvalue, compare, SPL_TAG>::node* create_node(binary_search_tree<tkey, tvalue, compare, SPL_TAG>& cont, Args&& ...args)
        {
            using node_t = typename binary_search_tree<tkey,tvalue,compare,SPL_TAG>::node;
            auto* n = cont._allocator.template new_object<node_t>(std::forward<Args>(args) ...);

            ++cont._size;
            return n;
        }

        static void delete_node(binary_search_tree<tkey, tvalue, compare, SPL_TAG>& cont, binary_search_tree<tkey, tvalue, compare, SPL_TAG>::node* node)
        {
            cont._allocator.delete_object(node);
            --cont._size;
        }

        //Does not invalidate node*, needed for splay tree
        static void post_search(binary_search_tree<tkey, tvalue, compare, SPL_TAG>& cont, binary_search_tree<tkey, tvalue, compare, SPL_TAG>::node **node) {
            using BST = binary_search_tree<tkey, tvalue, compare, SPL_TAG>;
            typename BST::node* target = *node;
            typename BST::node** link;
            std::cout << "uauau\n";
            while (target->parent != nullptr) {
                auto pr = target->parent;
                auto gr = pr->parent;
                if (gr != nullptr) {
                    if (gr->parent == nullptr) {
                        link = &(cont._root);
                    } else if (gr->parent->left_subtree == gr) {
                        link = &(gr->parent->left_subtree);
                    } else {
                        link = &(gr->parent->right_subtree);
                    }
                }
        
                if (gr == nullptr && target == pr->left_subtree)
                {
                    BST::small_right_rotation(cont._root);
                }
                else if (gr == nullptr && target == pr->right_subtree)
                {
                    BST::small_left_rotation(cont._root);
                }
                else if (pr == gr->left_subtree && target == pr->left_subtree)
                {
                    BST::double_right_rotation(*link);
                }
                else if (pr == gr->right_subtree && target == pr->right_subtree)
                {
                    BST::double_left_rotation(*link);
                }
                else if (pr == gr->left_subtree && target == pr->right_subtree)
                {
                    BST::big_right_rotation(*link);
                }
                else
                {
                    BST::big_left_rotation(*link);
                }
            }
        }

        //Does not invalidate node*
        static void post_insert(binary_search_tree<tkey, tvalue, compare, SPL_TAG>& cont, binary_search_tree<tkey, tvalue, compare, SPL_TAG>::node** node) {}

        static void erase(binary_search_tree<tkey, tvalue, compare, SPL_TAG>& cont, binary_search_tree<tkey, tvalue, compare, SPL_TAG>::node** link, int ind)
        {
            using node_BST = typename binary_search_tree<tkey, tvalue, compare, SPL_TAG>::node;
            post_search(cont, link);
            node_BST* to_del = cont._root;
            
            if ((to_del->left_subtree == nullptr) && (to_del->right_subtree == nullptr)) {
                cont._root = nullptr;
            } 
            else if ((to_del->left_subtree == nullptr) && (to_del->right_subtree != nullptr)) {

                cont._root = to_del->right_subtree;
                to_del->right_subtree->parent = nullptr;
            } 
            else {
                node_BST* to_swap = to_del->left_subtree;
                to_swap->parent = nullptr;
                while (to_swap->right_subtree != nullptr) {
                    to_swap = to_swap->right_subtree;
                }
                post_search(cont, &to_swap);
                to_swap->right_subtree = to_del->right_subtree;
                to_del->right_subtree->parent = to_swap;
                cont._root = to_swap;
            }
            __detail::bst_impl<tkey, tvalue, compare, SPL_TAG>::delete_node(cont, to_del);

        }

        static void swap(binary_search_tree<tkey, tvalue, compare, SPL_TAG>& lhs, binary_search_tree<tkey, tvalue, compare, SPL_TAG>& rhs) noexcept
        {
            std::swap(lhs._root, rhs._root);
            std::swap(lhs._size, rhs._size);
            std::swap(lhs._logger, rhs._logger);
            std::swap(lhs._allocator, rhs._allocator);
            std::swap(static_cast<compare&>(lhs), static_cast<compare&>(rhs));
        }
    };
}

template<typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>>
class splay_tree final: public binary_search_tree<tkey, tvalue, compare, __detail::SPL_TAG>
{
    friend class __detail::bst_impl<tkey, tvalue, compare, __detail::SPL_TAG>;
    using parent = binary_search_tree<tkey, tvalue, compare, __detail::SPL_TAG>;
public:

    using value_type = parent::value_type;

    explicit splay_tree(
            const compare& comp = compare(),
            pp_allocator<value_type> alloc = pp_allocator<value_type>(),
            logger *log = nullptr);

    explicit splay_tree(
            pp_allocator<value_type> alloc,
            const compare& comp = compare(),
            logger *log = nullptr);

    template<input_iterator_for_pair<tkey, tvalue> iterator>
    explicit splay_tree(iterator begin, iterator end, const compare& cmp = compare(),
            pp_allocator<value_type> alloc = pp_allocator<value_type>(),
            logger* log = nullptr);

    template<std::ranges::input_range Range>
    explicit splay_tree(Range&& range, const compare& cmp = compare(),
            pp_allocator<value_type> alloc = pp_allocator<value_type>(),
            logger* log = nullptr);


    splay_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(),
            pp_allocator<value_type> alloc = pp_allocator<value_type>(),
            logger* log = nullptr);

public:
    
    ~splay_tree() noexcept final = default;
    
    splay_tree(splay_tree const &other);
    
    splay_tree &operator=(splay_tree const &other);
    
    splay_tree(splay_tree &&other) noexcept =default;
    
    splay_tree &operator=(splay_tree &&other) noexcept =default ;

};

template<typename compare, typename U, typename iterator>
explicit splay_tree(iterator begin, iterator end, const compare& cmp = compare(),
        pp_allocator<U> alloc = pp_allocator<U>(),
        logger* log = nullptr) -> splay_tree<const typename std::iterator_traits<iterator>::value_type::first_type, typename std::iterator_traits<iterator>::value_type::second_type, compare>;

template<typename compare, typename U, std::ranges::forward_range Range>
explicit splay_tree(Range&& range, const compare& cmp = compare(),
        pp_allocator<U> alloc = pp_allocator<U>(),
        logger* log = nullptr) -> splay_tree<const typename std::iterator_traits<typename std::ranges::iterator_t<Range>>::value_type::first_type, typename std::iterator_traits<typename std::ranges::iterator_t<Range>>::value_type::second_type, compare> ;

template<typename tkey, typename tvalue, typename compare, typename U>
splay_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(),
        pp_allocator<U> alloc = pp_allocator<U>(),
        logger* log = nullptr) -> splay_tree<tkey, tvalue, compare>;

// region implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
splay_tree<tkey, tvalue, compare>::splay_tree(
        const compare& comp,
        pp_allocator<value_type> alloc,
        logger *log)
    : parent(comp, alloc, log)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
splay_tree<tkey, tvalue, compare>::splay_tree(
        pp_allocator<value_type> alloc,
        const compare& comp,
        logger *log)
    : parent(alloc, comp, log)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
template<input_iterator_for_pair<tkey, tvalue> iterator>
splay_tree<tkey, tvalue, compare>::splay_tree(
        iterator begin,
        iterator end,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* log)
    : parent(begin, end, cmp, alloc, log)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
template<std::ranges::input_range Range>
splay_tree<tkey, tvalue, compare>::splay_tree(
        Range&& range,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* log)
    : parent(range, cmp, alloc, log)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
splay_tree<tkey, tvalue, compare>::splay_tree(
        std::initializer_list<std::pair<tkey, tvalue>> data,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* log)
    : parent(data, cmp, alloc, log)
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
splay_tree<tkey, tvalue, compare>::splay_tree(splay_tree const &other)
    :parent(static_cast<parent>(other))
{}

template<typename tkey, typename tvalue, compator<tkey> compare>
splay_tree<tkey, tvalue, compare> &splay_tree<tkey, tvalue, compare>::operator=(splay_tree const &other)
{
    parent::operator=(static_cast<parent>(other));
    return *this;
}


// endregion implementation

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_SPLAY_TREE_H