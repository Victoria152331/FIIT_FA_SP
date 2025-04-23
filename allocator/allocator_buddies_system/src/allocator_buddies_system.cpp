#include <not_implemented.h>
#include <cstddef>
#include "../include/allocator_buddies_system.h"

allocator_buddies_system::~allocator_buddies_system()
{
    auto *meta = reinterpret_cast<global_metadata *>(_trusted_memory);
    if (meta->logger) meta->logger->debug("~allocator_buddies_system - begin");
    if (meta->parent_allocator == nullptr)
        std::pmr::get_default_resource()->deallocate(_trusted_memory, size_t(1) << meta->size);
    else
        meta->parent_allocator->deallocate(_trusted_memory, size_t(1) << meta->size);
    if (meta->logger) meta->logger->debug("~allocator_buddies_system - end");
}

allocator_buddies_system::allocator_buddies_system(
    allocator_buddies_system &&other) noexcept
{
    _trusted_memory = other._trusted_memory;
    other._trusted_memory = nullptr;
}

allocator_buddies_system &allocator_buddies_system::operator=(
    allocator_buddies_system &&other) noexcept
{
    _trusted_memory = other._trusted_memory;
    other._trusted_memory = nullptr;
    return *this;
}

allocator_buddies_system::allocator_buddies_system(
        size_t space_size,
        std::pmr::memory_resource *parent_allocator,
        logger *logger,
        allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    unsigned char k = __detail::nearest_greater_k_of_2(space_size);
    if (k < min_k)
        throw std::logic_error("space_size must be >= min block size");

    try {
        if (parent_allocator == nullptr) {
            _trusted_memory = std::pmr::get_default_resource()->allocate((1 << k) + allocator_metadata_size);
        } else {
            _trusted_memory = parent_allocator->allocate((1 << k) + allocator_metadata_size);
        }
    }
    catch (const std::bad_alloc &) {
        if (logger)
            logger->error("allocator_boundary_tags::allocator_boundary_tags - std::bad_alloc thrown");
        throw "bad alloc";
    }

    auto *meta = reinterpret_cast<global_metadata *>(_trusted_memory);
    new(&(meta->mutex)) std::mutex;
    meta->logger = logger;
    meta->parent_allocator = parent_allocator;
    meta->size = k;
    meta->fit_mode = allocate_fit_mode;

    auto *block = reinterpret_cast<block_metadata *>(reinterpret_cast<char *>(_trusted_memory) + allocator_metadata_size);
    block->occupied = false;
    block->size = meta->size;

    if (meta->logger) {
        meta->logger->debug("allocator_buddies_system::constructor - end");
        auto res = format_blocks_info();
        meta->logger->debug(res.first);
        meta->logger->information(std::to_string(res.second));
    }
}

