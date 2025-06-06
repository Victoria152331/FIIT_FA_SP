#include <not_implemented.h>

#include "../include/allocator_red_black_tree.h"

allocator_red_black_tree::~allocator_red_black_tree()
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    if (meta->logger) meta->logger->debug("~allocator_red_black_tree - begin");

    if (meta->parent_allocator == nullptr) {
        std::pmr::get_default_resource()->deallocate(_trusted_memory, meta->space_size);
    } else {
        meta->parent_allocator->deallocate(_trusted_memory, meta->space_size);
    }
    if (meta->logger) meta->logger->debug("~allocator_red_black_tree - end");
}

allocator_red_black_tree::allocator_red_black_tree(
    allocator_red_black_tree &&other) noexcept
{
    _trusted_memory = other._trusted_memory;
    other._trusted_memory = nullptr;
}

allocator_red_black_tree &allocator_red_black_tree::operator=(
    allocator_red_black_tree &&other) noexcept
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    if (meta->parent_allocator == nullptr) {
        std::pmr::get_default_resource()->deallocate(_trusted_memory, meta->space_size);
    } else {
        meta->parent_allocator->deallocate(_trusted_memory, meta->space_size);
    }

    _trusted_memory = other._trusted_memory;
    other._trusted_memory = nullptr;
    return *this;
}

allocator_red_black_tree::allocator_red_black_tree(
        size_t space_size,
        std::pmr::memory_resource *parent_allocator,
        logger *logger,
        allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    if (logger) logger->debug("allocator_red_black_tree::allocator_red_black_tree - constructor begin");
    try {
        if (parent_allocator == nullptr) {
            _trusted_memory = std::pmr::get_default_resource()->allocate(space_size + allocator_metadata_size);
        } else {
            _trusted_memory = parent_allocator->allocate(space_size + allocator_metadata_size);
        }
    }
    catch (const std::bad_alloc &) {
        if (logger)
            logger->error("allocator_red_black_tree::allocator_red_black_tree - std::bad_alloc thrown");
        throw;
    }

    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    meta->logger = logger;
    meta->fit_mode = allocate_fit_mode;
    meta->parent_allocator = parent_allocator;
    meta->space_size = space_size + allocator_metadata_size;
    new(&(meta->mutex)) std::mutex;

    free_block_metadata* block = reinterpret_cast<free_block_metadata*>(reinterpret_cast<char*>(_trusted_memory) + allocator_metadata_size);
    block->back = nullptr;
    block->front = nullptr;
    block->data.color = block_color::BLACK;
    block->data.occupied = false;
    block->left = nullptr;
    block->right = nullptr;
    block->parent = nullptr;
    meta->root = block;

    if (meta->logger) {
        meta->logger->debug("allocator_red_black_tree::allocator_red_black_tree - constructor end");
        auto res = format_blocks_info();
        meta->logger->debug(res.first);
        meta->logger->information(std::to_string(res.second));
    }
}

bool allocator_red_black_tree::do_is_equal(const std::pmr::memory_resource &other) const noexcept
{
    if (dynamic_cast<const allocator_red_black_tree*>(&other)->get_typename() == "allocator_red_black_tree") {
        return dynamic_cast<const allocator_red_black_tree*>(&other)->_trusted_memory == _trusted_memory;
    } else {
        return false;
    }
    
}

size_t allocator_red_black_tree::block_size(block_metadata* block) const
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    size_t _size;
    if (block == nullptr) {
        return 0;
    }
    if (block->front == nullptr) {
        _size = reinterpret_cast<char*>(_trusted_memory) + meta->space_size
                - reinterpret_cast<char*>(block);
    } else {
        _size = reinterpret_cast<char*>(block->front) -
            reinterpret_cast<char*>(block);
    }
    return _size;
}

