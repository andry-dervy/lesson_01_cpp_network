#pragma once

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <cstring>

#include <socket_wrapper/socket_headers.h>
#include <socket_wrapper/socket_wrapper.h>
#include <socket_wrapper/socket_class.h>

namespace udp_server
{
    enum class StateUdpServer
    {
        Created,
        Initialized,
        Running,
    };

    class udp_server
    {
    private:
        const int size_buffer_;
        StateUdpServer state_;
        bool is_running;
        std::unique_ptr<socket_wrapper::Socket> psock_;
        socket_wrapper::SocketWrapper sock_wrap_;
        int port_;
        sockaddr_in addrserv_;

        std::vector<char> sendline_;
        std::vector<char> recvline_;

        struct sockaddr_in client_address;
        socklen_t client_address_len;
        char client_address_buf[INET_ADDRSTRLEN];

    private:
        void set_state(StateUdpServer state) { state_ = state; }
        void request_processing(std::string &&request);

        std::vector<std::string> split(std::string &in) const;

    public:
        StateUdpServer get_state() const { return state_; }

    public:
        udp_server(int size_buffer);
        ~udp_server();
        void init(uint16_t port);
        void run();
    };

} // namespace udp_server