#include <not_implemented.h>
#include "../include/allocator_boundary_tags.h"

allocator_boundary_tags::~allocator_boundary_tags()
{
    allocator_boundary_tags::global_metadata* meta = reinterpret_cast<allocator_boundary_tags::global_metadata*>(_trusted_memory);
    meta->parent_allocator->deallocate(_trusted_memory, meta->space_size);
}

allocator_boundary_tags::allocator_boundary_tags(
    allocator_boundary_tags &&other) noexcept
{
    throw not_implemented("allocator_boundary_tags::allocator_boundary_tags(allocator_boundary_tags &&) noexcept", "your code should be here...");
}

allocator_boundary_tags &allocator_boundary_tags::operator=(
    allocator_boundary_tags &&other) noexcept
{
    throw not_implemented("allocator_boundary_tags &allocator_boundary_tags::operator=(allocator_boundary_tags &&) noexcept", "your code should be here...");
}


/** If parent_allocator* == nullptr you should use std::pmr::get_default_resource()
 */
allocator_boundary_tags::allocator_boundary_tags(
        size_t space_size,
        std::pmr::memory_resource *parent_allocator,
        logger *logger,
        allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    try {
        if (parent_allocator == nullptr) {
            _trusted_memory = std::pmr::get_default_resource()->allocate(space_size);
        } else {
            _trusted_memory = parent_allocator->allocate(space_size);
        }
    }
    catch (const std::bad_alloc &) {
        if (logger)
            logger->error("allocator_boundary_tags::allocator_boundary_tags - std::bad_alloc thrown");
        throw;
    }

    allocator_boundary_tags::global_metadata* meta = reinterpret_cast<allocator_boundary_tags::global_metadata*>(_trusted_memory);
    meta->logger = logger;
    meta->fit_mode = allocate_fit_mode;
    meta->parent_allocator = parent_allocator;
    meta->space_size = space_size;
    new(&(meta->mutex)) std::mutex;
    meta->first_block = reinterpret_cast<void*>(reinterpret_cast<char*>(_trusted_memory) + allocator_metadata_size);
}

[[nodiscard]] void *allocator_boundary_tags::do_allocate_sm(
    size_t size)
{
    throw not_implemented("[[nodiscard]] void *allocator_boundary_tags::do_allocate_sm(size_t)", "your code should be here...");
}

void allocator_boundary_tags::do_deallocate_sm(
    void *at)
{
    throw not_implemented("void allocator_boundary_tags::do_deallocate_sm(void *)", "your code should be here...");
}

inline void allocator_boundary_tags::set_fit_mode(
    allocator_with_fit_mode::fit_mode mode)
{
    allocator_boundary_tags::global_metadata* meta = reinterpret_cast<allocator_boundary_tags::global_metadata*>(_trusted_memory);
    meta->fit_mode = mode;
}


std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info() const
{
    throw not_implemented("std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info() const", "your code should be here...");
}

inline logger *allocator_boundary_tags::get_logger() const
{
    throw not_implemented("inline logger *allocator_boundary_tags::get_logger() const", "your code should be here...");
}

inline std::string allocator_boundary_tags::get_typename() const noexcept
{
    throw not_implemented("inline std::string allocator_boundary_tags::get_typename() const noexcept", "your code should be here...");
}


allocator_boundary_tags::boundary_iterator allocator_boundary_tags::begin() const noexcept
{
    throw not_implemented("allocator_boundary_tags::boundary_iterator allocator_boundary_tags::begin() const noexcept", "your code should be here...");
}

allocator_boundary_tags::boundary_iterator allocator_boundary_tags::end() const noexcept
{
    throw not_implemented("allocator_boundary_tags::boundary_iterator allocator_boundary_tags::end() const noexcept", "your code should be here...");
}

std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info_inner() const
{
    throw not_implemented("std::vector<allocator_test_utils::block_info> allocator_boundary_tags::get_blocks_info_inner() const", "your code should be here...");
}

