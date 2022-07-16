#include <algorithm>
#include <array>
#include <emmintrin.h>
#include <immintrin.h>
#include <iostream>
#include <numeric>
#include <vectorclass.h>
#include <vectormath_trig.h>

int main()
{
    Vec4f a{ 1.f, 2.f, 3.f, 4.f };
    Vec4f b{ 0.3f, 0.5f, 0.6f, 0.f };

    Vec4f c = a + sin(b);

    for (int i = 0; i < c.size(); ++i) {
        std::cout << c[i] << ", ";
    }
    std::cout << "\n";
}
