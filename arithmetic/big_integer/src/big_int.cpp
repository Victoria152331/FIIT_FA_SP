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

#define BITS (sizeof(unsigned int) * 8)

std::strong_ordering big_int::operator<=>(const big_int &other) const noexcept
{
    if (!(*this) && !other) {
        return std::strong_ordering::equal; // 0 == 0
    }
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
        return this->_sign ? std::strong_ordering::greater : std::strong_ordering::less;
    } else if (this->_digits.size() < other._digits.size()) {
        return this->_sign ? std::strong_ordering::less : std::strong_ordering::greater;
    }

    for (size_t i = this->_digits.size(); i > 0; i--) {
        if (this->_digits[i - 1] > other._digits[i - 1]) {
            return this->_sign ? std::strong_ordering::greater : std::strong_ordering::less;
        } else if (this->_digits[i - 1] < other._digits[i - 1]) {
            return this->_sign ? std::strong_ordering::less : std::strong_ordering::greater;
        }
    }
    return std::strong_ordering::equal;
}

bool big_int::operator==(const big_int &other) const noexcept
{
    return ((*this) <=> other) == std::strong_ordering::equal;
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
    *this += big_int(1);
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
    *this -= big_int(1);
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
    this->modulo_assign(other, rule);
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
    res |= other;
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

    for (size_t i = 0; i < this_size; i++) {
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

    for (size_t i = 0; i < other_size; i++) {
        _digits[i] |= other._digits[i];
    }
    optimise();
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

    for (size_t i = 0; i < other_size; i++) {
        _digits[i] ^= other._digits[i];
    }
    optimise();
    return *this;
}

big_int &big_int::operator<<=(size_t shift) &
{
    size_t big_shift = shift / BITS;
    size_t little_shift = shift % BITS;
    if (big_shift > 0) {
        auto it = _digits.cbegin();
        _digits.insert(it, big_shift, 0);
    }
    if (little_shift == 0) {
        optimise();
        return *this;
    }
    unsigned int mask = ((1 << little_shift) - 1) << (BITS - little_shift);
    unsigned int buf1, buf2 = 0;
    for (size_t i = big_shift; i < _digits.size(); i++) {
        buf1 = _digits[i] & mask;
        _digits[i] = (_digits[i] << little_shift) | buf2;
        buf2 = buf1 >> little_shift;
    }
    if (buf2) {
        _digits.push_back(buf2);
    }
    optimise();
    return *this;
}

big_int &big_int::operator>>=(size_t shift) &
{
    size_t big_shift = shift / BITS;
    size_t little_shift = shift % BITS;
    if (big_shift > 0) {
        auto it = _digits.cbegin();
        _digits.erase(it, it + big_shift);
    }
    if (little_shift == 0) {
        return *this;
    }
    unsigned int mask = (1 << little_shift) - 1;
    unsigned int buf1, buf2 = 0;
    for (size_t i = _digits.size(); i > 0; i--) {
        buf1 = _digits[i - 1] & mask;
        _digits[i - 1] = (_digits[i - 1] >> little_shift) | buf2;
        buf2 = buf1 << (BITS - little_shift);
    }
    if (_digits[_digits.size() - 1] == 0) {
        _digits.pop_back();
    }
    optimise();
    return *this;
}

big_int &big_int::plus_assign(const big_int &other, size_t shift) &
{
    auto summand = other << (shift * BITS);
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
            buf = (_digits[i] & 1) + (summand._digits[i] & 1) + acc;
            acc = buf >> 1;
            _digits[i] >>= 1;
            _digits[i] += (summand._digits[i] >> 1) + acc;
            acc = _digits[i] >> (BITS - 1);
            _digits[i] <<= 1;
            _digits[i] += buf & 1;
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
    optimise();
    return *this;
}

big_int &big_int::minus_assign(const big_int &other, size_t shift) &
{
    auto sub = other << (shift * BITS);
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

    if ((*this) == sub) {
        _sign = 1;
        _digits.clear();
        _digits.push_back(0);
        return *this;
    }
    if ((*this) < sub) {
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
        i++;
    }
    optimise();
    return *this;
}


std::string big_int::to_string() const
{
    if (!(*this)) {
        return "0";
    }

    big_int tmp = *this;
    bool positive = tmp._sign;
    tmp._sign = true;

    std::string result;
    const big_int ten(10);

    while (tmp) {
        auto [div, mod] = full_division_trivial(tmp, ten);
        big_int digit = mod;
        result.push_back(static_cast<char>('0' + digit._digits[0]));
        tmp = div;
        
    }

    if (!positive) {
        result.push_back('-');
    }

    std::reverse(result.begin(), result.end());
    return result;

}

std::ostream &operator<<(std::ostream &stream, const big_int &value)
{
    stream << value.to_string();
    return stream;
}

std::istream &operator>>(std::istream &stream, big_int &value)
{
    std::string str;
    stream >> str;
    value = big_int(str);
    return stream;
}

big_int::big_int(const std::vector<unsigned int, pp_allocator<unsigned int>> &digits, bool sign)
    : _sign(sign)
    , _digits(digits.begin(), digits.end())
{
    optimise();
}

big_int::big_int(std::vector<unsigned int, pp_allocator<unsigned int>> &&digits, bool sign) noexcept
    : _sign(sign)
    , _digits(digits)
{
    optimise();
}

big_int::big_int(const std::string &num, unsigned int radix, pp_allocator<unsigned int> allocator)
    : _sign(true)
    , _digits(1, 0, allocator)
{
    if (num.empty()) {
        return;
    }

    std::size_t pos = 0;
    bool buf_sign = true;

    if (num[0] == '+') {
        pos++;
    } else if (num[0] == '-') {
        buf_sign = false;
        pos++;
    }

    for (; pos < num.size(); ++pos) {
        char c = num[pos];  
        unsigned int digit;

        if ((c >= '0') && (c <= '9')) {
            digit = c - '0';
        } else if ((c >= 'A') && (c <= 'Z')) {
            digit = c - 'A' + 10;
        } else if ((c >= 'a') && (c <= 'z')) {
            digit = c - 'a' + 10;
        } else {
            throw std::invalid_argument("invalid symb");
        }

        if (digit >= radix) {
            throw std::invalid_argument("digit greater than radix");
        }

        *this *= big_int(radix);
        *this += big_int(digit);
    }

    _sign = buf_sign;
    optimise();  // убираем ведущие нули
}

big_int::big_int(pp_allocator<unsigned int> allocator)
    : _sign(true)
    , _digits(1, 0, allocator)
{}

big_int::multiplication_rule big_int::decide_mult(size_t rhs) const noexcept {
    size_t max_size = std::max(rhs, this->_digits.size());
    if (max_size < 20) {
        return big_int::multiplication_rule::trivial;
    } else if (max_size < 1000) {
        return big_int::multiplication_rule::Karatsuba;
    } else {
        return big_int::multiplication_rule::SchonhageStrassen;
    }
    
}

big_int::division_rule big_int::decide_div(size_t rhs) const noexcept {
    size_t max_size = std::max(rhs, this->_digits.size());
    if (max_size < 20) {
        return big_int::division_rule::trivial;
    } else if (max_size < 1000) {
        return big_int::division_rule::Newton;
    } else {
        return big_int::division_rule::BurnikelZiegler;
    }
}

big_int &big_int::multiply_assign(const big_int &other, big_int::multiplication_rule rule) &
{
    if (rule == big_int::multiplication_rule::trivial) {
        if (!(*this) || !other) {
            *this = big_int(0);
            return *this;
        }
        _sign = (_sign == other._sign);
        big_int buf = *this;
        _digits.clear();
        _digits.push_back(0);
        big_int summand = *this;
        unsigned int m11, m12, m21, m22, mid, acc;
        unsigned int mask1 = (1 << (sizeof(unsigned int) * 4)) - 1;
        unsigned int mask2 = mask1 << (sizeof(unsigned int) * 4);
        for (size_t i = 0; i < other._digits.size(); i++) {
            for (size_t j = 0; j < buf._digits.size(); j++) {
                m11 = (buf._digits[j] & mask1) * (other._digits[i] & mask1);
                m12 = (buf._digits[j] & mask1) * ((other._digits[i] & mask2) >> sizeof(unsigned int) * 4);
                m21 = ((buf._digits[j] & mask2) >> sizeof(unsigned int) * 4) * (other._digits[i] & mask1);
                m22 = ((buf._digits[j] & mask2) >> sizeof(unsigned int) * 4) * ((other._digits[i] & mask2) >> sizeof(unsigned int) * 4);

                //std::cout << m11 << " " << m12 << " " << m21 << " " << m22 << std::endl;

                summand._digits[0] = m11 & mask1;
                acc = (m11 & mask2) >> sizeof(unsigned int) * 4;

                mid = (m12 & mask1) + (m21 & mask1) + acc;
                summand._digits[0] |= (mid & mask1) << sizeof(unsigned int) * 4;
                acc = ((mid & mask2) >> sizeof(unsigned int) * 4) 
                    + ((m12 & mask2) >> sizeof(unsigned int) * 4) 
                    + ((m21 & mask2) >> sizeof(unsigned int) * 4);
                
                summand._digits.push_back(m22 + acc);
                summand.optimise();
                //summand.debug_print();
                
                this->plus_assign(summand, i + j);
                summand._digits.clear();
                summand._digits.push_back(0);
            }
        }
    }
    else if (rule == multiplication_rule::Karatsuba) {
        // 1) ноль
        if (!(*this) || !other) {
            *this = big_int(0);
            return *this;
        }

        // 2) сохраним знак результата
        bool result_sign = (_sign == other._sign);

        // 3) абсолютные копии для разбиения и рекурсии
        big_int a = *this; a._sign = true;
        big_int b = other; b._sign = true;

        size_t n = std::max(a._digits.size(), b._digits.size());
        // 4) базовый случай — одна «цифра»
        if (n <= 1) {
            *this = a;           // восстановим a (одна цифра)
            _sign = true;        // знак + для тривиального
            multiply_assign(b, multiplication_rule::trivial);
            _sign = result_sign;
            optimise();
            return *this;
        }

        size_t m = n / 2;

        // 5) разделим a = high1·B^m + low1
        big_int low1, high1;
        low1._digits.assign(a._digits.begin(),
                            a._digits.begin() + std::min(a._digits.size(), m));
        high1._digits.assign(a._digits.begin() + std::min(a._digits.size(), m),
                             a._digits.end());
        low1.optimise(); high1.optimise();

        // 6) разделим b = high2·B^m + low2
        big_int low2, high2;
        low2._digits.assign(b._digits.begin(),
                            b._digits.begin() + std::min(b._digits.size(), m));
        high2._digits.assign(b._digits.begin() + std::min(b._digits.size(), m),
                             b._digits.end());
        low2.optimise(); high2.optimise();

        // 7) три рекурсивных произведения (все по Карацубе)
        big_int z0 = low1;   z0.multiply_assign(low2,  multiplication_rule::Karatsuba);
        big_int z2 = high1;  z2.multiply_assign(high2, multiplication_rule::Karatsuba);

        big_int sum1 = low1; sum1.plus_assign(high1);  // (low1+high1)
        big_int sum2 = low2; sum2.plus_assign(high2);  // (low2+high2)

        big_int z1 = sum1;
        z1.multiply_assign(sum2,  multiplication_rule::Karatsuba);
        z1.minus_assign(z2);
        z1.minus_assign(z0);

        // 8) соберём результат: z2·B^(2m) + z1·B^m + z0
        *this = z0;
        plus_assign(z1,   m);
        plus_assign(z2, 2*m);

        // 9) восстановим знак и оптимизируем
        _sign = result_sign;
    }


    optimise();
    return *this;
}

big_int &big_int::divide_assign(const big_int &other, big_int::division_rule rule) &
{
    if (rule == big_int::division_rule::trivial) {
        auto res = full_division_trivial(*this, other);
        *this = res.first;
    }
    optimise();
    return *this;
}

big_int &big_int::modulo_assign(const big_int &other, big_int::division_rule rule) &
{
    if (rule == big_int::division_rule::trivial) {
        auto res = full_division_trivial(*this, other);
        *this = res.second;
    }
    optimise();
    return *this;
}

std::pair<big_int, big_int> big_int::full_division_trivial(const big_int& _dividend, const big_int& _divisor) {
    if (!_divisor) {
        throw std::logic_error("division by zero");
    }
    big_int dividend = _dividend;
    big_int divisor = _divisor;
    dividend._sign = true;
    divisor._sign = true;
    if (dividend < divisor) {
        auto res = std::make_pair(big_int(0), dividend);
        return res;
    }
    size_t dividend_size = dividend._digits.size();
    size_t divisor_size = divisor._digits.size();
    big_int divisor_shifted = divisor << ((dividend_size - divisor_size) * BITS);
    big_int mod = dividend;
    big_int quotient = big_int(0);
    quotient._digits.resize(dividend_size - divisor_size + 1, 0);

    // std::cout << "hello " << std::endl;

    unsigned int low, high, mid;

    for (size_t k = dividend_size - divisor_size + 1; k > 0; k--) {
        low = 0;
        high = UINT_MAX;
        if (big_int(high) * divisor_shifted <= mod) {
            low = high;
        } else {
            while (low + 1 < high) {
                mid = low + (high - low) / 2;
                // std::cout << mid << " " << low << " " << high << std::endl;
                // auto sub = big_int(mid) * divisor_shifted;
                // sub.debug_print();
                if (big_int(mid) * divisor_shifted <= mod) {
                    low = mid;
                } else {
                    high = mid;
                }
            }
        }
        //std::cout << "low " << low << std::endl;
        quotient._digits[k - 1] = low;
        
        auto sub = big_int(low) * divisor_shifted;
        // std::cout << "sub " << low << std::endl;
        // sub.debug_print();
        mod -= sub;
        // std::cout << "mod " << std::endl;
        // mod.debug_print();
        divisor_shifted >>= BITS;
    }
    quotient._sign = (_dividend._sign == _divisor._sign);
    quotient.optimise();
    mod.optimise();
    auto res = std::make_pair(quotient, mod);
    return res;
}

big_int operator""_bi(unsigned long long n)
{
    throw not_implemented("big_int operator\"\"_bi(unsigned long long n)", "your code should be here...");
}

void big_int::optimise() {
    while ((_digits.size() > 1) && (_digits.back() == 0)) {
        _digits.pop_back();
    }
    if ((_digits.size() == 1) && (_digits[0] == 0)) {
        _sign = 1;
    }
}