[[nodiscard]] void *allocator_buddies_system::do_allocate_sm(
    size_t size)
{
    auto *meta = reinterpret_cast<global_metadata *>(_trusted_memory);
    if (meta->logger) meta->logger->debug("do_allocate_sm - begin");
    void* res = nullptr;
    unsigned char need_size = __detail::nearest_greater_k_of_2(size + occupied_block_metadata_size);
    auto start = reinterpret_cast<char*>(_trusted_memory) + allocator_metadata_size;
    auto end = start + (1 << meta->size);
    auto cur_block = reinterpret_cast<block_metadata*>(start);

    meta->mutex.lock();
    
    try {
        if (meta->fit_mode == fit_mode::first_fit) {
            while (reinterpret_cast<char*>(cur_block) < end) {
                if (!cur_block->occupied && (cur_block->size >= need_size)) {
                    res = place_block(need_size, cur_block);
                    break;
                }
                cur_block = reinterpret_cast<block_metadata*>(reinterpret_cast<char*>(cur_block) + (1 << cur_block->size));
            }
            if (res == nullptr) {
                throw std::bad_alloc();
            }
        } else if (meta->fit_mode == fit_mode::the_best_fit) {
            block_metadata* best_block = nullptr;
            while (reinterpret_cast<char*>(cur_block) < end) {
                if (!cur_block->occupied && (cur_block->size >= need_size) &&
                ((best_block == nullptr) || (best_block->size > cur_block->size))) {
                    best_block = cur_block;
                }
                cur_block = reinterpret_cast<block_metadata*>(reinterpret_cast<char*>(cur_block) + (1 << cur_block->size));
            }
            if (best_block != nullptr) {
                res = place_block(need_size, best_block);
            } else {
                throw std::bad_alloc();
            }
        } else if (meta->fit_mode == fit_mode::the_worst_fit) {
            block_metadata* best_block = nullptr;
            while (reinterpret_cast<char*>(cur_block) < end) {
                if (!cur_block->occupied && (cur_block->size >= need_size) &&
                ((best_block == nullptr) || (best_block->size < cur_block->size))) {
                    best_block = cur_block;
                }
                cur_block = reinterpret_cast<block_metadata*>(reinterpret_cast<char*>(cur_block) + (1 << cur_block->size));
            }
            if (best_block != nullptr) {
                res = place_block(need_size, best_block);
            } else {
                throw std::bad_alloc();
            }
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

void* allocator_buddies_system::place_block(unsigned char size, block_metadata* cur_block)
{
    auto *meta = reinterpret_cast<global_metadata *>(_trusted_memory);

    void* res = reinterpret_cast<void*>(reinterpret_cast<char*>(cur_block) + occupied_block_metadata_size);
    *reinterpret_cast<void**>(reinterpret_cast<char*>(cur_block) + sizeof(block_metadata)) = _trusted_memory;
    cur_block->occupied = true;

    block_metadata* buddy;
    while ((cur_block->size > size) && (cur_block->size >= min_k)) {
        cur_block->size--;
        buddy = reinterpret_cast<block_metadata*>(
            reinterpret_cast<char*>(cur_block) + (1 << cur_block->size)
        );
        buddy->occupied = false;
        buddy->size = cur_block->size;
        
    }
    return res;
}

void allocator_buddies_system::do_deallocate_sm(void *at)
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    auto start = reinterpret_cast<char*>(_trusted_memory) + allocator_metadata_size;
    auto end = start + (1 << meta->size);
    meta->mutex.lock();
    try {
        if (meta->logger) meta->logger->debug("do_deallocate_sm - begin");
        if ((at == nullptr) || (reinterpret_cast<char*>(at) < start) || (reinterpret_cast<char*>(at) > end)) {
            throw std::runtime_error("do_deallocate_sm - invalid block");
        }

        block_metadata* block_to_free = reinterpret_cast<block_metadata*>(reinterpret_cast<char*>(at) - occupied_block_metadata_size);
        void** parent = reinterpret_cast<void**>(reinterpret_cast<char*>(at) - sizeof(void**));
    
        if ((*parent != _trusted_memory) || (block_to_free->occupied == false)) {
            throw std::runtime_error("do_deallocate_sm - invalid block");
        }
        block_to_free->occupied = false;
        
        block_metadata* buddy;
        size_t offset;
        while ((block_to_free->size != meta->size)) {
            offset = reinterpret_cast<char*>(block_to_free) - start;
            buddy = reinterpret_cast<block_metadata*>(start + (offset ^ (1 << block_to_free->size)));
            if ((buddy->occupied == true) || (buddy->size != block_to_free->size)) break;
            if (buddy < block_to_free) {
                block_to_free = buddy; 
            }
            block_to_free->size ++;
        }
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

std::pair<std::string, size_t> allocator_buddies_system::format_blocks_info() {
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

bool allocator_buddies_system::do_is_equal(const std::pmr::memory_resource &other) const noexcept
{
    if (dynamic_cast<const allocator_buddies_system*>(&other)->get_typename() == "allocator_buddies_system") {
        return dynamic_cast<const allocator_buddies_system*>(&other)->_trusted_memory == _trusted_memory;
    } else {
        return false;
    }
}

inline void allocator_buddies_system::set_fit_mode(
    allocator_with_fit_mode::fit_mode mode)
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    if (meta->logger) meta->logger->debug("set_fit_mode - begin");
    meta->mutex.lock();
    meta->fit_mode = mode;
    meta->mutex.unlock();
    if (meta->logger) meta->logger->debug("set_fit_mode - end");
}


std::vector<allocator_test_utils::block_info> allocator_buddies_system::get_blocks_info() const noexcept
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    if (meta->logger) meta->logger->debug("get_blocks_info - begin");
    meta->mutex.lock();
    auto res = get_blocks_info_inner();
    meta->mutex.unlock();
    if (meta->logger) meta->logger->debug("get_blocks_info - end");
    return res;
}

inline logger *allocator_buddies_system::get_logger() const
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    return meta->logger;
}

inline std::string allocator_buddies_system::get_typename() const
{
    return "allocator_buddies_system";
}

std::vector<allocator_test_utils::block_info> allocator_buddies_system::get_blocks_info_inner() const
{
    std::vector<block_info> blocks;
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    auto start = reinterpret_cast<char*>(_trusted_memory) + allocator_metadata_size;
    auto end = start + (1 << meta->size);
    auto cur_block = reinterpret_cast<block_metadata*>(start);
    while (reinterpret_cast<char*>(cur_block) < end) {
        blocks.push_back({static_cast<size_t>(1) << cur_block->size, cur_block->occupied});
        cur_block = reinterpret_cast<block_metadata*>(reinterpret_cast<char*>(cur_block) + (1 << cur_block->size));
    }
    return blocks;
}
