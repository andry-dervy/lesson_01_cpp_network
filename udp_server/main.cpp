#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

#include <socket_wrapper/socket_headers.h>
#include <socket_wrapper/socket_wrapper.h>
#include <socket_wrapper/socket_class.h>

#include "udp_server.h"

// Trim from end (in place).
static inline std::string &rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int c)
                         { return !std::isspace(c); })
                .base());
    return s;
}

static void test_udp_server(uint16_t port)
{
    constexpr int SZ_BUF = 256;
    udp_server::udp_server server{SZ_BUF};
    try
    {
        server.init(port);
        server.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return;
    }
}

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return EXIT_FAILURE;
    }

    const int port = std::stoi(argv[1]);

    test_udp_server(port);

    return EXIT_SUCCESS;
}

