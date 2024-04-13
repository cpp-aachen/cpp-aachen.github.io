#include <iostream>
#include <string>
#include <vector>

template<int level, typename Arg, typename Stream>
struct indented_printer
{
    void operator()(Stream& s, const Arg& arg)
    {
        s << ' ';
        return indented_printer<level - 1, Arg, Stream>()(s, arg);
    }
};

template<typename Arg, typename Stream>
struct indented_printer<0, Arg, Stream>
{
    void operator()(Stream& s, const Arg& arg)
    {
        s << arg;
    }
};

template<int level, typename Arg, typename Stream>
void printIndented(Stream& s, const Arg& arg)
{
    indented_printer<level, Arg, Stream>()(s, arg);
}

int main()
{
    std::vector<std::string> v;
    printIndented<20>(std::cout, v);
    return 0;
}
