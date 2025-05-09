//
// Created by Des Caldnd on 5/27/2024.
//

#include "../include/big_int.h"
#include <ranges>
#include <exception>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>

std::strong_ordering big_int::operator<=>(const big_int &other) const noexcept
{
    if (!(*this)) {
        if (other._sign) {
            return std::strong_ordering::less; // 0 < positive
        } else {
            return std::strong_ordering::greater; // 0 > negative
        }
    } else if (!other) {
        if (this->_sign) {
            return std::strong_ordering::greater; // positive > 0
        } else {
            return std::strong_ordering::less; // negative < 0
        }
    } else if (other._sign != this->_sign) {
        if (this->_sign) {
            return std::strong_ordering::greater; // positive > negative
        } else {
            return std::strong_ordering::less; // negative < positive
        }
    } else if (this->_digits.size() > other._digits.size()) {
        return std::strong_ordering::greater;
    } else if (this->_digits.size() < other._digits.size()) {
        return std::strong_ordering::less;
    }

    for (size_t i = this->_digits.size() - 1; i >= 0; i--) {
        if (this->_digits[i] > other._digits[i]) {
            return std::strong_ordering::greater;
        } else if (this->_digits[i] < other._digits[i]) {
            return std::strong_ordering::less;
        }
    }
    return std::strong_ordering::equal;
}

bool big_int::operator==(const big_int &other) const noexcept
{
    return ((*this) <=> other) == 0;
}

big_int::operator bool() const noexcept
{
    if ((_digits.size() == 1) && (_digits[0] == 0)) {
        return false;
    }
    return true;
}

big_int &big_int::operator++() &
{
    if (!(*this)) {
        _digits[0] = 1;
        _sign = false;
        return *this;
    }

    if (!_sign) {
        _sign = true;
        --(*this);
        _sign = false;
    }

    auto it = _digits.begin();
    for (; it != _digits.end(); it++) {
        ++(*it);
        if (*it != 0) {
            break;
        }
    }

    if (it == _digits.end()) {
        _digits.push_back(1);
    }

    return *this;
}


big_int big_int::operator++(int)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

big_int &big_int::operator--() &
{
    if (!(*this)) {
        _digits[0] = 1;
        _sign = true;
        return (*this);
    }

    if (!_sign) {
        _sign = true;
        ++(*this);
        _sign = false;
    }

    auto it = _digits.begin();
    for (; it != _digits.end(); it++) {
        --(*it);
        if (*it != -1) {
            break;
        }
    }

    return *this;
}


big_int big_int::operator--(int)
{
    auto tmp = *this;
    --(*this);
    return tmp;
}

big_int &big_int::operator+=(const big_int &other) &
{
    this->plus_assign(other);
    return *this;
}

big_int &big_int::operator-=(const big_int &other) &
{
    this->minus_assign(other);
    return *this;
}

big_int big_int::operator+(const big_int &other) const
{
    big_int res = (*this);
    res.plus_assign(other);
    return res;
}

big_int big_int::operator-(const big_int &other) const
{
    big_int res = (*this);
    res.minus_assign(other);
    return res;
}

big_int big_int::operator*(const big_int &other) const
{
    auto rule = decide_mult(other._digits.size());
    big_int res = (*this);
    res.multiply_assign(other, rule);
    return res;
}

big_int big_int::operator/(const big_int &other) const
{
    auto rule = decide_div(other._digits.size());
    big_int res = (*this);
    res.divide_assign(other, rule);
    return res;
}

big_int big_int::operator%(const big_int &other) const
{
    auto rule = decide_div(other._digits.size());
    big_int res = (*this);
    res.modulo_assign(other, rule);
    return res;
}

big_int &big_int::operator*=(const big_int &other) &
{
    auto rule = decide_mult(other._digits.size());
    this->multiply_assign(other, rule);
    return *this;
}

big_int &big_int::operator/=(const big_int &other) &
{
    auto rule = decide_div(other._digits.size());
    this->divide_assign(other, rule);
    return *this;
}

big_int &big_int::operator%=(const big_int &other) &
{
    auto rule = decide_div(other._digits.size());
    this->divide_assign(other, rule);
    return *this;
}

big_int big_int::operator&(const big_int &other) const
{
    throw not_implemented("big_int big_int::operator&(const big_int &) const", "your code should be here...");
}

big_int big_int::operator|(const big_int &other) const
{
    throw not_implemented("big_int big_int::operator|(const big_int &) const", "your code should be here...");
}

big_int big_int::operator^(const big_int &other) const
{
    throw not_implemented("big_int big_int::operator^(const big_int &) const", "your code should be here...");
}

big_int big_int::operator<<(size_t shift) const
{
    auto res = *this;
    res <<= shift;
    return res;
}

big_int big_int::operator>>(size_t shift) const
{
    auto res = *this;
    res >>= shift;
    return res;
}

big_int big_int::operator~() const
{
    throw not_implemented("big_int big_int::operator~() const", "your code should be here...");
}

big_int &big_int::operator&=(const big_int &other) &
{
    throw not_implemented("big_int &big_int::operator&=(const big_int &)", "your code should be here...");
}

