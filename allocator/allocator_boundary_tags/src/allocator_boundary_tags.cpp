#include <not_implemented.h>
#include "../include/allocator_boundary_tags.h"

allocator_boundary_tags::~allocator_boundary_tags()
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    if (meta->logger) meta->logger->debug("~allocator_boundary_tags - begin");

    if (meta->parent_allocator == nullptr) {
        std::pmr::get_default_resource()->deallocate(_trusted_memory, meta->space_size);
    } else {
        meta->parent_allocator->deallocate(_trusted_memory, meta->space_size);
    }
    if (meta->logger) meta->logger->debug("~allocator_boundary_tags - end");
}

allocator_boundary_tags::allocator_boundary_tags(
    allocator_boundary_tags &&other) noexcept
{
    _trusted_memory = other._trusted_memory;
    other._trusted_memory = nullptr;
}

allocator_boundary_tags &allocator_boundary_tags::operator=(
    allocator_boundary_tags &&other) noexcept
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


/** If parent_allocator* == nullptr you should use std::pmr::get_default_resource()
 */
allocator_boundary_tags::allocator_boundary_tags(
        size_t space_size,
        std::pmr::memory_resource *parent_allocator,
        logger *logger,
        allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    if (logger) logger->debug("allocator_boundary_tags::allocator_boundary_tags - constructor begin");
    try {
        if (parent_allocator == nullptr) {
            _trusted_memory = std::pmr::get_default_resource()->allocate(space_size + allocator_metadata_size);
        } else {
            _trusted_memory = parent_allocator->allocate(space_size + allocator_metadata_size);
        }
    }
    catch (const std::bad_alloc &) {
        if (logger)
            logger->error("allocator_boundary_tags::allocator_boundary_tags - std::bad_alloc thrown");
        throw "bad alloc";
    }

    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    meta->logger = logger;
    meta->fit_mode = allocate_fit_mode;
    meta->parent_allocator = parent_allocator;
    meta->space_size = space_size + allocator_metadata_size;
    new(&(meta->mutex)) std::mutex;
    meta->first_block = nullptr;

    if (meta->logger) {
        meta->logger->debug("allocator_boundary_tags::allocator_boundary_tags - constructor end");
        auto res = format_blocks_info();
        meta->logger->debug(res.first);
        meta->logger->information(std::to_string(res.second));
    }
}