allocator_boundary_tags::allocator_boundary_tags(const allocator_boundary_tags &other)
{
    throw not_implemented("allocator_boundary_tags::allocator_boundary_tags(const allocator_boundary_tags &other)", "your code should be here...");
}

allocator_boundary_tags &allocator_boundary_tags::operator=(const allocator_boundary_tags &other)
{
    throw not_implemented("allocator_boundary_tags &allocator_boundary_tags::operator=(const allocator_boundary_tags &other)", "your code should be here...");
}

bool allocator_boundary_tags::do_is_equal(const std::pmr::memory_resource &other) const noexcept
{
    throw not_implemented("bool allocator_boundary_tags::do_is_equal(const std::pmr::memory_resource &other) const noexcept", "your code should be here...");
}

bool allocator_boundary_tags::boundary_iterator::operator==(
        const allocator_boundary_tags::boundary_iterator &other) const noexcept
{
    throw not_implemented("bool allocator_boundary_tags::boundary_iterator::operator==(const allocator_boundary_tags::boundary_iterator &) const noexcept", "your code should be here...");
}

bool allocator_boundary_tags::boundary_iterator::operator!=(
        const allocator_boundary_tags::boundary_iterator & other) const noexcept
{
    throw not_implemented("bool allocator_boundary_tags::boundary_iterator::operator!=(const allocator_boundary_tags::boundary_iterator &) const noexcept", "your code should be here...");
}

allocator_boundary_tags::boundary_iterator &allocator_boundary_tags::boundary_iterator::operator++() & noexcept
{
    throw not_implemented("allocator_boundary_tags::boundary_iterator &allocator_boundary_tags::boundary_iterator::operator++() & noexcept", "your code should be here...");
}

allocator_boundary_tags::boundary_iterator &allocator_boundary_tags::boundary_iterator::operator--() & noexcept
{
    throw not_implemented("allocator_boundary_tags::boundary_iterator &allocator_boundary_tags::boundary_iterator::operator--() & noexcept", "your code should be here...");
}

allocator_boundary_tags::boundary_iterator allocator_boundary_tags::boundary_iterator::operator++(int n)
{
    throw not_implemented("allocator_boundary_tags::boundary_iterator allocator_boundary_tags::boundary_iterator::operator++(int n)", "your code should be here...");
}

allocator_boundary_tags::boundary_iterator allocator_boundary_tags::boundary_iterator::operator--(int n)
{
    throw not_implemented("allocator_boundary_tags::boundary_iterator allocator_boundary_tags::boundary_iterator::operator--(int n)", "your code should be here...");
}

size_t allocator_boundary_tags::boundary_iterator::size() const noexcept
{
    throw not_implemented("size_t allocator_boundary_tags::boundary_iterator::size() const noexcept", "your code should be here...");
}

bool allocator_boundary_tags::boundary_iterator::occupied() const noexcept
{
    throw not_implemented("bool allocator_boundary_tags::boundary_iterator::occupied() const noexcept", "your code should be here...");
}

void* allocator_boundary_tags::boundary_iterator::operator*() const noexcept
{
    throw not_implemented("void* allocator_boundary_tags::boundary_iterator::operator*() const noexcept", "your code should be here...");
}

allocator_boundary_tags::boundary_iterator::boundary_iterator()
{
    throw not_implemented("allocator_boundary_tags::boundary_iterator::boundary_iterator()", "your code should be here...");
}

allocator_boundary_tags::boundary_iterator::boundary_iterator(void *trusted)
{
    throw not_implemented("allocator_boundary_tags::boundary_iterator::boundary_iterator(void *)", "your code should be here...");
}

void *allocator_boundary_tags::boundary_iterator::get_ptr() const noexcept
{
    throw not_implemented("void *allocator_boundary_tags::boundary_iterator::get_ptr() const noexcept", "your code should be here...");
}
