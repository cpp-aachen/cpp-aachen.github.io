#include "align_allocator.h"

#include "benchmark/benchmark.h"

#include <array>
#include <cmath>
#include <immintrin.h>
#include <iostream>
#include <vector>

template<typename FLOAT>
struct Vec3d
{
    FLOAT x, y, z;
};

constexpr int SIZE = 1024;

using VectorAOS = std::vector<Vec3d<double>>;
using VectorSOA = Vec3d<std::vector<double, align_allocator<double, sizeof(__m256)>>>;

VectorAOS initVectorAOS(double val)
{
    return VectorAOS(SIZE, Vec3d<double>{ val, val, val });
}

VectorSOA initVectorSOA(double val)
{
    return VectorSOA{ std::vector<double, align_allocator<double, sizeof(__m256)>>(SIZE, val),
                      std::vector<double, align_allocator<double, sizeof(__m256)>>(SIZE, val),
                      std::vector<double, align_allocator<double, sizeof(__m256)>>(SIZE, val) };
}

const VectorAOS inputAOS = initVectorAOS(1.0);
VectorAOS outputAOS = initVectorAOS(0.0);

const VectorSOA inputSOA = initVectorSOA(1.0);
VectorSOA outputSOA = initVectorSOA(0.0);

void printVec(const Vec3d<double>& val)
{
    std::cout << val.x << "\t" << val.y << "\t" << val.z << "\n";
}

void print(const VectorAOS& vec)
{
    std::cout << "Size = " << vec.size() << "\n";
    for (auto val : vec) {
        printVec(val);
    }
}

void print(const VectorSOA& vec)
{
    std::cout << "Size = " << vec.x.size() << "\n";
    for (int i = 0; i < vec.x.size(); ++i) {
        Vec3d<double> val{ vec.x[i], vec.y[i], vec.z[i] };
        printVec(val);
    }
}

void normalizeVectorAOS(VectorAOS& val)
{
    for (size_t i = 0; i < val.size(); ++i) {
        const double norm = std::sqrt(val[i].x * val[i].x + val[i].y * val[i].y + val[i].z * val[i].z);
        const double inv_norm = 1. / norm;

        val[i].x *= inv_norm;
        val[i].y *= inv_norm;
        val[i].z *= inv_norm;
    }
}

void normalizeVectorSOA(VectorSOA& val)
{
    for (size_t i = 0; i < val.x.size(); ++i) {
        const double norm = std::sqrt(val.x[i] * val.x[i] + val.y[i] * val.y[i] + val.z[i] * val.z[i]);
        const double inv_norm = 1. / norm;

        val.x[i] *= inv_norm;
        val.y[i] *= inv_norm;
        val.z[i] *= inv_norm;
    };
}

void normalizeSSE(VectorSOA& val, size_t index)
{
    static const __m128d one = { 1., 1. };
    __m128d x = _mm_load_pd(&val.x[index]);
    __m128d y = _mm_load_pd(&val.y[index]);
    __m128d z = _mm_load_pd(&val.z[index]);

    // hypot = x^2+y^2+z^2
    __m128d x2 = _mm_mul_pd(x, x);
    __m128d y2 = _mm_mul_pd(y, y);
    __m128d z2 = _mm_mul_pd(z, z);
    __m128d hypot = _mm_add_pd(x2, y2);
    hypot = _mm_add_pd(hypot, z2);

    // norm = sqrt(hypot)
    __m128d norm = _mm_sqrt_pd(hypot);
    __m128d inv_norm = _mm_div_pd(one, norm);

    __m128d res_x = _mm_mul_pd(x, inv_norm);
    __m128d res_y = _mm_mul_pd(y, inv_norm);
    __m128d res_z = _mm_mul_pd(z, inv_norm);

    _mm_store_pd(&val.x[index], res_x);
    _mm_store_pd(&val.y[index], res_y);
    _mm_store_pd(&val.z[index], res_z);
}

void normalizeVectorSOA_SSE(VectorSOA& val)
{
    for (size_t i = 0; i < val.x.size(); i += 2) {
        normalizeSSE(val, i);
    };
}

void normalizeAVX(VectorSOA& val, size_t index)
{
    static const __m256d one = { 1., 1., 1., 1. };
    __m256d x = _mm256_load_pd(&val.x[index]);
    __m256d y = _mm256_load_pd(&val.y[index]);
    __m256d z = _mm256_load_pd(&val.z[index]);

    // hypot = x^2+y^2+z^2
    __m256d x2 = _mm256_mul_pd(x, x);
    __m256d y2 = _mm256_mul_pd(y, y);
    __m256d z2 = _mm256_mul_pd(z, z);
    __m256d hypot = _mm256_add_pd(_mm256_add_pd(x2, y2), z2);

    // norm = sqrt(hypot)
    __m256d norm = _mm256_sqrt_pd(hypot);
    //__m256d inv_norm = _mm256_div_pd(_mm256_set_pd(1, 1, 1, 1), norm);
    __m256d inv_norm = _mm256_div_pd(one, norm);

    __m256d res_x = _mm256_mul_pd(x, inv_norm);
    __m256d res_y = _mm256_mul_pd(y, inv_norm);
    __m256d res_z = _mm256_mul_pd(z, inv_norm);

    _mm256_store_pd(&val.x[index], res_x);
    _mm256_store_pd(&val.y[index], res_y);
    _mm256_store_pd(&val.z[index], res_z);
}

void normalizeVectorSOA_AVX(VectorSOA& val)
{
    for (size_t i = 0; i < val.x.size(); i += 4) {
        normalizeAVX(val, i);
    };
}

static void NormalizeAOS(benchmark::State& state)
{
    outputAOS = inputAOS;
    for (auto _ : state) {
        normalizeVectorAOS(outputAOS);
        benchmark::DoNotOptimize(outputAOS);
    }
    // print(outputAOS);
}
BENCHMARK(NormalizeAOS);

static void NormalizeSOA(benchmark::State& state)
{
    outputSOA = inputSOA;
    for (auto _ : state) {
        normalizeVectorSOA(outputSOA);
        benchmark::DoNotOptimize(outputSOA);
    }
    // print(outputSOA);
}
BENCHMARK(NormalizeSOA);

static void NormalizeSOA_SSE(benchmark::State& state)
{
    outputSOA = inputSOA;
    for (auto _ : state) {
        normalizeVectorSOA_SSE(outputSOA);
        benchmark::DoNotOptimize(outputSOA);
    }
    // print(outputSOA);
}
BENCHMARK(NormalizeSOA_SSE);

static void NormalizeSOA_AVX(benchmark::State& state)
{

    outputSOA = inputSOA;
    for (auto _ : state) {
        normalizeVectorSOA_AVX(outputSOA);
        benchmark::DoNotOptimize(outputSOA);
    }
    // print(outputSOA);
}
BENCHMARK(NormalizeSOA_AVX);

BENCHMARK_MAIN();