[[nodiscard]] void *allocator_boundary_tags::do_allocate_sm(
    size_t size)
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    if (meta->logger) meta->logger->debug("do_allocate_sm - begin");

    // if (size == 0) {
    //     if (meta->logger) {
    //         meta->logger->warning("do_allocate_sm - user requested 0 bytes");
    //         meta->logger->debug("do_allocate_sm - end with zero");
    //     }
    //     return nullptr;
    // }

    void* res = nullptr;
    size_t need_size = size + occupied_block_metadata_size;
    meta->mutex.lock();

    try {
        if (meta->first_block == nullptr) {

            if (need_size > (meta->space_size - allocator_metadata_size)) {
                throw std::bad_alloc();
            }
            res = place_first(meta, size);

        } else if (meta->fit_mode == fit_mode::first_fit) {

            block_metadata* cur_block_meta = reinterpret_cast<block_metadata*>(meta->first_block);
            size_t empty_space = (reinterpret_cast<char*>(cur_block_meta) - reinterpret_cast<char*>(_trusted_memory)) - allocator_metadata_size;
            if (empty_space >= need_size) {
                res = place_first(meta, size + ((empty_space - need_size >= occupied_block_metadata_size) ? 0 : (empty_space - need_size)));
            }

            while ((res == nullptr) && (cur_block_meta->next != nullptr)) {
                empty_space = (reinterpret_cast<char*>(cur_block_meta->next) - reinterpret_cast<char*>(cur_block_meta)) - occupied_block_metadata_size - cur_block_meta->block_size;
                if (empty_space >= need_size) {
                    res = place_after(cur_block_meta, size + ((empty_space - need_size >= occupied_block_metadata_size) ? 0 : (empty_space - need_size)));
                }
                cur_block_meta = reinterpret_cast<block_metadata*>(cur_block_meta->next);
            }

            empty_space = ((reinterpret_cast<char*>(_trusted_memory) + meta->space_size) - reinterpret_cast<char*>(cur_block_meta)) - occupied_block_metadata_size - cur_block_meta->block_size;
            if ((res == nullptr) && (empty_space >= need_size)) {
                res = place_after(cur_block_meta, size + ((empty_space - need_size >= occupied_block_metadata_size) ? 0 : (empty_space - need_size)));
            } else if (res == nullptr) {
                throw std::bad_alloc();
            }
        } else if (meta->fit_mode == fit_mode::the_best_fit) {
            block_metadata* cur_block_meta = reinterpret_cast<block_metadata*>(meta->first_block);
            block_metadata* best_space_prev = reinterpret_cast<block_metadata*>(meta->first_block);
            size_t best_space = 0;
            bool is_first_space_fit = false;
            size_t empty_space = (reinterpret_cast<char*>(cur_block_meta) - reinterpret_cast<char*>(_trusted_memory)) - allocator_metadata_size;

            if (empty_space >= need_size) {
                best_space = empty_space;
                is_first_space_fit = true;
            }

            while (cur_block_meta->next != nullptr) {
                empty_space = (reinterpret_cast<char*>(cur_block_meta->next) - reinterpret_cast<char*>(cur_block_meta)) - occupied_block_metadata_size - cur_block_meta->block_size;
                if ((empty_space >= need_size) && ((best_space == 0) || (empty_space < best_space))) {
                    best_space = empty_space;
                    is_first_space_fit = false;
                    best_space_prev = cur_block_meta;
                }
                cur_block_meta = reinterpret_cast<block_metadata*>(cur_block_meta->next);
            }
            
            empty_space = ((reinterpret_cast<char*>(_trusted_memory) + meta->space_size) - reinterpret_cast<char*>(cur_block_meta)) - occupied_block_metadata_size - cur_block_meta->block_size;
            if ((empty_space >= need_size) && ((best_space == 0) || (empty_space < best_space))) {
                best_space = empty_space;
                is_first_space_fit = false;
                best_space_prev = cur_block_meta;
            }

            if (best_space == 0) {
                throw std::bad_alloc();;
            } else if (is_first_space_fit) {
                res = place_first(meta, size + ((best_space - need_size >= occupied_block_metadata_size) ? 0 : (best_space - need_size)));
            } else {
                res = place_after(best_space_prev, (size + (best_space - need_size >= occupied_block_metadata_size) ? 0 : (best_space - need_size)));
            }
        } else if (meta->fit_mode == fit_mode::the_worst_fit) {
        block_metadata* cur_block_meta = reinterpret_cast<block_metadata*>(meta->first_block);
        block_metadata* best_space_prev = reinterpret_cast<block_metadata*>(meta->first_block);
        size_t best_space = 0;
        bool is_first_space_fit = false;
        size_t empty_space = (reinterpret_cast<char*>(cur_block_meta) - reinterpret_cast<char*>(_trusted_memory)) - allocator_metadata_size;

        if (empty_space >= need_size) {
            best_space = empty_space;
            is_first_space_fit = true;
        }

        while (cur_block_meta->next != nullptr) {
            empty_space = (reinterpret_cast<char*>(cur_block_meta->next) - reinterpret_cast<char*>(cur_block_meta)) - occupied_block_metadata_size - cur_block_meta->block_size;
            if ((empty_space >= need_size) && (empty_space > best_space)) {
                best_space = empty_space;
                is_first_space_fit = false;
                best_space_prev = cur_block_meta;
            }
            cur_block_meta = reinterpret_cast<block_metadata*>(cur_block_meta->next);
        }
        
        empty_space = ((reinterpret_cast<char*>(_trusted_memory) + meta->space_size) - reinterpret_cast<char*>(cur_block_meta)) - occupied_block_metadata_size - cur_block_meta->block_size;
        if ((empty_space >= need_size) && (empty_space > best_space)) {
            best_space = empty_space;
            is_first_space_fit = false;
            best_space_prev = cur_block_meta;
        }

        if (best_space == 0) {
            throw std::bad_alloc();
        } else if (is_first_space_fit) {
            res = place_first(meta, size + ((best_space - need_size >= occupied_block_metadata_size) ? 0 : (best_space - need_size)));
        } else {
            res = place_after(best_space_prev, size + ((best_space - need_size >= occupied_block_metadata_size) ? 0 : (best_space - need_size)));
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

void* allocator_boundary_tags::place_after(allocator_boundary_tags::block_metadata* cur_block_meta, size_t size) {
    block_metadata* new_block_meta = reinterpret_cast<block_metadata*>(
        reinterpret_cast<char*>(cur_block_meta) + occupied_block_metadata_size + cur_block_meta->block_size);

    new_block_meta->block_size = size;
    new_block_meta->parent = _trusted_memory;

    new_block_meta->prev = reinterpret_cast<void*>(cur_block_meta);
    new_block_meta->next = cur_block_meta->next;
    cur_block_meta->next = reinterpret_cast<void*>(new_block_meta);
    if (new_block_meta->next != nullptr) {
        reinterpret_cast<block_metadata*>(new_block_meta->next)->prev = reinterpret_cast<void*>(new_block_meta);
    }
    return reinterpret_cast<void*>(reinterpret_cast<char*>(new_block_meta) + occupied_block_metadata_size);
}

void* allocator_boundary_tags::place_first(global_metadata* meta, size_t size) {
    block_metadata* new_block_meta = reinterpret_cast<block_metadata*>(
        reinterpret_cast<char*>(_trusted_memory) + allocator_metadata_size);
    new_block_meta->block_size = size;
    new_block_meta->parent = _trusted_memory;

    new_block_meta->prev = _trusted_memory;
    new_block_meta->next = meta->first_block;
    meta->first_block = reinterpret_cast<void*>(new_block_meta);
    
    if (new_block_meta->next != nullptr) {
        reinterpret_cast<block_metadata*>(new_block_meta->next)->prev = reinterpret_cast<void*>(new_block_meta);
    }
    
    return reinterpret_cast<void*>(reinterpret_cast<char*>(new_block_meta) + occupied_block_metadata_size);
}

void allocator_boundary_tags::do_deallocate_sm(
    void *at)
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    if (meta->logger) meta->logger->debug("do_deallocate_sm - begin");
    block_metadata* block_to_del = reinterpret_cast<block_metadata*>(reinterpret_cast<char*>(at) - occupied_block_metadata_size);
    meta->mutex.lock();
    try {
        if (block_to_del->parent != _trusted_memory) {
            meta->mutex.unlock();
            throw std::runtime_error("do_deallocate_sm - invalid block");
        }
        if (block_to_del->prev == _trusted_memory) {
            meta->first_block = block_to_del->next;
        } else {
            reinterpret_cast<block_metadata*>(block_to_del->prev)->next = block_to_del->next;
        }
        if (block_to_del->next != nullptr) {
            reinterpret_cast<block_metadata*>(block_to_del->next)->prev = block_to_del->prev;
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

std::pair<std::string, size_t> allocator_boundary_tags::format_blocks_info() {
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

inline void allocator_boundary_tags::set_fit_mode(
    allocator_with_fit_mode::fit_mode mode)
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    if (meta->logger) meta->logger->debug("set_fit_mode - begin");
    meta->mutex.lock();
    meta->fit_mode = mode;
    meta->mutex.unlock();
    if (meta->logger) meta->logger->debug("set_fit_mode - end");
}


std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info() const
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    if (meta->logger) meta->logger->debug("get_blocks_info - begin");
    meta->mutex.lock();
    std::vector<allocator_test_utils::block_info> res = get_blocks_info_inner();
    meta->mutex.unlock();
    if (meta->logger) meta->logger->debug("get_blocks_info - end");
    return res;
}

inline logger *allocator_boundary_tags::get_logger() const
{
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);
    return meta->logger;
}

inline std::string allocator_boundary_tags::get_typename() const noexcept
{
    throw not_implemented("inline std::string allocator_boundary_tags::get_typename() const noexcept", "your code should be here...");
}


std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info_inner() const
{
    std::vector<block_info> blocks;
    global_metadata* meta = reinterpret_cast<global_metadata*>(_trusted_memory);

    if (meta->first_block == nullptr) {
        blocks.push_back({meta->space_size - allocator_metadata_size, false});
        return blocks;
    }

    block_metadata* cur_block_meta = reinterpret_cast<block_metadata*>(meta->first_block);
    size_t empty_space = (reinterpret_cast<char*>(cur_block_meta) - reinterpret_cast<char*>(_trusted_memory)) - allocator_metadata_size;


    while (cur_block_meta->next != nullptr) {
        if (empty_space > 0) {
            blocks.push_back({empty_space, false});
        }

        blocks.push_back({cur_block_meta->block_size + occupied_block_metadata_size, true});

        empty_space = (reinterpret_cast<char*>(cur_block_meta->next) - reinterpret_cast<char*>(cur_block_meta)) - occupied_block_metadata_size - cur_block_meta->block_size;
        cur_block_meta = reinterpret_cast<block_metadata*>(cur_block_meta->next);
    }

    if (empty_space > 0) {
        blocks.push_back({empty_space, false});
    }

    blocks.push_back({cur_block_meta->block_size + occupied_block_metadata_size, true});

    empty_space = ((reinterpret_cast<char*>(_trusted_memory) + meta->space_size) - reinterpret_cast<char*>(cur_block_meta)) - occupied_block_metadata_size - cur_block_meta->block_size;

    if (empty_space > 0) {
        blocks.push_back({empty_space, false});
    }

    return blocks;
}

bool allocator_boundary_tags::do_is_equal(const std::pmr::memory_resource &other) const noexcept
{
    return dynamic_cast<const allocator_boundary_tags*>(&other)->_trusted_memory == _trusted_memory;
}
