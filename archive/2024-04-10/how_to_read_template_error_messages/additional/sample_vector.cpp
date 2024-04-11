#include <algorithm>
#include <vector>

int main()
{
    std::vector<std::vector<int>> values;
    return std::find(values.begin(), values.end(), 5) != values.end();
}
