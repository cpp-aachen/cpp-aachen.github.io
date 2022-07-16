#include <iostream>
#include <emmintrin.h>

#include "sse_helper.h"

extern "C"
{
    float add_f_asm(float, float);
    double add_d_asm(double, double);
}

float add_f_sse(float left, float right)
{
    return getf(_mm_add_ss(__m128{ left }, __m128{ right }));
}

double add_d_sse(double left, double right)
{
    return _mm_add_sd(__m128d{left, 0.0}, __m128d{right, 0})
#ifdef _MSC_VER
        .m128d_f64
#endif
        [0];
}

int main()
{

    using namespace std;
    cout << "Test asm calls\n";

    const float f1 = 1.1f, f2 = 2.2f;
    const double d1 = 1.1, d2 = 2.2;

    // asm functions:
    {
        const float f3 = add_f_asm(f1, f2);
        cout << "Add (add_f_asm): " << f3 << "\n";

        const double d3 = add_d_asm(d1, d2);
        cout << "Add (add_d_asm): " << d3 << "\n";
    }

    // SSE functions:
    {
        const float f3 = add_f_sse(f1, f2);
        cout << "Add (add_f_sse): " << f3 << "\n";

        const double d3 = add_d_sse(d1, d2);
        cout << "Add (add_d_sse): " << d3 << "\n";
    }
}
