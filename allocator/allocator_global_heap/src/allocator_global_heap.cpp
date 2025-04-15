#include <not_implemented.h>
#include "../include/allocator_global_heap.h"

allocator_global_heap::allocator_global_heap(
    logger *logger)
{

    if (_logger)
        _logger->trace("allocator_global_heap::allocator_global_heap - begin");

    _logger = logger;

    if (_logger)
        _logger->trace("allocator_global_heap::allocator_global_heap - end");
}

[[nodiscard]] void *allocator_global_heap::do_allocate_sm(
    size_t size)
{
    if (_logger)
        _logger->debug("allocator_global_heap::do_allocate_sm - begin");

    try {
        void *ptr = ::operator new(size);
        if (_logger)
            _logger->debug("allocator_global_heap::do_allocate_sm - end");
        return ptr;
    }
    catch (const std::bad_alloc &) {
        if (_logger)
            _logger->error("allocator_global_heap::do_allocate_sm - std::bad_alloc thrown");
        throw;
    }
}

void allocator_global_heap::do_deallocate_sm(
    void *at)
{
    if (_logger)
        _logger->debug("allocator_global_heap::do_deallocate_sm - begin");

    ::operator delete(at);

    if (_logger)
        _logger->debug("allocator_global_heap::do_deallocate_sm - end");
}

inline logger *allocator_global_heap::get_logger() const
{
    return _logger;
}

inline std::string allocator_global_heap::get_typename() const
{
    throw not_implemented("inline std::string allocator_global_heap::get_typename() const", "your code should be here...");
}

allocator_global_heap::~allocator_global_heap()
{
    if (_logger)
        _logger->trace("allocator_global_heap::~allocator_global_heap - begin");

    if (_logger)
        _logger->trace("allocator_global_heap::~allocator_global_heap - end");
}

allocator_global_heap::allocator_global_heap(const allocator_global_heap &other)
{
    if (_logger)
        _logger->trace("allocator_global_heap::copy_ctor - begin");

    _logger = other._logger;

    if (_logger)
        _logger->trace("allocator_global_heap::copy_ctor - end");
}

allocator_global_heap &allocator_global_heap::operator=(const allocator_global_heap &other)
{
    if (_logger)
        _logger->trace("allocator_global_heap::copy_assign - begin");

    if (this != &other)
        _logger = other._logger;

    if (_logger)
        _logger->trace("allocator_global_heap::copy_assign - end");

    return *this;
}

bool allocator_global_heap::do_is_equal(const std::pmr::memory_resource &other) const noexcept
{
    if (_logger)
        _logger->trace("allocator_global_heap::do_is_equal - begin");

    bool result = (this == &other);

    if (_logger)
        _logger->trace("allocator_global_heap::do_is_equal - end");

    return result;
}

allocator_global_heap::allocator_global_heap(allocator_global_heap &&other) noexcept
{
    if (_logger)
        _logger->trace("allocator_global_heap::move_ctor - begin");

    _logger = other._logger;
    other._logger = nullptr;

    if (_logger)
        _logger->trace("allocator_global_heap::move_ctor - end");
}

allocator_global_heap &allocator_global_heap::operator=(allocator_global_heap &&other) noexcept
{
    if (_logger)
        _logger->trace("allocator_global_heap::move_assign - begin");

    if (this != &other) {
        _logger = other._logger;
        other._logger = nullptr;
    }

    if (_logger)
        _logger->trace("allocator_global_heap::move_assign - end");

    return *this;
}
