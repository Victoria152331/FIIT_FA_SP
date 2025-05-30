#include <iostream>
#include <cmath>
#include "fraction.h"


int main() {
    fraction x(1, 3);
    fraction res;               

    std::cout << "x              = " << x.to_string() << " ≈ " << x.to_double() << "\n";
    res = x.sin();
    std::cout << "sin (fraction) = " << res << " ≈ " << res.to_double() << "\n";
    std::cout << "sin (std)      = " << std::sin(x.to_double()) << "\n\n";
    //std::cout << "difference     = " << std::abs(approx - actual) << "\n";
    res = x.cos();
    std::cout << "cos (fraction) = " << res << " ≈ " << res.to_double() << "\n";
    std::cout << "cos (std)      = " << std::cos(x.to_double()) << "\n\n";
    res = x.arcsin();
    std::cout << "arcsin (fraction) = " << res << " ≈ " << res.to_double() << "\n";
    std::cout << "arcsin (std)      = " << std::asin(x.to_double()) << "\n\n";
    res = x.arctg();
    std::cout << "arctan (fraction) = " << res << " ≈ " << res.to_double() << "\n";
    std::cout << "arctan (std)      = " << std::atan(x.to_double()) << "\n\n";
    res = x.arccos();
    std::cout << "arccos (fraction) = " << res << " ≈ " << res.to_double() << "\n";
    std::cout << "arccos (std)      = " << std::acos(x.to_double()) << "\n\n";
    res = x.arcctg();
    std::cout << "arcot (fraction) = " << res << " ≈ " << res.to_double() << "\n";
    std::cout << "arccot (std)      = " << M_PI / 2 - std::atan(x.to_double()) << "\n\n";
    res = x.root(3);
    std::cout << "root3 (fraction) = " << res << " ≈ " << res.to_double() << "\n";
    std::cout << "root3 (std)      = " << std::pow(x.to_double(), 1./3.) << "\n\n";

    

    res = x.ln();
    std::cout << "ln (fraction) = " << res << " ≈ " << res.to_double() << "\n";
    std::cout << "ln (std)      = " << std::log(x.to_double()) << "\n\n";

    res = x.lg();
    std::cout << "log10 (fraction) = " << res << " ≈ " << res.to_double() << "\n";
    std::cout << "log10 (std)      = " << std::log10(x.to_double()) << "\n\n";

    res = x.log2();
    std::cout << "log2 (fraction) = " << res << " ≈ " << res.to_double() << "\n";
    std::cout << "log2 (std)      = " << std::log2(x.to_double()) << "\n\n";

    std::cout << fraction::approx_e().to_double() << std::endl;

    return 0;
}
