#include "../include/fraction.h"

void fraction::optimise()
{
    if (_numerator == big_int(0)) {
        _denominator = big_int(1);
        return;
    }

    big_int a = _numerator;
    if (_numerator < big_int(0)) {
        a = big_int(0) - _numerator;
    }
    big_int b = _denominator;
    if (_denominator < big_int(0)) {
        b = big_int(0) - _denominator
    }

    while (b != big_int(0)) {
        big_int r = a % b;
        a = b;
        b = r;
    }
    
    _numerator   /= a;
    _denominator /= a;

    if (_denominator < big_int(0)) {
        _numerator   = big_int(0) - _numerator;
        _denominator = big_int(0) - _denominator;
    }

}

template<std::convertible_to<big_int> f, std::convertible_to<big_int> s>
fraction::fraction(f &&numerator, s &&denominator)
    : _numerator(numerator)
    , _denominator(denominator)
{
    optimise();
}

fraction::fraction(pp_allocator<big_int::value_type> allocator)
    : _numerator(big_int(0, allocator))
    , _denominator(big_int(1, allocator))
{}

fraction &fraction::operator+=(fraction const &other) &
{
    _numerator = (_numerator * other._denominator + other._numerator * _denominator);
    _denominator *= other._denominator;
    optimise();
    return *this;
}

fraction fraction::operator+(fraction const &other) const
{
    auto tmp = *this;
    tmp+-= other;
    return tmp;
}

fraction &fraction::operator-=(fraction const &other) &
{
    _numerator = (_numerator * other._denominator - other._numerator * _denominator);
    _denominator *= other._denominator;
    optimise();
    return *this;
}

fraction fraction::operator-(fraction const &other) const
{
    auto tmp = *this;
    tmp -= other;
    return tmp;
}

fraction &fraction::operator*=(fraction const &other) &
{
    _denominator *= other._denominator;
    _numerator *= other._numerator;
    optimise();
    return *this;
}

fraction fraction::operator*(fraction const &other) const
{
    fraction res = *this;
    res *= other;
    return res;
}

fraction &fraction::operator/=(fraction const &other) &
{
    _denominator *= other._numerator;
    _numerator *= other._denominator;
    optimise();
    return *this;
}

fraction fraction::operator/(fraction const &other) const
{
    fraction res = *this;
    res *= other;
    return res;
}

bool fraction::operator==(fraction const &other) const noexcept
{
    return (_numerator == other._numerator) && (_denominator == other._denominator)
}

std::partial_ordering fraction::operator<=>(const fraction& other) const noexcept
{
    return (_numerator * other._denominator) <=> (other_numerator * _denominator)
}

std::ostream &operator<<(std::ostream &stream, fraction const &obj)
{
    throw not_implemented("std::ostream &operator<<(std::ostream &, fraction const &)", "your code should be here...");
}

std::istream &operator>>(std::istream &stream, fraction &obj)
{
    throw not_implemented("std::istream &operator>>(std::istream &, fraction &)", "your code should be here...");
}

std::string fraction::to_string() const
{
    throw not_implemented("std::string fraction::to_string() const", "your code should be here...");
}

fraction fraction::sin(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::sin(fraction const &) const", "your code should be here...");
}

fraction fraction::cos(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::cos(fraction const &) const", "your code should be here...");
}

fraction fraction::tg(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::tg(fraction const &) const", "your code should be here...");
}

fraction fraction::ctg(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::ctg(fraction const &) const", "your code should be here...");
}

fraction fraction::sec(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::sec(fraction const &) const", "your code should be here...");
}

fraction fraction::cosec(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::cosec(fraction const &) const", "your code should be here...");
}

fraction fraction::pow(size_t degree) const
{
    throw not_implemented("fraction fraction::pow(size_t) const", "your code should be here...");
}

fraction fraction::root(size_t degree, fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::root(size_t , fraction const &) const", "your code should be here...");
}

fraction fraction::log2(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::log2(fraction const &) const", "your code should be here...");
}

fraction fraction::ln(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::ln(fraction const &) const", "your code should be here...");
}

fraction fraction::lg(fraction const &epsilon) const
{
    throw not_implemented("fraction fraction::lg(fraction const &) const", "your code should be here...");
}