[[nodiscard]] void *allocator_red_black_tree::do_allocate_sm(
    size_t size)
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    if (meta->logger) meta->logger->debug("do_allocate_sm - begin");

    void* res = nullptr;
    size_t need_size = size + occupied_block_metadata_size;
    meta->mutex.lock();

    try {
        if (meta->fit_mode == fit_mode::first_fit) {
            free_block_metadata* cur = meta->root;
            while ((cur != nullptr) && block_size(static_cast<block_metadata*>(cur)) < need_size) {
                cur = cur->right;
            } 

            if (cur == nullptr) {
                throw std::bad_alloc();
            }

            res = place_in(cur, size);
        } else if (meta->fit_mode == fit_mode::the_best_fit) {
            free_block_metadata* cur = meta->root;
            size_t cur_size, left_size;
            while ((cur != nullptr)) {
                cur_size = block_size(static_cast<block_metadata*>(cur));
                left_size = block_size(static_cast<block_metadata*>(cur->left));
                if (cur_size < need_size) {
                    cur = cur->right;
                } else if (left_size >= need_size) {
                    cur = cur->left;
                } else {
                    break;
                }
            }
            if (cur == nullptr) {
                throw std::bad_alloc();
            }

            res = place_in(cur, size);
        } else {
            free_block_metadata* cur = meta->root;
            while ((cur != nullptr) && (cur->right != nullptr)) {
                cur = cur->right;
            }

            if ((cur == nullptr) || (block_size(static_cast<block_metadata*>(cur)) < need_size)) {
                throw std::bad_alloc();
            }

            res = place_in(cur, size);
        }
    }
    catch (const std::bad_alloc &) {
        meta->mutex.unlock();
        if (meta->logger) meta->logger->error("do_allocate_sm - std::bad_alloc thrown");
        throw;
    }
    meta->mutex.unlock();

    if (meta->logger) {
        meta->logger->debug("do_allocate_sm - end");
        auto res = format_blocks_info();
        meta->logger->debug(res.first);
        meta->logger->information(std::to_string(res.second));
    }
    return res;
}

void* allocator_red_black_tree::place_in(free_block_metadata* block, size_t size)
{
    erase_block(block);
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    free_block_metadata old_block = *block;
    occ_block_metadata* new_occ = reinterpret_cast<occ_block_metadata*>(block);
    new_occ->data.occupied = true;
    new_occ->back = old_block.back;
    new_occ->front = old_block.front;
    new_occ->parent =  _trusted_memory;
    if (block_size(static_cast<block_metadata*>(new_occ))
            > size + occupied_block_metadata_size + free_block_metadata_size)
    {   
        free_block_metadata* new_free = reinterpret_cast<free_block_metadata*>(
            reinterpret_cast<char*>(new_occ) + size + occupied_block_metadata_size
        );
        new_free->data.occupied = false;
        new_free->front = new_occ->front;
        new_free->back = static_cast<block_metadata*>(new_occ);
        new_occ->front = static_cast<block_metadata*>(new_free);
        new_free->left = nullptr;
        new_free->right = nullptr;
        new_free->parent = nullptr;
        insert_block(new_free);
    }
    return reinterpret_cast<void*>(
        reinterpret_cast<char*>(new_occ) + occupied_block_metadata_size
    );
} 


void allocator_red_black_tree::do_deallocate_sm(
    void *at)
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    if (meta->logger) meta->logger->debug("do_deallocate_sm - begin");
    occ_block_metadata* block_to_free = reinterpret_cast<occ_block_metadata*>(reinterpret_cast<char*>(at) - occupied_block_metadata_size);
    meta->mutex.lock();
    try {
        if (block_to_free->parent != _trusted_memory) {
            meta->mutex.unlock();
            throw std::runtime_error("do_deallocate_sm - invalid block");
        }

        occ_block_metadata old_block = *block_to_free;
        free_block_metadata* to_insert = reinterpret_cast<free_block_metadata*>(block_to_free);
        to_insert->front = old_block.front;
        to_insert->back = old_block.back;
        to_insert->left = nullptr;
        to_insert->right = nullptr;
        to_insert->parent = nullptr;
        to_insert->data.occupied = false;
        to_insert->data.color = block_color::RED;
        
        if (to_insert->back != nullptr && !to_insert->back->data.occupied) {
            erase_block(static_cast<free_block_metadata*>(to_insert->back));
            to_insert->back->front = to_insert->front;
            if (to_insert->front) to_insert->front->back = to_insert->back;
            to_insert = static_cast<free_block_metadata*>(to_insert->back);
        }
        if (to_insert->front != nullptr && !to_insert->front->data.occupied) {
            erase_block(static_cast<free_block_metadata*>(to_insert->front));
            to_insert->front = to_insert->front->front;
            if (to_insert->front) to_insert->front->back = to_insert;
        }
        insert_block(to_insert);
    } 
    catch (...) {
        meta->mutex.unlock();
        if (meta->logger) meta->logger->error("do_deallocate_sm - exception during deallocation");
        throw;
    }
    meta->mutex.unlock();
    if (meta->logger) {
        meta->logger->debug("do_deallocate_sm - end");;
        auto res = format_blocks_info();
        meta->logger->debug(res.first);
        meta->logger->information(std::to_string(res.second));
    }
}

bool allocator_red_black_tree::compare_size(free_block_metadata* lhs, free_block_metadata* rhs)
{
    return block_size(static_cast<block_metadata*>(lhs)) < block_size(static_cast<block_metadata*>(rhs));
}

