#include "udp_client.h"
#include "stdexcept"

namespace udp_client
{

    udp_client::udp_client(int size_buffer)
        : size_buffer_(size_buffer), state_(StateUdpClient::Created)
    {
        sendline_.resize(size_buffer_);
        recvline_.resize(size_buffer_ + 1);
    }

    udp_client::~udp_client()
    {
    }

    void udp_client::init(std::string address, uint16_t port)
    {
        psock_ = std::make_unique<socket_wrapper::Socket>(
            AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        if (!*psock_)
        {
            std::string err = "Error socket: " + sock_wrap_.get_last_error_string();
            throw std::runtime_error(err);
        }

        addrserv_ =
            {
                .sin_family = PF_INET,
                .sin_port = htons(port),
            };

        int res = inet_pton(AF_INET, address.c_str(), &addrserv_.sin_addr);

        if (res != 1)
        {
            throw std::runtime_error("Invalid an address format.");
        }

        set_state(StateUdpClient::Initialized);
    }

    void udp_client::run()
    {
        if (get_state() != StateUdpClient::Initialized)
        {
            throw std::runtime_error("Client isn't initialized.");
        }

        set_state(StateUdpClient::Running);

        int n;
        while (fgets(sendline_.data(), size_buffer_, stdin))
        {
            sendto(*psock_,
                   sendline_.data(),
                   std::strlen(sendline_.data()),
                   0,
                   reinterpret_cast<const sockaddr *>(&addrserv_),
                   sizeof(addrserv_));
            n = recvfrom(*psock_, recvline_.data(), size_buffer_, 0, nullptr, nullptr);
            recvline_[n] = 0;

            response_processing(std::string(recvline_.data()));
        }

        set_state(StateUdpClient::Initialized);
    }

    void udp_client::response_processing(std::string &&response)
    {
        std::cout << response << std::endl;
    }

} // namespace upd_client