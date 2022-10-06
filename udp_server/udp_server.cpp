
#include "udp_server.h"

namespace udp_server
{

    udp_server::udp_server(int size_buffer)
        : size_buffer_(size_buffer),
          state_(StateUdpServer::Created),
          is_running(false)
    {
        sendline_.resize(size_buffer_);
        recvline_.resize(size_buffer_ + 1);
    }

    udp_server::~udp_server()
    {
    }

    void udp_server::init(uint16_t port)
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

        addrserv_.sin_addr.s_addr = INADDR_ANY;

        set_state(StateUdpServer::Initialized);
    }

    void udp_server::run()
    {
        if (get_state() != StateUdpServer::Initialized)
        {
            throw std::runtime_error("Server isn't initialized.");
        }

        if (bind(*psock_, reinterpret_cast<const sockaddr *>(&addrserv_), sizeof(addrserv_)) != 0)
        {
            set_state(StateUdpServer::Created);
            std::string err = sock_wrap_.get_last_error_string();
            throw std::runtime_error(err);
        }

        set_state(StateUdpServer::Running);

        // socket address used to store client address
        client_address = {0};
        client_address_len = sizeof(sockaddr_in);
        ssize_t recv_len = 0;

        std::cout << "Running echo server...\n"
                  << std::endl;

        is_running = true;
        while (is_running)
        {
            // Read content into buffer from an incoming client.
            recv_len = recvfrom(*psock_,
                                recvline_.data(),
                                size_buffer_, 0,
                                reinterpret_cast<sockaddr *>(&client_address),
                                &client_address_len);

            if (recv_len > 0)
            {
                recvline_[recv_len] = '\0';
                request_processing(std::string(recvline_.data()));
            }
        }
        std::cout << "The server has been terminated...\n"
                  << std::endl;
    }

    void udp_server::request_processing(std::string &&request)
    {
        std::cout
            << "Client with address "
            << inet_ntop(AF_INET, &client_address.sin_addr, client_address_buf, sizeof(client_address_buf) / sizeof(client_address_buf[0]))
            << ":" << ntohs(client_address.sin_port)
            << " sent datagram "
            << "[length = "
            << request.size() // recv_len
            << "]:\n"
            << request
            << std::endl;

        auto vmes = split(request);

        if (!vmes.empty() && vmes[0] == "exit")
        {
            is_running = false;
        }

        // Send same content back to the client ("echo").
        sendto(*psock_, request.c_str(), request.size(), 0, reinterpret_cast<const sockaddr *>(&client_address),
               client_address_len);

        std::cout << std::endl;
    }

    std::vector<std::string> udp_server::split(std::string &in) const
    {
        std::vector<std::string> v;

        size_t first = 0, i = 0;
        size_t sz = in.size();
        while (i < sz)
        {
            if (std::isspace(in[i]))
            {
                if (first == i)
                {
                    ++first;
                }
                else
                {
                    v.push_back(in.substr(first, i - first));
                    first = i + 1;
                }
            }
            ++i;
        }
        if (first != i)
        {
            v.push_back(in.substr(first, i - first));
            first = i + 1;
        }
        return v;
    }

} // namespace udp_server