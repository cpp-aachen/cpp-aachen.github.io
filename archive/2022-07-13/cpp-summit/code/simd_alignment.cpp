static_assert(__STDCPP_DEFAULT_NEW_ALIGNMENT__ == 16);
static_assert(__STDCPP_DEFAULT_NEW_ALIGNMENT__ * 8 == 128);

#include <iostream>

int main()
{
    using namespace std;

    cout << "Hallo\n";
#ifdef __STDCPP_DEFAULT_NEW_ALIGNMENT__
    cout << "__STDCPP_DEFAULT_NEW_ALIGNMENT__ = " << __STDCPP_DEFAULT_NEW_ALIGNMENT__ << " bytes ("
         << __STDCPP_DEFAULT_NEW_ALIGNMENT__ * 8 << " bits)\n";
#else
    cout << "__STDCPP_DEFAULT_NEW_ALIGNMENT__ is not defined\n";

#endif
}
