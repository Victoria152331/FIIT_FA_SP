//
// Created by Des Caldnd on 2/28/2025.
//

#ifndef B_TREE_DISK_HPP
#define B_TREE_DISK_HPP

#include <iterator>
#include <utility>
#include <vector>
#include <concepts>
#include <stack>
#include <fstream>
#include <optional>
#include <cstddef>
#include <filesystem>

template<typename compare, typename tkey>
concept compator = requires(const compare c, const tkey& lhs, const tkey& rhs)
{
    {c(lhs, rhs)} -> std::same_as<bool>;
} && std::copyable<compare> && std::default_initializable<compare>;

template<typename f_iter, typename tkey, typename tval>
concept input_iterator_for_pair = std::input_iterator<f_iter> && std::same_as<typename std::iterator_traits<f_iter>::value_type, std::pair<tkey, tval>>;

template<typename T>
concept serializable = requires (const T t, std::fstream& s)
{
    {t.serialize(s)};
    {T::deserialize(s)} -> std::same_as<T>;
    {t.serialize_size()} -> std::same_as<size_t>;
} && std::copyable<T>;


template <serializable tkey, serializable tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 2>
class B_tree_disk final : private compare
{
public:

    using tree_data_type = std::pair<tkey, tvalue>;
    using tree_data_type_const = std::pair<tkey, tvalue>;

private:

    static constexpr const size_t min_keys = t - 1;
    static constexpr const size_t max_keys = 2 * t - 1;
    static constexpr const size_t max_ptrs = 2 * t;

    static constexpr size_t header_size = sizeof(size_t); // node_counter

    static constexpr size_t record_size = sizeof(size_t)                // size
                                      + sizeof(bool)                  // _is_leaf
                                      + max_ptrs * sizeof(size_t)     // pointers
                                      + max_keys * sizeof(size_t);    // offset


    // region comparators declaration

    inline bool compare_keys(const tkey& lhs, const tkey& rhs) const;

    inline bool compare_pairs(const tree_data_type& lhs, const tree_data_type& rhs) const;

    // endregion comparators declaration

public:

    struct btree_disk_node
    {
        size_t size; // кол-во заполненных ячеек
        bool _is_leaf;
        size_t position_in_disk;
        std::vector<tree_data_type> keys;
        std::vector<size_t> pointers;
        void serialize(std::fstream& stream, std::fstream& stream_for_data) const;

        static btree_disk_node deserialize(std::fstream& stream, std::fstream& stream_for_data);

        explicit btree_disk_node(bool is_leaf);
        btree_disk_node();
    };

private:

    friend btree_disk_node;

    std::fstream _file_for_tree;

    std::fstream _file_for_key_value;

    btree_disk_node _current_node;

public:

    size_t _count_of_node; //только растет

    // region constructors declaration

    explicit B_tree_disk(const std::string& file_path, const compare& cmp = compare());

    // endregion constructors declaration

    // region five declaration

    B_tree_disk(B_tree_disk&& other) noexcept =default;
    B_tree_disk& operator=(B_tree_disk&& other) noexcept =default;

    B_tree_disk(const B_tree_disk& other) =delete;
    B_tree_disk& operator=(const B_tree_disk& other) =delete;

    ~B_tree_disk() noexcept = default;

    // endregion five declaration

    // region iterators declaration

    class btree_disk_const_iterator
    {
        std::stack<std::pair<size_t , size_t>> _path;
        size_t _index;
        B_tree_disk<tkey,tvalue, compare, t>& _tree;
    public:
        using value_type = tree_data_type_const;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = ptrdiff_t;

        using self = btree_disk_const_iterator;

        friend class B_tree_disk;

        value_type operator*() noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(self& other) noexcept;
        bool operator!=(self& other) noexcept;

        explicit btree_disk_const_iterator(B_tree_disk<tkey, tvalue, compare, t>& tree, const std::stack<std::pair<size_t, size_t>>& path = std::stack<std::pair<size_t, size_t>>(), size_t index = 0);

    };

    friend class btree_disk_const_iterator;

    std::optional<tvalue> at(const tkey&);//либо пустое, либо tvalue//std::nullopt

    btree_disk_const_iterator begin();
    btree_disk_const_iterator end() ;

    //std::vector<tree_data_type_const> find_range(const tkey& lower, const tkey& upper) const;

    std::pair<btree_disk_const_iterator, btree_disk_const_iterator> find_range(const tkey& lower, const tkey& upper, bool include_lower = true, bool include_upper = false);

