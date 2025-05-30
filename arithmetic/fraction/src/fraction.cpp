#include "../include/fraction.h"
#include <sstream>

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
        b = big_int(0) - _denominator;
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
    tmp += other;
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

fraction fraction::operator - () const
{
    return fraction(-1, 1) * (*this);
}

fraction &fraction::operator/=(fraction const &other) &
{
    if (other._numerator == 0_bi) {
        throw std::logic_error("division by zero");
    }
    _denominator *= other._numerator;
    _numerator *= other._denominator;
    optimise();
    return *this;
}

fraction fraction::operator/(fraction const &other) const
{
    fraction res = *this;
    res /= other;
    return res;
}

bool fraction::operator==(fraction const &other) const noexcept
{
    return (_numerator == other._numerator) && (_denominator == other._denominator);
}

std::partial_ordering fraction::operator<=>(const fraction& other) const noexcept
{
    return (_numerator * other._denominator) <=> (other._numerator * _denominator);
}

std::ostream &operator<<(std::ostream &stream, fraction const &obj)
{
    stream << obj._numerator << "/" << obj._denominator;
    return stream;
}

std::istream &operator>>(std::istream &stream, fraction &obj)
{
    big_int num, den;
    stream >> num;

    char slash;
    stream >> slash;
    
    stream >> den;
    obj._numerator   = num;
    obj._denominator = den;
    obj.optimise();
    return stream;

}

std::string fraction::to_string() const
{
    std::stringstream stream;
    stream << (*this)._numerator << "/" << (*this)._denominator;
    return stream.str();
}

double fraction::to_double() const
{
    std::string s = (*this).to_string();
    auto pos = s.find('/');
    double num = std::stod(s.substr(0, pos));
    double den = std::stod(s.substr(pos + 1));
    return num / den;
}

fraction fraction::sin(fraction const &epsilon) const
{
    fraction x = *this;
    fraction sum = x;
    fraction term = x;
    size_t k = 1;
    while ((term > epsilon) || (term < -epsilon)) {
        term = term * fraction(-1, big_int(k + 1) * big_int(k + 2)) * x * x;
        sum += term;
        k += 2;
    }
    return sum;
}

fraction fraction::cos(fraction const &epsilon) const
{
    fraction x = *this;
    fraction sum = fraction(1, 1);
    fraction term = fraction(1, 1);
    size_t k = 0;
    while ((term > epsilon) || (term < -epsilon)) {
        term = term * fraction(-1, big_int(k + 1) * big_int(k + 2)) * x * x;
        sum += term;
        k += 2;
    }
    return sum;
}

fraction fraction::tg(fraction const &epsilon) const
{
    fraction buf = (*this).cos(epsilon);
    if ((buf < epsilon) && (buf > -epsilon)) {
        throw std::domain_error("tg: cos(x) == 0");
    }
    return (*this).sin(epsilon) / buf;
}

fraction fraction::ctg(fraction const &epsilon) const
{
    fraction buf = (*this).sin(epsilon);
    if ((buf < epsilon) && (buf > -epsilon)) {
        throw std::domain_error("ctg: sin(x) == 0");
    }
    return (*this).cos(epsilon) / (buf);
}

fraction fraction::sec(fraction const &epsilon) const
{
    fraction buf = (*this).cos(epsilon);
    if ((buf < epsilon) && (buf > -epsilon)) {
        throw std::domain_error("sec: cos(x) == 0");
    }
    return fraction(1, 1) / buf;
}

fraction fraction::cosec(fraction const &epsilon) const
{
    fraction buf = (*this).sin(epsilon);
    if ((buf < epsilon) && (buf > -epsilon)) {
        throw std::domain_error("cosec: sin(x) == 0");
    }
    return fraction(1, 1) / buf;
}

fraction fraction::arcsin(fraction const &epsilon) const
{
    if ((*this) > fraction(1, 1) + epsilon || (*this) < fraction(-1, 1) - epsilon) {
        throw std::domain_error("arcsin: |x| > 1");
    }
    fraction x = *this;
    fraction sum = x;
    fraction term = x;
    size_t k = 1;
    while ((term > epsilon) || (term < -epsilon)) {
        term = term * fraction(big_int(k), big_int(k + 1)) * x * x;
        sum += term * fraction(1, big_int(k + 2));
        k += 2;
    }
    return sum;
}

