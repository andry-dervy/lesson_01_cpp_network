#include <iostream>
#include "resolver.h"

void test_resolver()
{
    using std::cout;
    using std::endl;

    cout << "\nTest Resolver::get_hostaddr" << endl;

    auto logger = std::make_shared<Logger>(cout);
    Resolver res;
    res.set_logger(logger);
    auto vips = res.get_hostaddr("mail.ru");

    if (vips.empty())
    {
        cout << "Addresses didn't find." << endl;
    }
    else
    {
        cout << "Addresses found: " << endl;
        for (const auto &a : vips)
            cout << a.first << endl;
    }

    cout << "\nTest Resolver::get_hostname" << endl;

    for (auto ip : vips)
        cout << "Host " << ip.first << " haves a name: " << res.get_hostname(ip.first, ip.second) << endl;
}

int main()
{
    std::cout << "Project resolve" << std::endl;

    test_resolver();

    return 0;
}
