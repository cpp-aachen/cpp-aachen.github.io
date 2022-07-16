#include "benchmark/benchmark.h"

#include <cmath>
#include <immintrin.h>
#include <iostream>

#include "sse_helper.h"


template<typename FLOAT>
struct Vec3d
{
    FLOAT x, y, z;
};

void printVec(double val[])
{
    std::cout << val[0] << "\t" << val[1] << "\t" << val[2] << "\n";
}

void printVec(const Vec3d<double>& val)
{
    std::cout << val.x << "\t" << val.y << "\t" << val.z << "\n";
}

void normalize(double val[])
{
    double norm = std::sqrt(val[0] * val[0] + val[1] * val[1] + val[2] * val[2]);
    norm = 1. / norm;
    val[0] *= norm;
    val[1] *= norm;
    val[2] *= norm;
}

void normalizeVec3d(Vec3d<double>& val)
{
    const double norm = std::sqrt(val.x * val.x + val.y * val.y + val.z * val.z);
    const double inv_norm = 1. / norm;
    val.x *= inv_norm;
    val.y *= inv_norm;
    val.z *= inv_norm;
}

Vec3d<double> normalizeVec3d_1(const Vec3d<double>& val)
{
    const double norm = std::sqrt(val.x * val.x + val.y * val.y + val.z * val.z);
    const double inv_norm = 1. / norm;
    return { val.x * inv_norm, val.y * inv_norm, val.z * inv_norm };
}

void normalizeStdHypot(double val[3])
{

    const double norm = std::hypot(val[0], val[1], val[2]);
    const double inv_norm = 1. / norm;
    val[0] *= inv_norm;
    val[1] *= inv_norm;
    val[2] *= inv_norm;
}

void normalizeSSE(double val[])
{

    double* data = &(val[0]);
    __m128d r0 = _mm_loadu_pd(data);
    __m128d r1 = _mm_load_sd(data + 2);
    __m128d norm = _mm_add_sd(_mm_mul_pd(r0, r0), _mm_mul_sd(r1, r1));
    norm = _mm_add_sd(norm, _mm_shuffle_pd(norm, norm, _MM_SHUFFLE2(0, 1)));
    norm = _mm_sqrt_sd(norm, norm);
    norm = _mm_div_sd(_mm_set_sd(1), norm);
    norm = _mm_shuffle_pd(norm, norm, _MM_SHUFFLE2(0, 0));
    r0 = _mm_mul_pd(r0, norm);
    r1 = _mm_mul_sd(r1, norm);
    _mm_storeu_pd(data, r0);
    _mm_store_sd(data + 2, r1);
}

void normalizeSSE2(double val[3])
{
    __m128d x = { val[0] };
    __m128d y = { val[1] };
    __m128d z = { val[2] };

    // hypot = x^2+y^2+z^2
    __m128d x2 = _mm_mul_sd(x, x);
    __m128d y2 = _mm_mul_sd(y, y);
    __m128d z2 = _mm_mul_sd(z, z);
    __m128d hypot = _mm_add_sd(_mm_add_sd(x2, y2), z2);

    // norm = sqrt(hypot)
    __m128d norm = _mm_sqrt_sd(hypot, hypot);
    __m128d inv_norm = _mm_div_sd(_mm_set_sd(1.), norm);

    val[0] = getd(_mm_mul_sd(x, inv_norm));
    val[1] = getd(_mm_mul_sd(y, inv_norm));
    val[2] = getd(_mm_mul_sd(z, inv_norm));
}

void normalizeSSE2(Vec3d<double>& val)
{
    __m128d x = { val.x };
    __m128d y = { val.y };
    __m128d z = { val.z };

    // hypot = x^2+y^2+z^2
    __m128d x2 = _mm_mul_sd(x, x);
    __m128d y2 = _mm_mul_sd(y, y);
    __m128d z2 = _mm_mul_sd(z, z);
    __m128d hypot = _mm_add_sd(_mm_add_sd(x2, y2), z2);

    // norm = sqrt(hypot)
    __m128d norm = _mm_sqrt_sd(hypot, hypot);
    __m128d inv_norm = _mm_div_sd(_mm_set_sd(1.), norm);

    val.x = getd(_mm_mul_sd(x, inv_norm));
    val.y = getd(_mm_mul_sd(y, inv_norm));
    val.z = getd(_mm_mul_sd(z, inv_norm));
}

static void NormalizeRaw(benchmark::State& state)
{
    // Code before the loop is not measured
    double val[3] = { 1.0, 1.0, 1.0 };
    for (auto _ : state) {
        normalize(val);
    }
    // printVec(val);
    benchmark::DoNotOptimize(val);
}
BENCHMARK(NormalizeRaw);

static void NormalizeVec3d(benchmark::State& state)
{
    // Code before the loop is not measured
    Vec3d<double> val = { 1.0, 1.0, 1.0 };
    for (auto _ : state) {
        normalizeVec3d(val);
    }

    // printVec(val);
    benchmark::DoNotOptimize(val);
}
BENCHMARK(NormalizeVec3d);

static void NormalizeVec3d_1(benchmark::State& state)
{
    // Code before the loop is not measured
    Vec3d<double> val = { 1.0, 1.0, 1.0 };
    for (auto _ : state) {
        val = normalizeVec3d_1(val);
    }

    // printVec(val);
    benchmark::DoNotOptimize(val);
}
BENCHMARK(NormalizeVec3d_1);

static void NormalizeSSE(benchmark::State& state)
{
    // Code before the loop is not measured
    double val[3] = { 1.0, 1.0, 1.0 };
    for (auto _ : state) {
        normalizeSSE(val);
    }
    benchmark::DoNotOptimize(val);
}
BENCHMARK(NormalizeSSE);

static void NormalizeStdHypot(benchmark::State& state)
{
    // Code before the loop is not measured
    double val[3] = { 1.0, 1.0, 1.0 };
    for (auto _ : state) {
        normalizeStdHypot(val);
    }
    benchmark::DoNotOptimize(val);
}
BENCHMARK(NormalizeStdHypot);

static void NormalizeSSE2(benchmark::State& state)
{
    // Code before the loop is not measured
    double val[3] = { 1.0, 1.0, 1.0 };
    for (auto _ : state) {
        normalizeSSE2(val);
    }
    // printVec(val);
    benchmark::DoNotOptimize(val);
}
BENCHMARK(NormalizeSSE2);

static void NormalizeSSE2a(benchmark::State& state)
{
    // Code before the loop is not measured
    Vec3d<double> val = { 1.0, 1.0, 1.0 };
    for (auto _ : state) {
        normalizeSSE2(val);
    }
    // printVec(val);
    benchmark::DoNotOptimize(val);
}
BENCHMARK(NormalizeSSE2a);

BENCHMARK_MAIN();
