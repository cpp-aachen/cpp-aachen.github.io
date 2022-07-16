#include <algorithm>
#include <array>
#include <emmintrin.h>
#include <immintrin.h>
#include <iostream>
#include <numeric>
#include <vectorclass.h>
#include <vectormath_trig.h>

constexpr int SIZE = 16;
using VEC = std::array<float, SIZE>;

void add_vec1(const VEC& left, const VEC& right, VEC& result)
{
    for (int i = 0; i < SIZE; ++i) {
        if (right[i] > 0.f) result[i] = left[i] + right[i];
        else
            result[i] = left[i];
    }
}

void add_vec2(const VEC& left, const VEC& right, VEC& result)
{
    for (int i = 0; i < SIZE; ++i) {
        result[i] = (right[i] > 0.f) ? left[i] + right[i] : left[i];
    }
}

void add_vec3(const VEC& left, const VEC& right, VEC& result)
{
    for (int i = 0; i < SIZE; ++i) {
        result[i] = left[i] + (right[i] > 0.f ? right[i] : 0.f);
    }
}

void add_vec_simd(const VEC& left, const VEC& right, VEC& result)
{

    constexpr __m128 zero = { 0.f, 0.f, 0.f, 0.f };
    // __m128 zero = _mm_setzero_ps();

    for (int i = 0; i < SIZE; i += 4) {
        __m128 left_pack = _mm_load_ps(&left[i]);
        __m128 right_pack = _mm_load_ps(&right[i]);

        // (right > 1.f) ? right : 0.f
        __m128 bit_mask = _mm_cmpge_ps(right_pack, zero);       // bitmask stored in floats
        __m128 float_masked = _mm_and_ps(right_pack, bit_mask); // right or 0.0f

        // result = left + float_masked
        __m128 result_pack = _mm_add_ps(left_pack, float_masked);

        _mm_store_ps(&result[i], result_pack);
    }
}

void add_vec_vectorclass(const VEC& left, const VEC& right, VEC& result)
{
    for (int i = 0; i < SIZE; i += Vec4f::size()) {
        const Vec4f vleft = Vec4f{}.load_a(&left[i]);
        const Vec4f vright = Vec4f{}.load(&right[i]);

        const Vec4f vresult = vleft + sin(vright);
        vresult.store(result.data() + i);
    }
}

void printVec(const VEC& vec)
{
    for (float v : vec) {
        std::cout << v << ", ";
    }
    std::cout << '\n';
}

int main()
{
    using namespace std;
    cout << "Test\n";

    std::array<float, SIZE> left;
    std::array<float, SIZE> right;

    std::iota(left.begin(), left.end(), 1.f);
    std::transform(left.begin(), left.end(), right.begin(), [](float v) { return -10.f + v; });

    printVec(right);
    VEC result1 = { 0.f };
    add_vec1(left, right, result1);
    printVec(result1);

    VEC result2 = { 0.f };
    add_vec2(left, right, result2);
    printVec(result2);

    VEC result3 = { 0.f };
    add_vec3(left, right, result3);
    printVec(result3);

    VEC result_simd = { 0.f };
    add_vec_simd(left, right, result_simd);
    printVec(result_simd);

    VEC result_vectorclass = { 0.f };
    add_vec_vectorclass(left, right, result_vectorclass);
    printVec(result_vectorclass);
}