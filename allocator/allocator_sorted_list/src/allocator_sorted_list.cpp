#include <not_implemented.h>
#include "../include/allocator_sorted_list.h"

allocator_sorted_list::~allocator_sorted_list()
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    if (meta->logger) meta->logger->debug("~allocator_sorted_list - begin");

    if (meta->parent_allocator == nullptr) {
        std::pmr::get_default_resource()->deallocate(_trusted_memory, meta->space_size);
    } else {
        meta->parent_allocator->deallocate(_trusted_memory, meta->space_size);
    }
    if (meta->logger) meta->logger->debug("~allocator_sorted_list - end");
}


allocator_sorted_list::allocator_sorted_list(
    allocator_sorted_list &&other) noexcept
{
    _trusted_memory = other._trusted_memory;
    other._trusted_memory = nullptr;
}

allocator_sorted_list &allocator_sorted_list::operator=(
    allocator_sorted_list &&other) noexcept
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

allocator_sorted_list::allocator_sorted_list(
        size_t space_size,
        std::pmr::memory_resource *parent_allocator,
        logger *logger,
        allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    if (logger) logger->debug("allocator_sorted_list::allocator_sorted_list - constructor begin");
    try {
        if (parent_allocator == nullptr) {
            _trusted_memory = std::pmr::get_default_resource()->allocate(space_size + allocator_metadata_size);
        } else {
            _trusted_memory = parent_allocator->allocate(space_size + allocator_metadata_size);
        }
    }
    catch (const std::bad_alloc &) {
        if (logger)
            logger->error("allocator_sorted_list::allocator_sorted_list - std::bad_alloc thrown");
        throw;
    }

    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    meta->logger = logger;
    meta->fit_mode = allocate_fit_mode;
    meta->parent_allocator = parent_allocator;
    meta->space_size = space_size + allocator_metadata_size;
    new(&(meta->mutex)) std::mutex;

    block_metadata* block = reinterpret_cast<block_metadata*>(reinterpret_cast<char*>(_trusted_memory) + allocator_metadata_size);
    block->block_size = meta->space_size - allocator_metadata_size;
    block->ptr = nullptr;
    meta->first_block = reinterpret_cast<void*>(block);

    if (meta->logger) {
        meta->logger->debug("allocator_sorted_list::allocator_sorted_list - constructor end");
        auto res = format_blocks_info();
        // for (block_metadata* cur = reinterpret_cast<block_metadata*>(meta->first_block); cur != nullptr; cur = reinterpret_cast<block_metadata*>(cur->ptr)) {
        //     std::cout << cur->block_size << "|";
        // }
        // std::cout << std::endl;
        meta->logger->debug(res.first);
        meta->logger->information(std::to_string(res.second));
    }
}

[[nodiscard]] void *allocator_sorted_list::do_allocate_sm(
    size_t size)
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    if (meta->logger) meta->logger->debug("do_allocate_sm - begin");

    void* res = nullptr;
    size_t need_size = size + block_metadata_size;
    meta->mutex.lock();

    try {
        if (meta->fit_mode == fit_mode::first_fit
                || meta->fit_mode == fit_mode::the_best_fit) {
            
            block_metadata** cur_ptr = reinterpret_cast<block_metadata**>(&(meta->first_block));;
            block_metadata* cur = reinterpret_cast<block_metadata*>(meta->first_block);
            while ((cur != nullptr) && (cur->block_size < need_size)) {
                cur_ptr = reinterpret_cast<block_metadata**>(&(cur->ptr));
                cur = reinterpret_cast<block_metadata*>(cur->ptr);
            }
            if (cur == nullptr) {
                throw std::bad_alloc();
            }
            // if (meta->logger) meta->logger->debug("place in");
            res = place_in(cur_ptr, size);

        } else if (meta->fit_mode == fit_mode::the_worst_fit) {

            block_metadata** cur_ptr = reinterpret_cast<block_metadata**>(&(meta->first_block));;
            block_metadata* cur = reinterpret_cast<block_metadata*>(meta->first_block);

            while ((cur != nullptr) && (cur->ptr != nullptr)) {
                cur_ptr = reinterpret_cast<block_metadata**>(&(cur->ptr));
                cur = reinterpret_cast<block_metadata*>(cur->ptr);
            }
            if ((cur == nullptr) || (cur->block_size < need_size)) {
                throw std::bad_alloc();
            }
            //if (meta->logger) meta->logger->debug("place in");
            res = place_in(cur_ptr, size);
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
        // for (block_metadata* cur = reinterpret_cast<block_metadata*>(meta->first_block); cur != nullptr; cur = reinterpret_cast<block_metadata*>(cur->ptr)) {
        //     std::cout << cur->block_size << "|";
        // }
        // std::cout << std::endl;
        meta->logger->debug(res.first);
        meta->logger->information(std::to_string(res.second));
    }
    return res;
}

void* allocator_sorted_list::place_in(block_metadata** block, size_t size) {
    if (*block == nullptr) {
        throw std::bad_alloc();
    }
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    block_metadata* new_occ_block = *block;
    if ((*block)->block_size <= size + 2 * block_metadata_size) {
        *block = reinterpret_cast<block_metadata*>((*block)->ptr);
        new_occ_block->ptr = _trusted_memory;
    } else {
        //meta->logger->debug("new free");
        block_metadata* new_free_block = reinterpret_cast<block_metadata*>(
            reinterpret_cast<char*>(new_occ_block) + size + block_metadata_size
        );
        *block = reinterpret_cast<block_metadata*>((*block)->ptr);
        new_free_block->block_size = new_occ_block->block_size - size - block_metadata_size;
        new_free_block->ptr = nullptr;
        new_occ_block->ptr = _trusted_memory;
        new_occ_block->block_size = size + block_metadata_size;
        insert_in_list(new_free_block);
    }
    return reinterpret_cast<void*>(
        reinterpret_cast<char*>(new_occ_block) + block_metadata_size
    );
    
}

