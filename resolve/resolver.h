#ifndef RESOLVER_H
#define RESOLVER_H

#include <netdb.h>

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include <optional>
#include <mutex>

class Logger
{
public:
    Logger(std::ostream &ostream);
    ~Logger() {}

    void operator()(std::string &msg);
    void operator()(std::string &&msg);
    void log(std::string &msg);
    void log(std::string &&msg);

private:
    std::mutex mtx_;
    std::ostream &ostream_;
};

enum class eTypeAddr
{
    IPv4,
    IPv6,
};

class Resolver
{
public:
    Resolver();
    ~Resolver() {}
    void set_logger(std::shared_ptr<Logger> &logger);

    std::vector<std::pair<std::string, eTypeAddr>> get_hostaddr(
        const std::string &host_name, const std::string service = "", __socket_type s_type = SOCK_DGRAM, int ipproto = IPPROTO_UDP);
    std::optional<std::shared_ptr<addrinfo>> getaddrinfo_(
        const std::string &host_name, const std::optional<std::string> &service, __socket_type s_type, int ipproto);

    void print_addrinfo(const addrinfo &ref_addrinfo) const;

    // std::optional<
    //     std::pair<std::string, std::string>>
    // getnameinfo_(const sockaddr &sock, socklen_t sock_len, int addr_families);
    // std::string get_hostname(const std::string &host_addr, eTypeAddr type_addr);
    std::string get_hostname(const std::string &host_addr, eTypeAddr type_addr);

private:
    std::shared_ptr<Logger> logger_;
    void log(std::string &msg) const;
    void log(std::string &&msg) const;

    std::shared_ptr<const addrinfo> addrinfo_;
    std::vector<std::string> host_addr_;
    std::string host_name_;
    std::vector<std::string> host_aliases;
};

#endif // RESOLVER_H
