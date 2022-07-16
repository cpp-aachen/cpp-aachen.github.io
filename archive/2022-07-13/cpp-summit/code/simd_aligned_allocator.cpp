#include <immintrin.h>
#include <iostream>
#include <vector>

#include <boost/align/aligned_allocator.hpp>

int main()
{
    std::vector<double, boost::alignment::aligned_allocator<double, 16>> vec1(3.14, 16);
    __m256d v1 = _mm256_load_pd(&vec1[1]);

    return v1[0];
}
