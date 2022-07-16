#pragma once

float getf(const __m128 v)
{
#ifdef _MSC_VER
    return v.m128_f32[0];
#else
    return v[0];
#endif
}

double getd(const __m128d v)
{
#ifdef _MSC_VER
    return v.m128d_f64[0];
#else
    return v[0];
#endif
}