fraction fraction::arccos(fraction const &epsilon) const 
{
    if ((*this) > fraction(1, 1) + epsilon || (*this) < fraction(-1, 1) - epsilon) {
        throw std::domain_error("arccos: |x| > 1");
    }
    fraction half_pi = approx_pi(epsilon) / fraction(2, 1);
    return half_pi - arcsin(epsilon);
}

fraction fraction::arctg(fraction const &epsilon) const
{
    fraction x = *this;
    fraction sum = x;
    fraction term = x;
    fraction rec_term = x;
    size_t k = 3;
    while ((term > epsilon) || (term < -epsilon)) {
        rec_term = -rec_term * x * x;
        term = rec_term * fraction(1, k);
        sum += term;
        k += 2;
    }
    return sum;
}

fraction fraction::arcctg(fraction const &epsilon) const 
{
    fraction half_pi = approx_pi(epsilon) / fraction(2, 1);
    return half_pi - arctg(epsilon);
}

fraction fraction::arcsec(fraction const &epsilon) const 
{
    if (((*this) < fraction(1, 1) - epsilon) && ((*this) > fraction(-1, 1) + epsilon)) {
        throw std::domain_error("arcsec: |x| < 1");
    }
    fraction buf = fraction(1, 1) / (*this);
    return buf.arccos(epsilon);
}

fraction fraction::arccosec(fraction const &epsilon) const 
{
    if (((*this) < fraction(1, 1) - epsilon) && ((*this) > fraction(-1, 1) + epsilon)) {
        throw std::domain_error("arccosec: |x| < 1");
    }
    fraction buf = fraction(1, 1) / (*this);
    return buf.arcsin(epsilon);
}


fraction fraction::approx_pi(fraction const &epsilon)
{
    fraction epsA = epsilon / fraction(32, 1);  // 16·epsA < ε/2
    fraction epsB = epsilon / fraction(8, 1);   //  4·epsB < ε/2


    fraction A = fraction(1, 5).arctg(epsA);
    fraction B = fraction(1, 239).arctg(epsB);


    return A * fraction(16, 1) - B * fraction(4, 1);
}

fraction fraction::pow(size_t degree) const
{
    if (degree == 1) {
        return *this;
    }
    if (degree == 0) {
        return fraction(1, 1);
    }
    if (degree & 1) {
        fraction buf = *this;
        return buf * (*this).pow(degree - 1);
    } else {
        fraction buf = (*this).pow(degree >> 1);
        return buf * buf;
    }
}

fraction fraction::root(size_t degree, fraction const &epsilon) const
{
    if (degree == 0) {
        throw std::domain_error("root: degree == 0");
    }
    if ((degree & 1 == 0) && (*this < -epsilon)) {
        throw std::domain_error("root: even root of negative number");
    }
    // x[n + 1] = x[n] - f(x[n]) / f'(x[n])
    // f(x) = x ^ degree - (*this)
    fraction x (1, 1);
    fraction term (1, 1);
    while ((term > epsilon) || (term < -epsilon)) {
        term = (x.pow(degree) - (*this)) / (fraction(degree, 1) * x.pow(degree - 1));
        x = x - term;
    }
    return x;
}

fraction fraction::log2(fraction const &epsilon) const
{
    if ((*this) <= epsilon) {
        throw std::domain_error("log2: x <= 0");
    }
    return (*this).ln() / fraction(2, 1).ln();
}

fraction fraction::ln(fraction const &epsilon) const 
{
    if ((*this) <= epsilon) {
        throw std::domain_error("ln: x <= 0");
    }
    fraction t = (*this - fraction(1, 1)) / (*this + fraction(1, 1));
    fraction term = t;    // первый член: t^(2*0+1)/(2*0+1) = t
    fraction sum   = term;
    size_t k = 1;

    while ((term > epsilon) || (term < -epsilon)) {
        term = term * t * t;
        k += 2;
        sum += term * fraction(1, k);
        
    }
    return sum * fraction(2, 1);
}

fraction fraction::lg(fraction const &epsilon) const
{
    if ((*this) <= epsilon) {
        throw std::domain_error("lg: x <= 0");
    }
    return (*this).ln() / fraction(10, 1).ln();
}

fraction fraction::approx_e(fraction const &epsilon)
{
    fraction sum   = fraction(1, 1);
    fraction term  = fraction(1, 1);
    size_t k = 1;
    while (term > epsilon) {
        term = term / fraction(k, 1);
        sum += term;
        k++;
    }

    return sum;
}