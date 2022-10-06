#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <cstring>

#include <socket_wrapper/socket_headers.h>
#include <socket_wrapper/socket_wrapper.h>
#include <socket_wrapper/socket_class.h>

#include "udp_client.h"

void test_udp_client(std::string address, uint16_t port)
{
    constexpr int SZ_BUF = 256;
    udp_client::udp_client client{SZ_BUF};
    try
    {
        client.init(address, port);
        client.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return;
    }
}

int main(int argc, char const *argv[])
{

    if (argc != 3)
    {
        std::cout << "Usage: " << argv[0] << " <ipv4> <port>" << std::endl;
        return EXIT_FAILURE;
    }

    const int port{std::stoi(argv[2])};

    test_udp_client(std::string(argv[1]), port);

    // socket_wrapper::SocketWrapper sock_wrap;

    // socket_wrapper::Socket sock = {AF_INET, SOCK_DGRAM, IPPROTO_UDP};

    // std::cout << "Starting client on the port " << port << "...\n";

    // if (!sock)
    // {
    //     std::cerr << sock_wrap.get_last_error_string() << std::endl;
    //     return EXIT_FAILURE;
    // }

    // sockaddr_in addrserv =
    //     {
    //         .sin_family = PF_INET,
    //         .sin_port = htons(port),
    //     };

    // int res = inet_pton(AF_INET, argv[1], &addrserv.sin_addr);

    // if (res != 1)
    // {
    //     std::cerr << "Invalid an address format." << std::endl;
    //     return EXIT_FAILURE;
    // }

    // constexpr int SZ_BUF = 256;
    // char buffer[256];
    // char sendline[SZ_BUF], recvline[SZ_BUF + 1];
    // int n;
    // while (fgets(sendline, SZ_BUF, stdin))
    // {
    //     sendto(sock,
    //            sendline,
    //            std::strlen(sendline),
    //            0,
    //            reinterpret_cast<const sockaddr *>(&addrserv),
    //            sizeof(addrserv));
    //     n = recvfrom(sock, recvline, SZ_BUF, 0, nullptr, nullptr);
    //     recvline[n] = 0;
    //     fputs(recvline, stdout);
    // }

    return EXIT_SUCCESS;
}