    /*
     * Does nothing if key exists
     * Second return value is true, when inserted
     */
    bool insert(const tree_data_type& data);

    /*
     * Updates value if key exists
     */
    bool update(const tree_data_type& data);

    /*
     * Return true if deleted
     */
    bool erase(const tkey& key);

    bool is_valid() const noexcept;


    std::pair<std::stack<std::pair<size_t, size_t>>, std::pair<size_t, bool>>  find_path(const tkey& key);

public:

    btree_disk_node disk_read(size_t position);

    void check_tree(size_t pos, size_t depth);

    void disk_write(btree_disk_node& node);

private:

    std::pair<size_t, bool> find_index(const tkey &key, btree_disk_node& node) const noexcept;

    void insert_array(btree_disk_node& node, size_t right_node, const tree_data_type& data, size_t index) noexcept;

    void split_node(std::stack<std::pair<size_t, size_t>>& path);

    btree_disk_node remove_array(btree_disk_node& node, size_t index, bool remove_left_ptr = true) noexcept;

    void rebalance_node(std::stack<std::pair<size_t, size_t>> &path, btree_disk_node& node,size_t &index);

};

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::is_valid() const noexcept
{

}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::erase(const tkey& key)
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::rebalance_node(std::stack<std::pair<size_t, size_t>> &path, btree_disk_node& node, size_t &index)
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node B_tree_disk<tkey, tvalue, compare, t>::remove_array(btree_disk_node& node, size_t index, bool remove_left_ptr) noexcept
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::update(const B_tree_disk::tree_data_type &data)
{

}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::insert(const B_tree_disk::tree_data_type &data)
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::split_node(std::stack<std::pair<size_t, size_t>>& path)
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::insert_array(btree_disk_node& node, size_t right_node, const tree_data_type& data, size_t index) noexcept
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
std::pair<std::stack<std::pair<size_t, size_t>>, std::pair<size_t,bool>>  B_tree_disk<tkey, tvalue, compare, t>::find_path(const tkey& key)
{
    std::stack<std::pair<size_t, size_t>> path;
    constexpr size_t root_pos = 0;
    path.push({root_pos, 0});

    btree_disk_node node = disk_read(root_pos);

    while (true) {
        auto bs_res = find_index(key, node);
        
        if (bs_res.second) { // node contains key
            return {path, {bs_res.first, true}};
        }

        if (node._is_leaf) { // return pointer for insert
            return {path, {bs_res.first, false}};
        }

        size_t child_pos = node.pointers[bs_res.first];
        path.push({child_pos, bs_res.first});
        node = disk_read(child_pos);
    }
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
std::pair<size_t, bool> B_tree_disk<tkey, tvalue, compare, t>::find_index(const tkey &key, btree_disk_node& node) const noexcept
{
    /*
        returns:
        if contains: <key index, true>
        else: <pointer index, false>
    */
    size_t k = node.size;
    size_t low = 0;
    size_t high = k + 1;
    size_t mid;
    while (low + 1 < high) {
        mid = low + (high - low) / 2;
        if (compare_keys(key, node.keys[mid - 1].first;)) {
            high = mid;
        } else if (compare_keys(node.keys[mid - 1].first;, key)) {
            low = mid;
        } else {
            return {mid - 1, true};
        }
    }
    return {low, false};
}

// read write

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node::serialize(std::fstream &tree_stream, std::fstream& data_stream) const
{
    tree_stream.write(reinterpret_cast<const char*>(&size), sizeof(size));
    tree_stream.write(reinterpret_cast<const char*>(&_is_leaf), sizeof(_is_leaf));


    for (size_t i = 0; i < max_ptrs; ++i) {
        size_t pos = (i < pointers.size() ? pointers[i] : 0);
        tree_stream.write(reinterpret_cast<const char*>(&pos), sizeof(pos));
    }


    for (size_t i = 0; i < max_keys; ++i) {
        size_t data_off = 0;
        if (i < size) {
            data_off = static_cast<size_t>(data_stream.tellp());
            keys[i].first.serialize(data_stream);
            keys[i].second.serialize(data_stream);
        }
        tree_stream.write(reinterpret_cast<const char*>(&data_off), sizeof(data_off));
    }
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::disk_write(btree_disk_node& node)
{
    size_t offset = header_size + record_size * node.position_in_disk;
    _file_for_tree.seekp(static_cast<std::streamoff>(offset));
    node.serialize(_file_for_tree, _file_for_key_value);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node::deserialize(std::fstream &tree_stream, std::fstream& data_stream)
{
    btree_disk_node node;

    tree_stream.read(reinterpret_cast<char*>(&node.size), sizeof(node.size));
    tree_stream.read(reinterpret_cast<char*>(&node._is_leaf), sizeof(node._is_leaf));

    node.pointers.resize(node.size + 1);
    for (size_t i = 0; i < max_ptrs; ++i) {
        size_t pos;
        tree_stream.read(reinterpret_cast<char*>(&pos), sizeof(pos));
        if (i < node.size + 1)
            node.pointers[i] = pos;
    }

    node.keys.resize(node.size);
    size_t off;
    for (size_t i = 0; i < max_keys; ++i) {
        tree_stream.read(reinterpret_cast<char*>(&off), sizeof(size_t));
        
        if (i < node.size) {
            data_stream.seekg(static_cast<std::streamoff>(off));
            node.keys[i].first  = tkey::deserialize(data_stream);
            node.keys[i].second = tvalue::deserialize(data_stream);
        }
    }

    return node;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node B_tree_disk<tkey, tvalue, compare, t>::disk_read(size_t node_position)
{
    size_t offset = header_size + record_size * node_position;
    _file_for_tree.seekg(static_cast<std::streamoff>(offset));
    btree_disk_node node = btree_disk_node::deserialize(_file_for_tree, _file_for_key_value);
    return node;
}

// node implementation

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node::btree_disk_node(bool is_leaf) : _is_leaf(is_leaf), size(0), position_in_disk(_count_of_node)
{
    pointers.clear();
    keys.clear();
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node::btree_disk_node() : _is_leaf(true), size(0), position_in_disk(_count_of_node)
{
    pointers.clear();
    keys.clear();
}

// compare

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::compare_pairs(const B_tree_disk::tree_data_type &lhs,
                                                          const B_tree_disk::tree_data_type &rhs) const
{
    return compare_keys(lhs.first, rhs.first);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::compare_keys(const tkey &lhs, const tkey &rhs) const
{
    return compare::operator()(lhs, rhs);
}

// constructor

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::B_tree_disk(const std::string& file_path, const compare& cmp):  compare(cmp)
{
    std::string tree_name = file_path + "/tree";
    std::string data_name = file_path + "/data";
    
    _file_for_tree.open(tree_name, std::ios::in | std::ios::out | std::ios::binary);
    _file_for_key_value.open(data_name, std::ios::in | std::ios::out | std::ios::binary);

    if (!_file_for_tree.is_open() || !_file_for_key_value.is_open()) {
        
        if (_file_for_tree.is_open()) _file_for_tree.close();
        if (_file_for_key_value.is_open()) _file_for_key_value.close();

        _file_for_tree.open(tree_name, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);
        _file_for_key_value.open(data_name, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);

        _count_of_node = 0;
        btree_disk_node root;
        _count_of_node++;

        // header
        _file_for_tree.seekp(0);
        _file_for_tree.write(reinterpret_cast<const char*>(&_count_of_node), sizeof(_count_of_node));

        root.serialize(_file_for_tree, _file_for_key_value);

        _file_for_key_value.seekg(0);

    } else {
        // Load header
        _file_for_tree.seekg(0);
        _file_for_tree.read(reinterpret_cast<char*>(&_count_of_node), sizeof(_count_of_node));
        _file_for_key_value.seekg(0);
    }
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::check_tree(size_t pos, size_t depth)
{

}

// iterator

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::btree_disk_const_iterator(B_tree_disk<tkey, tvalue, compare, t>& tree, const std::stack<std::pair<size_t, size_t>> &path, size_t index) : _tree(tree), _path(path), _index(index) {}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator B_tree_disk<tkey, tvalue, compare, t>::begin()
{


}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator B_tree_disk<tkey, tvalue, compare, t>::end()
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self& B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator++()
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator++(int)
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self& B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator--()
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator--(int)
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator==(B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self &other) noexcept
{
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator!=(B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self &other) noexcept
{
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::value_type B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator*() noexcept
{
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
std::optional<tvalue> B_tree_disk<tkey, tvalue, compare, t>::at(const tkey & key)
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator, typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator>
B_tree_disk<tkey, tvalue, compare, t>::find_range(const tkey &lower, const tkey &upper, bool include_lower, bool include_upper)
{

}

#endif //B_TREE_DISK_HPP
