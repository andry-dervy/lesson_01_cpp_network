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

namespace udp_client
{
    enum class StateUdpClient
    {
        Created,
        Initialized,
        Running,
    };

    class udp_client
    {
    private:
        const int size_buffer_;
        std::unique_ptr<socket_wrapper::Socket> psock_;
        StateUdpClient state_;

        socket_wrapper::SocketWrapper sock_wrap_;
        int port_;
        sockaddr_in addrserv_;

        std::vector<char> sendline_;
        std::vector<char> recvline_;

    private:
        void set_state(StateUdpClient state) { state_ = state; }
        void response_processing(std::string &&response);

    public:
        StateUdpClient get_state() const { return state_; }

    public:
        udp_client(int size_buffer);
        ~udp_client();
        void init(std::string address, uint16_t port);
        void run();
    };

} // namespace upd_client