big_int &big_int::operator|=(const big_int &other) &
{
    throw not_implemented("big_int &big_int::operator|=(const big_int &)", "your code should be here...");
}

big_int &big_int::operator^=(const big_int &other) &
{
    throw not_implemented("big_int &big_int::operator^=(const big_int &)", "your code should be here...");
}

big_int &big_int::operator<<=(size_t shift) &
{
    if (shift > 0) {
        auto it = _digits.cbegin();
        _digits.insert(it, shift, 0);
    }
    return *this;
}

big_int &big_int::operator>>=(size_t shift) &
{
    if (shift > 0) {
        auto it = _digits.cbegin();
        _digits.erase(it, it + shift - 1);
    }
    return *this;
}

big_int &big_int::plus_assign(const big_int &other, size_t shift) &
{
    auto summand = other << shift;
    if (!(*this)) {
        *this = summand;
        return *this;
    } 
    if (!summand) {
        return *this;
    }

    if (this->_sign > summand._sign) { // pos + neg
        summand._sign = true;
        *this -= summand;
        return *this
    } if (this->_sign < summand._sign) { // neg + pos
        summand._sign = false;
        *this -= summand;
        return *this;
    }

    bool acc = 0;
    unsigned int buf;
    size_t num_size = this->_digits.size();
    size_t i = 0;
    for (; i < summand._digits.size(); i++) {
        if (i >= num_size) {
            this->_digits.push_back(summand[i] + acc);
            acc = 0;
        } else {
            buf = (_digits[i] & 1) + (summand._digits[i] & 1);
            acc = buf > 1;
            _digits[i] >>= 1;
            summand._digits[i] >>= 1;
            _digits[i] += summand._digits[i] + acc;
            acc = (_digits[i] & (1 << (sizeof(unsigned int) * 8 - 1)))
            _digits[i] <<= 1;
            _digits[i] += buf;
        }
    }
    while (acc) {
        if (i >= num_size) {
            this->_digits.push_back(acc);
            acc = 0;
        } else if (_digits[i] != -1) {
            _digits[i] += acc;
            acc = 0;
        } else {
            _digits[i] = 0;
            i++;
        }
    }
    return *this;
}

big_int &big_int::minus_assign(const big_int &other, size_t shift) &
{
    throw not_implemented("big_int &big_int::minus_assign(const big_int &, size_t)", "your code should be here...");
}


std::string big_int::to_string() const
{
    throw not_implemented("std::string big_int::to_string() const", "your code should be here...");
}

std::ostream &operator<<(std::ostream &stream, const big_int &value)
{
    throw not_implemented("std::ostream &operator<<(std::ostream &, const big_int &)", "your code should be here...");
}

std::istream &operator>>(std::istream &stream, big_int &value)
{
    throw not_implemented("std::istream &operator>>(std::istream &, big_int &)", "your code should be here...");
}

big_int::big_int(const std::vector<unsigned int, pp_allocator<unsigned int>> &digits, bool sign)
    : _sign(sign)
    , _digits(digits)
{}

big_int::big_int(std::vector<unsigned int, pp_allocator<unsigned int>> &&digits, bool sign) noexcept
    : _sign(sign)
    , _digits(digits)
{}

big_int::big_int(const std::string &num, unsigned int radix, pp_allocator<unsigned int>)
{
    throw not_implemented("big_int::big_int(const std::string &num, unsigned int radix, pp_allocator<unsigned int>)", "your code should be here...");
}

big_int::big_int(pp_allocator<unsigned int>)
{
    throw not_implemented("big_int::big_int(pp_allocator<unsigned int>)", "your code should be here...");
}

big_int::multiplication_rule big_int::decide_mult(size_t rhs) const noexcept {
    size_t max_size = std::max(rhs, this->_digits.size());
    if (max_size < 10) {
        return big_int::multiplication_rule::trivial;
    } else if (max_size < 1000) {
        return big_int::multiplication_rule::Karatsuba;
    } else {
        return big_int::multiplication_rule::SchonhageStrassen;
    }
    
}

big_int::division_rule big_int::decide_div(size_t rhs) const noexcept {
    size_t max_size = std::max(rhs, this->_digits.size());
    if (max_size < 10) {
        return big_int::division_rule::trivial;
    } else if (max_size < 1000) {
        return big_int::division_rule::Newton;
    } else {
        return big_int::division_rule::BurnikelZiegler;
    }
}

big_int &big_int::multiply_assign(const big_int &other, big_int::multiplication_rule rule) &
{
    throw not_implemented("big_int &big_int::multiply_assign(const big_int &other, big_int::multiplication_rule rule) &", "your code should be here...");
}

big_int &big_int::divide_assign(const big_int &other, big_int::division_rule rule) &
{
    throw not_implemented("big_int &big_int::divide_assign(const big_int &other, big_int::division_rule rule) &", "your code should be here...");
}

big_int &big_int::modulo_assign(const big_int &other, big_int::division_rule rule) &
{
    throw not_implemented("big_int &big_int::modulo_assign(const big_int &other, big_int::division_rule rule) &", "your code should be here...");
}

big_int operator""_bi(unsigned long long n)
{
    throw not_implemented("big_int operator\"\"_bi(unsigned long long n)", "your code should be here...");
}