void allocator_red_black_tree::insert_block(free_block_metadata* block)
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    free_block_metadata* cur = meta->root;
    free_block_metadata* prev = nullptr;
    bool is_left = false;

    if (cur == nullptr) {
        meta->root = block;
        block->parent = nullptr;
        block->left = nullptr;
        block->right = nullptr;
        block->data.color = block_color::BLACK;
        return;
    }

    while (cur != nullptr) {
        if (compare_size(cur, block)) {
            prev = cur;
            cur = cur->right;
            is_left = false;
        } else {
            prev = cur;
            cur = cur->left;
            is_left = true;
        }
    }

    if (is_left) {
        prev->left = block;
    } else {
        prev->right = block;
    }
    block->parent = prev;
    block->left = nullptr;
    block->right = nullptr;
    block->data.color = block_color::RED;

    // balance
}

void allocator_red_black_tree::erase_block(free_block_metadata* block)
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    free_block_metadata** link;
    free_block_metadata *start_balance, *start_balance_pr;
    if (block->parent == nullptr) {
        link = &(meta->root);
    } else if (block->parent->left == block) {
        link = &(block->parent->left);
    } else {
        link = &(block->parent->right);
    }
    if ((block->left == nullptr) && (block->right == nullptr)) {
        *link = nullptr;
        start_balance = nullptr;
        start_balance_pr = block->parent;
    } 
    else if ((block->left == nullptr) && (block->right != nullptr)) {

        *link = block->right;
        block->right->parent = block->parent;
        start_balance = block->right;
        start_balance_pr = block->parent;
    } 
    else if ((block->left != nullptr) && (block->right == nullptr)) {

        *link = block->left;
        block->left->parent = block->parent;
        start_balance = block->left;
        start_balance_pr = block->parent;
    } else {

        free_block_metadata* to_swap = block->left;

        if (to_swap->right == nullptr) {
            to_swap->parent = block->parent;
            *link = to_swap;
            to_swap->right = block->right;
            block->right->parent = to_swap;
            block->parent = to_swap;
            std::swap(to_swap->data, block->data);
            start_balance = to_swap->left;
            start_balance_pr = to_swap;
        } else {

            while (to_swap->right != nullptr) {
                to_swap = to_swap->right;
            }
            to_swap->parent->right = to_swap->left;
            if (to_swap->left) {
                to_swap->left->parent = to_swap->parent;
            }
    
            std::swap(to_swap->left, block->left);
            std::swap(to_swap->right, block->right);
            std::swap(to_swap->parent, block->parent);
            std::swap(to_swap->data, block->data);
    
            *link = to_swap;
    
            to_swap->left->parent = to_swap;
            to_swap->right->parent = to_swap;

            start_balance = to_swap->left;
            start_balance_pr = to_swap;
        }
    }
}



void allocator_red_black_tree::set_fit_mode(allocator_with_fit_mode::fit_mode mode)
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    if (meta->logger) meta->logger->debug("set_fit_mode - begin");
    meta->mutex.lock();
    meta->fit_mode = mode;
    meta->mutex.unlock();
    if (meta->logger) meta->logger->debug("set_fit_mode - end");
}


std::vector<allocator_test_utils::block_info> allocator_red_black_tree::get_blocks_info() const
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    if (meta->logger) meta->logger->debug("get_blocks_info - begin");
    meta->mutex.lock();
    std::vector<allocator_test_utils::block_info> res = get_blocks_info_inner();
    meta->mutex.unlock();
    if (meta->logger) meta->logger->debug("get_blocks_info - end");
    return res;
}

inline logger *allocator_red_black_tree::get_logger() const
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    return meta->logger;
}

std::vector<allocator_test_utils::block_info> allocator_red_black_tree::get_blocks_info_inner() const
{
    std::vector<block_info> blocks;
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);

    block_metadata* cur = reinterpret_cast<block_metadata*>(
        reinterpret_cast<char*>(_trusted_memory) + allocator_metadata_size
    );

    while (cur != nullptr) {
        blocks.push_back({block_size(cur), cur->data.occupied});
        cur = cur->front;
    }
    return blocks;
}

std::pair<std::string, size_t> allocator_red_black_tree::format_blocks_info() {
    std::stringstream res;
    size_t free_memory = 0;
    auto blocks = get_blocks_info_inner();
    for (auto & block : blocks) {
        if (!block.is_block_occupied) {
            free_memory += block.block_size;
        }
        res << (block.is_block_occupied ? "occup" : "avail") << " " << block.block_size << "|";
    }
    return {res.str(), free_memory};
}

inline std::string allocator_red_black_tree::get_typename() const noexcept
{
    return "allocator_red_black_tree";
}