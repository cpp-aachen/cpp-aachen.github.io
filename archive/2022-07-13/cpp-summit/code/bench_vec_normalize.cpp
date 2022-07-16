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

Vec3d<double> normalizeSingleVec3d(const Vec3d<double>& val)
{
    const double norm = std::sqrt(val.x * val.x + val.y * val.y + val.z * val.z);
    const double inv_norm = 1. / norm;
    return { val.x * inv_norm, val.y * inv_norm, val.z * inv_norm };
}

void normalizeVectorAOS(const VectorAOS& in, VectorAOS& out)
{
    out.resize(in.size());
    for (size_t i = 0; i < in.size(); ++i) {
        out[i] = normalizeSingleVec3d(in[i]);
    }
}

void normalizeVectorSOA(const VectorSOA& in, VectorSOA& out)
{
    out.x.resize(in.x.size());
    out.y.resize(in.y.size());
    out.z.resize(in.z.size());

    for (size_t i = 0; i < in.x.size(); ++i) {

        const double x = in.x[i];
        const double y = in.y[i];
        const double z = in.z[i];

        const double norm = std::sqrt(in.x[i] * in.x[i] + in.y[i] * in.y[i] + in.z[i] * in.z[i]);
        const double inv_norm = 1. / norm;

        out.x[i] = in.x[i] * inv_norm;
        out.y[i] = in.y[i] * inv_norm;
        out.z[i] = in.z[i] * inv_norm;
    };
}

void normalizeSSE(const VectorSOA& in, VectorSOA& out, size_t index)
{

    static const __m128d one = { 1., 1.};
 
    __m128d x = _mm_load_pd(&in.x[index]);
    __m128d y = _mm_load_pd(&in.y[index]);
    __m128d z = _mm_load_pd(&in.z[index]);

    // hypot = x^2+y^2+z^2
    __m128d x2 = _mm_mul_pd(x, x);
    __m128d y2 = _mm_mul_pd(y, y);
    __m128d z2 = _mm_mul_pd(z, z);
    __m128d hypot = _mm_add_pd(one, z2);

    // norm = sqrt(hypot)
    __m128d norm = _mm_sqrt_pd(hypot);
    __m128d inv_norm = _mm_div_pd(_mm_set_pd(1., 1.), norm);

    __m128d res_x = _mm_mul_pd(x, inv_norm);
    __m128d res_y = _mm_mul_pd(y, inv_norm);
    __m128d res_z = _mm_mul_pd(z, inv_norm);

    _mm_store_pd(&out.x[index], res_x);
    _mm_store_pd(&out.y[index], res_y);
    _mm_store_pd(&out.z[index], res_z);
}

void normalizeVectorSOA_SSE(const VectorSOA& in, VectorSOA& out)
{
    out.x.resize(in.x.size());
    out.y.resize(in.y.size());
    out.z.resize(in.z.size());

    for (size_t i = 0; i < in.x.size(); i += 2) {
        normalizeSSE(in, out, i);
    };
}

void normalizeAVX(const VectorSOA& in, VectorSOA& out, size_t index)
{
    static const __m256d one = { 1., 1., 1., 1. };
    __m256d x = _mm256_load_pd(&in.x[index]);
    __m256d y = _mm256_load_pd(&in.y[index]);
    __m256d z = _mm256_load_pd(&in.z[index]);

    // hypot = x^2+y^2+z^2
    __m256d x2 = _mm256_mul_pd(x, x);
    __m256d y2 = _mm256_mul_pd(y, y);
    __m256d z2 = _mm256_mul_pd(z, z);
    __m256d hypot = _mm256_add_pd(_mm256_add_pd(x2, y2), z2);

    // norm = sqrt(hypot)
    __m256d norm = _mm256_sqrt_pd(hypot);
    __m256d inv_norm = _mm256_div_pd(one, norm);

    __m256d res_x = _mm256_mul_pd(x, inv_norm);
    __m256d res_y = _mm256_mul_pd(y, inv_norm);
    __m256d res_z = _mm256_mul_pd(z, inv_norm);

    _mm256_store_pd(&out.x[index], res_x);
    _mm256_store_pd(&out.y[index], res_y);
    _mm256_store_pd(&out.z[index], res_z);
}

void normalizeVectorSOA_AVX(const VectorSOA& in, VectorSOA& out)
{
    out.x.resize(in.x.size());
    out.y.resize(in.y.size());
    out.z.resize(in.z.size());

    for (size_t i = 0; i < in.x.size(); i += 4) {
        normalizeAVX(in, out, i);
    };
}

static void NormalizeAOS(benchmark::State& state)
{
    outputAOS = initVectorAOS(0.0);
    for (auto _ : state) {
        normalizeVectorAOS(inputAOS, outputAOS);
        benchmark::DoNotOptimize(outputAOS);
    }
    // print(outputAOS);
}
BENCHMARK(NormalizeAOS);

static void NormalizeSOA(benchmark::State& state)
{
    outputSOA = initVectorSOA(0.0);
    for (auto _ : state) {
        normalizeVectorSOA(inputSOA, outputSOA);
        benchmark::DoNotOptimize(outputSOA);
    }
    // print(outputSOA);
}
BENCHMARK(NormalizeSOA);

static void NormalizeSOA_SSE(benchmark::State& state)
{
    outputSOA = initVectorSOA(0.0);
    for (auto _ : state) {
        normalizeVectorSOA_SSE(inputSOA, outputSOA);
        benchmark::DoNotOptimize(outputSOA);
    }
    // print(outputSOA);
}
BENCHMARK(NormalizeSOA_SSE);

static void NormalizeSOA_AVX(benchmark::State& state)
{
    outputSOA = initVectorSOA(0.0);
    for (auto _ : state) {
        normalizeVectorSOA_AVX(inputSOA, outputSOA);
        benchmark::DoNotOptimize(outputSOA);
    }
    // print(outputSOA);
}
BENCHMARK(NormalizeSOA_AVX);

BENCHMARK_MAIN();
