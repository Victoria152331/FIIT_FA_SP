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
    auto res = *this;
    res &= other;
    return res;
}

big_int big_int::operator|(const big_int &other) const
{
    auto res = *this;
    res &= other;
    return res;
}

big_int big_int::operator^(const big_int &other) const
{
    auto res = *this;
    res ^= other;
    return res;
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
    auto res = *this;
    for (size_t i = 0; i < res._digits.size(); i++) {
        res._digits[i] = ~res._digits[i];
    }
    return res;
}

big_int &big_int::operator&=(const big_int &other) &
{
    size_t this_size = _digits.size();
    size_t other_size = other._digits.size();

    if (this_size > other_size) {
        this_size = other_size;
        _digits.erase(_digits.begin() + other_size, _digits.end());
    }

    for (size_t i; i < this_size; i++) {
        _digits[i] &= other._digits[i];
    }
    return *this;
}

big_int &big_int::operator|=(const big_int &other) &
{
    size_t this_size = _digits.size();
    size_t other_size = other._digits.size();
    
    if (this_size < other_size) {
        other_size = this_size;
        _digits.insert(_digits.end(), other._digits.begin() + this_size, other._digits.end());
    }

    for (size_t i; i < other_size; i++) {
        _digits[i] |= other._digits[i];
    }
    return *this;
}

big_int &big_int::operator^=(const big_int &other) &
{
    size_t this_size = _digits.size();
    size_t other_size = other._digits.size();
    
    if (this_size < other_size) {
        other_size = this_size;
        _digits.insert(_digits.end(), other._digits.begin() + this_size, other._digits.end());
    }

    for (size_t i; i < this_size; i++) {
        _digits[i] ^= other._digits[i];
    }
    return *this;
}

big_int &big_int::operator<<=(size_t shift) &
{
    size_t big_shift = shift / 8;
    size_t little_shift = shift % 8;
    if (big_shift > 0) {
        auto it = _digits.cbegin();
        _digits.insert(it, big_shift, 0);
    }
    if (little_shift == 0) {
        return *this;
    }
    unsigned int mask = ((1 << little_shift) - 1) << (8 - little_shift);
    unsigned int buf1, buf2 = 0;
    for (size_t i = big_shift; i < _digits.size(); i++) {
        buf1 = _digits[i] & mask;
        _digits[i] = (_digits[i] << little_shift) | buf2;
        buf2 = buf1 >> little_shift;
    }
    if (buf2) {
        _digits.push_back(buf2);
    }
    return *this;
}

big_int &big_int::operator>>=(size_t shift) &
{
    size_t big_shift = shift / 8;
    size_t little_shift = shift % 8;
    if (big_shift > 0) {
        auto it = _digits.cbegin();
        _digits.erase(it, it + big_shift - 1);
    }
    if (little_shift == 0) {
        return *this;
    }
    unsigned int mask = (1 << little_shift) - 1;
    unsigned int buf1, buf2 = 0;
    for (size_t i = _digits.size() - 1; i <= big_shift; i--) {
        buf1 = _digits[i] & mask;
        _digits[i] = (_digits[i] >> little_shift) | buf2;
        buf2 = buf1 << (8 - little_shift);
    }
    if (_digits[_digits.size() - 1] == 0) {
        _digits.pop_back();
    }
    return *this;
}

big_int &big_int::plus_assign(const big_int &other, size_t shift) &
{
    auto summand = other << (shift * 8);
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
        return *this;
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
            this->_digits.push_back(summand._digits[i] + acc);
            acc = 0;
        } else {
            buf = (_digits[i] & 1) + (summand._digits[i] & 1);
            acc = buf > 1;
            _digits[i] >>= 1;
            summand._digits[i] >>= 1;
            _digits[i] += summand._digits[i] + acc;
            acc = (_digits[i] & (1 << (sizeof(unsigned int) * 8 - 1)));
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
    auto sub = other << (shift * 8);
    if (!(*this)) {
        *this = sub;
        this->_sign = !(this->_sign);
        return *this;
    }
    if (!sub) {
        return *this;
    }

    if (this->_sign > sub._sign) { // pos - neg
        sub._sign = true;
        *this += sub;
        return *this;
    } if (this->_sign < sub._sign) { // neg - pos
        sub._sign = false;
        *this += sub;
        return *this;
    }

    auto cmp = (*this) <=> sub;
    if (cmp == 0) {
        _sign = 1;
        _digits.clear();
        _digits.push_back(0);
        return *this;
    }
    if (cmp < 0) {
        auto tmp = *this;
        *this = sub;
        sub = tmp;
        _sign = !_sign;
    }

    bool acc = false;
    unsigned int sub_item;
    size_t i = 0;
    for (; i < sub._digits.size(); i++) {
        sub_item = sub._digits[i] + acc;
        acc = _digits[i] < sub_item;
        _digits[i] -= sub_item;
    }
    while(acc) {
        acc = _digits[i] == 0;
        _digits[i] -= 1;
    }
    return *this;
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