bool allocator_sorted_list::do_is_equal(const std::pmr::memory_resource &other) const noexcept
{
    if (dynamic_cast<const allocator_sorted_list*>(&other)->get_typename() == "allocator_sorted_list") {
        return dynamic_cast<const allocator_sorted_list*>(&other)->_trusted_memory == _trusted_memory;
    } else {
        return false;
    }
    
}

void allocator_sorted_list::do_deallocate_sm(
    void *at)
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    if (meta->logger) meta->logger->debug("do_deallocate_sm - begin");
    block_metadata* block_to_del = reinterpret_cast<block_metadata*>(reinterpret_cast<char*>(at) - block_metadata_size);
    meta->mutex.lock();
    try {
        if (block_to_del->ptr != _trusted_memory) {
            meta->mutex.unlock();
            throw std::runtime_error("do_deallocate_sm - invalid block");
        }
        block_metadata** cur_free_block = reinterpret_cast<block_metadata**>(&(meta->first_block));
        block_metadata* left_block = nullptr;
        block_metadata* right_block = nullptr;
        block_to_del->ptr = nullptr;
        // if (meta->logger) meta->logger->debug("iterating start");
        while ((*cur_free_block != nullptr)) {
            if (reinterpret_cast<char*>(*cur_free_block) + (*cur_free_block)->block_size == reinterpret_cast<char*>(block_to_del)) {
                left_block = *cur_free_block; 
                *cur_free_block = reinterpret_cast<block_metadata*>(left_block->ptr);
            } else  if (reinterpret_cast<char*>(*cur_free_block) == reinterpret_cast<char*>(block_to_del) + block_to_del->block_size) {
                right_block  = *cur_free_block;
                *cur_free_block = reinterpret_cast<block_metadata*>(right_block->ptr);
            } else {
                cur_free_block = reinterpret_cast<block_metadata**>(&((*cur_free_block)->ptr));
            }
        }
        // if (meta->logger) meta->logger->debug("iterating end");
        if (left_block) {
            left_block->block_size += block_to_del->block_size;
            left_block->ptr = nullptr;
            block_to_del = left_block;
        }
        if (right_block) {
            block_to_del->block_size += right_block->block_size;
        }
        // if (meta->logger) meta->logger->debug("insert");
        insert_in_list(block_to_del);
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
        // for (block_metadata* cur = reinterpret_cast<block_metadata*>(meta->first_block); cur != nullptr; cur = reinterpret_cast<block_metadata*>(cur->ptr)) {
        //     std::cout << cur->block_size << "|";
        // }
        // std::cout << std::endl;
        meta->logger->debug(res.first);
        meta->logger->information(std::to_string(res.second));
    }
}

void allocator_sorted_list::insert_in_list (block_metadata* block) {
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    block_metadata* cur_block = reinterpret_cast<block_metadata*>(meta->first_block);
    if (cur_block == nullptr) {
        meta->first_block = block;
        block->ptr = nullptr;
        return;
    }
    if (block->block_size < cur_block->block_size) {
        meta->first_block = block;
        block->ptr = cur_block;
    } else {
        while ((cur_block->ptr != nullptr) && 
        (reinterpret_cast<block_metadata*>(cur_block->ptr)->block_size < block->block_size)) {
            cur_block = reinterpret_cast<block_metadata*>(cur_block->ptr);
        }
        block->ptr = cur_block->ptr;
        cur_block->ptr = reinterpret_cast<void*>(block);
    }
}

inline void allocator_sorted_list::set_fit_mode(
    allocator_with_fit_mode::fit_mode mode)
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    if (meta->logger) meta->logger->debug("set_fit_mode - begin");
    meta->mutex.lock();
    meta->fit_mode = mode;
    meta->mutex.unlock();
    if (meta->logger) meta->logger->debug("set_fit_mode - end");
}

std::vector<allocator_test_utils::block_info> allocator_sorted_list::get_blocks_info() const noexcept
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    if (meta->logger) meta->logger->debug("get_blocks_info - begin");
    meta->mutex.lock();
    std::vector<allocator_test_utils::block_info> res = get_blocks_info_inner();
    meta->mutex.unlock();
    if (meta->logger) meta->logger->debug("get_blocks_info - end");
    return res;
}

inline logger *allocator_sorted_list::get_logger() const
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    return meta->logger;
}

inline std::string allocator_sorted_list::get_typename() const
{
    return "allocator_sorted_list";
}


std::vector<allocator_test_utils::block_info> allocator_sorted_list::get_blocks_info_inner() const
{
    std::vector<block_info> blocks;
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);

    size_t total = allocator_metadata_size;
    block_metadata* cur = reinterpret_cast<block_metadata*>(
        reinterpret_cast<char*>(_trusted_memory) + allocator_metadata_size
    );
    while (total < meta->space_size) {
        total += cur->block_size;
        if (cur->ptr == _trusted_memory) {
            blocks.push_back({cur->block_size, true});
        } else {
            blocks.push_back({cur->block_size, false});
        }
        cur = reinterpret_cast<block_metadata*>(reinterpret_cast<char*>(cur) + cur->block_size);
    }
    return blocks;
}

std::pair<std::string, size_t> allocator_sorted_list::format_blocks_info() {
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