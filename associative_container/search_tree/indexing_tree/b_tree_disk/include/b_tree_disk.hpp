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

#include <iostream>

#define ROOT 1

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

    std::pair<std::stack<std::pair<size_t, size_t>>, std::pair<size_t, bool>>  find_path(const tkey& key);

public:

    btree_disk_node disk_read(size_t position);

    void disk_write(btree_disk_node& node);

private:

    std::pair<size_t, bool> find_index(const tkey &key, btree_disk_node& node) const noexcept;

    void split_node(std::stack<std::pair<size_t, size_t>>& path, btree_disk_node& node);

    void rebalance_node(std::stack<std::pair<size_t, size_t>> &path, btree_disk_node& node);

};

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::erase(const tkey& key)
{
    auto [path, bs_res] = find_path(key);
    auto [index, contain] = bs_res;
    if (!contain) {
        return false;
    }
    btree_disk_node node = disk_read(path.top().first);
    node.keys.erase(node.keys.begin() + index);
    node.size--;

    while ((node.size < min_keys)) {
        if (node.position_in_disk == ROOT) {
            if (node.size == 0) {
                btree_disk_node new_root = disk_read(node.pointers[0]);
                new_root.position_in_disk = 0;
                node = new_root;
            }
            break;
        }
        rebalance_node(path, node);
    }

    disk_write(node);
    return true;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::rebalance_node(std::stack<std::pair<size_t, size_t>> &path, btree_disk_node& node)
{
    
    size_t index = path.top().second;
    path.pop();

    btree_disk_node parent = disk_read(path.top().first);
    size_t k_cur = node.size;
    btree_disk_node left_bro, right_bro;
    size_t k_left = 0, k_right = 0;
    if (index != 0) {
        left_bro = disk_read(parent.pointers[index - 1]);
        k_left = left_bro.size;
    }
    if (parent.size > index) {
        right_bro = disk_read(parent.pointers[index + 1]);
        k_right = right_bro.size;
    }


    if (k_left > min_keys) {
        node.keys.insert(node.keys.begin(), parent.keys[index - 1]);
        node.pointers.insert(node.pointers.begin(), left_bro.pointers[k_left]);
        node.size++;

        parent.keys[index - 1] = left_bro.keys[k_left - 1];

        left_bro.keys.pop_back();
        left_bro.pointers.pop_back();
        left_bro.size--;
        disk_write(node);
        disk_write(left_bro);

    } else if (k_right > min_keys) {
        node.keys.push_back(parent.keys[index]);
        node.pointers.push_back(right_bro.pointers[0]);
        node.size++;

        parent.keys[index] = right_bro.keys[0];

        right_bro.keys.erase(right_bro.keys.begin());
        right_bro.pointers.erase(right_bro.pointers.begin());
        right_bro.size--;
        disk_write(node);
        disk_write(right_bro);

    } else if (index != 0) {
        left_bro.keys.push_back(parent.keys[index - 1]);
        left_bro.keys.insert(left_bro.keys.end(), node.keys.begin(), node.keys.end());
        left_bro.pointers.insert(left_bro.pointers.end(), node.pointers.begin(), node.pointers.end());

        parent.keys.erase(parent.keys.begin() + index - 1);
        parent.pointers.erase(parent.pointers.begin() + index);
        left_bro.size = left_bro.keys.size();
        parent.size--;
        disk_write(left_bro);

    } else { // index == 0
        node.keys.push_back(parent.keys[0]);
        node.keys.insert(node.keys.end(), right_bro.keys.begin(), right_bro.keys.end());
        node.pointers.insert(node.pointers.end(), right_bro.pointers.begin(), right_bro.pointers.end());

        parent.keys.erase(parent.keys.begin());
        parent.pointers.erase(parent.pointers.begin() + 1);
        node.size = node.keys.size();
        parent.size--;
        disk_write(node);
    }
    node = parent;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::update(const B_tree_disk::tree_data_type &data)
{
    auto [path, bs_res] = find_path(data.first);
    auto [index, contain] = bs_res;
    if (!contain) {
        return false;
    }
    btree_disk_node node = disk_read(path.top().first);
    node.keys[index] = data;
    disk_write(node);
    return true;
}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::insert(const B_tree_disk::tree_data_type &data)
{
    auto [path, bs_res] = find_path(data.first);
    auto [index, contain] = bs_res;
    if (contain) {
        return false;
    }
    btree_disk_node node = disk_read(path.top().first);
    node.keys.insert(node.keys.begin() + index, data);
    node.size++;

    while (node.size > max_keys) {
        split_node(path, node);
    }

    disk_write(node);
    return true;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::split_node(std::stack<std::pair<size_t, size_t>>& path, btree_disk_node& node)
{
    size_t split = (max_keys + 1) / 2;
    size_t index = path.top().second;
    tree_data_type cur_data = node.keys[split];
    path.pop();

    btree_disk_node left_part, right_part;

    right_part.keys.clear();
    right_part.pointers.clear();

    left_part = node;
    right_part._is_leaf = node._is_leaf;

    if (node.position_in_disk == ROOT) {
        left_part.position_in_disk = _count_of_node;
        _count_of_node++;
        node.keys.clear();
        node.pointers.clear();
        node.pointers.push_back(left_part.position_in_disk);
        node.size = 0;
        node._is_leaf = false;
    } else {
        node = disk_read(path.top().first);
    }
    right_part.position_in_disk = _count_of_node;
    _count_of_node ++;
    right_part.keys.insert(
        right_part.keys.end(),
        left_part.keys.begin() + split + 1,
        left_part.keys.end()
    );
    right_part.pointers.insert(
        right_part.pointers.end(),
        left_part.pointers.begin() + split + 1,
        left_part.pointers.end()
    );
    right_part.size = right_part.keys.size();

    left_part.keys.erase(
        left_part.keys.begin() + split,
        left_part.keys.end()
    );
    left_part.pointers.erase(
        left_part.pointers.begin() + split + 1,
        left_part.pointers.end()
    );
    left_part.size = left_part.keys.size();

    node.keys.insert(node.keys.begin() + index, cur_data);
    node.pointers.insert(node.pointers.begin() + index + 1, right_part.position_in_disk);
    node.size++;
    //std::cout << "pupu " << node.pointers[0] << node.pointers[1] << "\n";
    disk_write(left_part);
    disk_write(right_part);

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
std::pair<std::stack<std::pair<size_t, size_t>>, std::pair<size_t,bool>>  B_tree_disk<tkey, tvalue, compare, t>::find_path(const tkey& key)
{
    std::stack<std::pair<size_t, size_t>> path;
    path.push({ROOT, 0});

    btree_disk_node node = disk_read(1);
    // std::cout << "find:" << "\n";

    while (true) {
        auto bs_res = find_index(key, node);
        // std::cout << "pupu " << bs_res.first << static_cast<int>(bs_res.second) << "\n";

        if (bs_res.second) { // node contains key
            return {path, bs_res};
        }

        if (node._is_leaf) { // return pointer for insert
            return {path, bs_res};
        }
        
        size_t child_pos = node.pointers[bs_res.first];
        path.push({child_pos, bs_res.first});
        // std::cout << "read child" << "\n";
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
        if (compare_keys(key, node.keys[mid - 1].first)) {
            high = mid;
        } else if (compare_keys(node.keys[mid - 1].first, key)) {
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
    std::cout << "| write " << node.position_in_disk << " " << static_cast<int>(node._is_leaf) << "\n";
    size_t offset = header_size + record_size * (node.position_in_disk - ROOT);
    _file_for_tree.seekp(static_cast<std::streamoff>(offset));
    _file_for_key_value.seekp(0, std::ios::end);
    node.serialize(_file_for_tree, _file_for_key_value);
    _file_for_tree.seekp(static_cast<std::streamoff>(0));
    _file_for_tree.write(reinterpret_cast<char*>(&_count_of_node), sizeof(size_t));
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node::deserialize(std::fstream &tree_stream, std::fstream& data_stream)
{
    btree_disk_node node;

    // std::cout << "reading\n";

    tree_stream.read(reinterpret_cast<char*>(&node.size), sizeof(node.size));
    tree_stream.read(reinterpret_cast<char*>(&node._is_leaf), sizeof(node._is_leaf));

    node.pointers.resize(node.size + 1, 0);
    for (size_t i = 0; i < max_ptrs; ++i) {
        size_t pos;
        tree_stream.read(reinterpret_cast<char*>(&pos), sizeof(pos));
        if (i < node.size + 1)
            node.pointers[i] = pos;
    }

    std::pair<tkey, tvalue> empty_key = {tkey(), tvalue()};
    node.keys.resize(node.size, empty_key);
    size_t off;
    for (size_t i = 0; i < max_keys; ++i) {
        tree_stream.read(reinterpret_cast<char*>(&off), sizeof(size_t));
        
        if (i < node.size) {
            // std::cout << "h1\n";
            data_stream.seekg(static_cast<std::streamoff>(off));
            node.keys[i].first  = tkey::deserialize(data_stream);
            node.keys[i].second = tvalue::deserialize(data_stream);
            // std::cout << "h2\n";
        }
    }

    // std::cout << "reading2\n";

    return node;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node B_tree_disk<tkey, tvalue, compare, t>::disk_read(size_t node_position)
{
    if (node_position == 0) {
        btree_disk_node node;
        return node;
    }
    size_t offset = header_size + record_size * (node_position - ROOT);
    _file_for_tree.seekg(static_cast<std::streamoff>(offset));
    btree_disk_node node = btree_disk_node::deserialize(_file_for_tree, _file_for_key_value);
    node.position_in_disk = node_position;
    return node;
}

// node implementation

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node::btree_disk_node(bool is_leaf) : _is_leaf(is_leaf), size(0), position_in_disk(0)
{
    pointers.clear();
    keys.clear();
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node::btree_disk_node() : _is_leaf(true), size(0), position_in_disk(0)
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

        _count_of_node = ROOT;
        btree_disk_node root;
        root.position_in_disk = ROOT;
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

// iterator

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::btree_disk_const_iterator(B_tree_disk<tkey, tvalue, compare, t>& tree, const std::stack<std::pair<size_t, size_t>> &path, size_t index) : _tree(tree), _path(path), _index(index) {}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator B_tree_disk<tkey, tvalue, compare, t>::begin()
{
    auto path = std::stack<std::pair<size_t, size_t>>();
    path.push({ROOT, 0});
    btree_disk_node cur_node = disk_read(ROOT);
    if (cur_node.size == 0) {
        return btree_disk_const_iterator(*this, path, 0);
    }
    
    while (!cur_node._is_leaf) {
        path.push({cur_node.pointers[0], 0});
        cur_node = disk_read(cur_node.pointers[0]);
    }
    return btree_disk_const_iterator(*this, path, 0);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator B_tree_disk<tkey, tvalue, compare, t>::end()
{
    auto path = std::stack<std::pair<size_t, size_t>>();
    path.push({ROOT, 0});
    btree_disk_node cur_node = disk_read(ROOT);
    if (cur_node.size == 0) {
        return btree_disk_const_iterator(*this, path, 0);
    }
    size_t k = cur_node.size;
    while (!cur_node._is_leaf) {
        path.push({cur_node.pointers[k], k});
        cur_node = disk_read(cur_node.pointers[k]);
        k = cur_node.size;
    }
    path.push({cur_node.pointers[k], k});
    return btree_disk_const_iterator(*this, path, 0);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self& B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator++()
{
    btree_disk_node node = _tree.disk_read(_path.top().first);
    if (node.position_in_disk == 0) { // before begin or end
        if (_path.top().second == 0) { // means before begin
            _index = _path.top().second;
            _path.pop();
        }
        return *this;
    }
    size_t k = node.size;
    if (!node._is_leaf) {
        _path.push({node.pointers[_index + 1], _index + 1});
        btree_disk_node cur_node = _tree.disk_read(node.pointers[_index + 1]);
        _index = 0;
        while (!cur_node._is_leaf) {
            _path.push({cur_node.pointers[0], 0});
            cur_node = _tree.disk_read(cur_node.pointers[0]);
        }
        return *this;
    } else if (_index + 1 < k) {
        _index++;
        return *this;
    } else {
        btree_disk_node cur_node = node;
        while (_path.size() > 1) {
            _index = _path.top().second;
            _path.pop();
            cur_node = _tree.disk_read(_path.top().first);
            k = cur_node.size;
            if (_index < k) {
                return *this;
            }
        }
        
    //     create end
        _index = 0;
        while (cur_node.position_in_disk != 0) {
            k = cur_node.size;
            _path.push({cur_node.pointers[k], k});
            cur_node = _tree.disk_read(cur_node.pointers[k]);
        }
        return *this;
    }
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator++(int)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self& B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator--()
{
    btree_disk_node node = _tree.disk_read(_path.top().first);
    if (node == nullptr) { // before begin or end
        if (_path.top().second != 0) { // means end
            _index = _path.top().second;
            _path.pop();
        }
        return *this;
    }

    size_t k = node.size;
    if (!node._is_leaf) {
        _path.push({node.pointers[_index], _index});
        btree_disk_node cur_node = _tree.disk_read(node.pointers[_index]);
        k = cur_node.size;
        _index = k - 1;
        while (!cur_node._is_leaf) {
            _path.push({cur_node.pointers[k], k});
            cur_node = _tree.disk_read(cur_node.pointers[k]);
            k = cur_node.size;
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
        btree_disk_node cur_node = _tree.disk_read(_path.top().first);
        while (cur_node.position_in_disk != 0) {
            _path.push({cur_node.pointers[0], 0});
            cur_node = _tree.disk_read(cur_node.pointers[0]);
        }
        return *this;
    }
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator--(int)
{
    auto tmp = *this;
    --(*this);
    return tmp;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator==(B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self &other) noexcept
{
    return (_path == other._path) && (_index == other._index);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator!=(B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self &other) noexcept
{
    return (_path != other._path) || (_index != other._index);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::value_type B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator*() noexcept
{
    btree_disk_node node = _tree.disk_read(_path.top().first);
    return node.keys[_index];
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
std::optional<tvalue> B_tree_disk<tkey, tvalue, compare, t>::at(const tkey & key)
{
    //std::cout << "at start\n";
    auto [path, bs_res] = find_path(key);
    //std::cout << "at end\n";
    auto [index, contain] = bs_res;
    if (!contain) {
        return std::nullopt;
    }
    btree_disk_node node = disk_read(path.top().first);
    return node.keys[index].second;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator, typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator>
B_tree_disk<tkey, tvalue, compare, t>::find_range(const tkey &lower, const tkey &upper, bool include_lower, bool include_upper)
{
    
}

#endif //B_TREE_DISK_HPP


class serializable_int {
public:
    int value;

    serializable_int(int v = 0) : value(v) {}

    void serialize(std::fstream& s) const {
        s.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    static serializable_int deserialize(std::fstream& s) {
        int v;
        s.read(reinterpret_cast<char*>(&v), sizeof(v));
        return serializable_int(v);
    }

    size_t serialize_size() const noexcept {
        return sizeof(value);
    }

    std::strong_ordering operator<=>(const serializable_int &other) const noexcept
    {
        return value <=> other.value;
    }
};


class serializable_string {
public:
    std::string value;

    serializable_string(const std::string& s = "") : value(s) {}

    void serialize(std::fstream& s) const {
        size_t k = value.size();
        s.write(reinterpret_cast<const char*>(&k), sizeof(k));
        if (k > 0) {
            s.write(value.data(), k);
        }
    }

    static serializable_string deserialize(std::fstream& s) {
        size_t k;
        s.read(reinterpret_cast<char*>(&k), sizeof(k));
        std::string str;
        str.resize(k);
        if (k > 0) {
            s.read(str.data(), k);
        }
        return serializable_string(str);
    }

    size_t serialize_size() const noexcept {
        return sizeof(size_t) + value.size();
    }
};