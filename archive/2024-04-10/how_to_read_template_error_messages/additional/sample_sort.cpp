#include <algorithm>
#include <string>
#include <vector>

struct UserName
{
    std::string name;
};

int main()
{
    std::vector<UserName> users;
    std::sort(users.begin(), users.end());
}
