#include <algorithm>
#include <array>
#include <emmintrin.h>
#include <immintrin.h>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>
#include <vectorclass.h>
#include <vectormath_trig.h>

// /O2 /arch:avx2
// -O2 -msse4 -std=c++17 -ftree-vectorize

#include <array>
#include <cmath>

constexpr int SIZE = 256;
using VEC = std::array<float, SIZE>;

void add_vec_size(const VEC& left, const VEC& right, VEC& result)
{
    Vec4f tmp;
    for (int i = 0; i < SIZE; i += tmp.size()) {
        Vec4f vleft(&left[i]);
        Vec4f vright(&right[i]);

        Vec4f vresult = vleft + std::sin(vright);
        vresult.store(result.data() + i);
    }
}

int main()
{
    for (int i = 0; i < c.size(); ++i) {
        std::cout << c[i] << ", ";
    }
    std::cout << "\n";
}
