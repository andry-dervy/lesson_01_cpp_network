#include <cassert>
#include <netdb.h>
#include <arpa/inet.h>
#include "resolver.h"

void Logger::log(std::string &msg)
{
    std::lock_guard<std::mutex> lock(mtx_);
    ostream_ << msg << std::endl;
}

void Logger::log(std::string &&msg)
{
    std::lock_guard<std::mutex> lock(mtx_);
    ostream_ << msg << std::endl;
}

Logger::Logger(std::ostream &ostream)
    : ostream_(ostream)
{
}

void Logger::operator()(std::string &msg)
{
    log(msg);
}

void Logger::operator()(std::string &&msg)
{
    log(msg);
}

Resolver::Resolver()
    : logger_(nullptr)
{
}

void Resolver::set_logger(std::shared_ptr<Logger> &logger)
{
    logger_ = logger;
}

std::optional<std::shared_ptr<addrinfo>> Resolver::getaddrinfo_(
    const std::string &host_name, const std::optional<std::string> &service, __socket_type s_type, int ipproto)
{
    std::vector<std::string> vips;
    addrinfo hints =
        {
            .ai_flags = AI_CANONNAME,
            .ai_family = AF_UNSPEC,
            .ai_socktype = s_type, // SOCK_STREAM,
            .ai_protocol = ipproto};

    addrinfo *servinfo = nullptr;
    auto serv = service.has_value() ? service->c_str() : nullptr;

    int status = getaddrinfo(host_name.c_str(), serv, &hints, &servinfo);

    if (status != 0)
    {
        std::stringstream ss;
        ss << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        log(ss.str());
        return std::nullopt;
    }

    std::shared_ptr<addrinfo> p_addrinfo{servinfo, [](addrinfo *p)
                                         { freeaddrinfo(p); }};

    return std::make_optional<std::shared_ptr<addrinfo>>(p_addrinfo);
}

std::vector<std::pair<std::string, eTypeAddr>> Resolver::get_hostaddr(
    const std::string &host_name, const std::string service, __socket_type s_type, int ipproto)
{
    std::vector<std::pair<std::string, eTypeAddr>> vips;

    auto servinfo = getaddrinfo_(
        host_name,
        service.empty() ? std::optional<std::string>(service) : std::nullopt,
        s_type, ipproto);

    if (!servinfo.has_value())
    {
        std::stringstream ss;
        ss << "Didn't get a addrinfo." << std::endl;
        log(ss.str());
        return vips;
    }

    auto p_serv = servinfo.value();

    for (auto *s = p_serv.get(); s != nullptr; s = s->ai_next)
    {
        print_addrinfo(*s);

        assert(s->ai_family == s->ai_addr->sa_family);

        std::stringstream ss;
        if (AF_INET == s->ai_family)
        {
            char ip[INET_ADDRSTRLEN];
            sockaddr_in const *const sin = reinterpret_cast<const sockaddr_in *const>(s->ai_addr);
            ss << inet_ntop(AF_INET, &(sin->sin_addr), ip, INET_ADDRSTRLEN);
            vips.push_back(std::make_pair(ss.str(), eTypeAddr::IPv4));
        }
        else if (AF_INET6 == s->ai_family)
        {
            char ip6[INET6_ADDRSTRLEN];
            const sockaddr_in6 *const sin = reinterpret_cast<const sockaddr_in6 *const>(s->ai_addr);
            ss << inet_ntop(AF_INET6, &(sin->sin6_addr), ip6, INET6_ADDRSTRLEN);
            vips.push_back(std::make_pair(ss.str(), eTypeAddr::IPv6));
        }
    }

    return vips;
}

std::string Resolver::get_hostname(const std::string &host_addr, eTypeAddr type_addr)
{
    std::string host_name;
    hostent *htnt;
    in_addr addr;

    int addr_families;
    int addr_len;
    switch (type_addr)
    {
    case eTypeAddr::IPv4:
        addr_families = AF_INET;
        addr_len = 4;
        break;
    case eTypeAddr::IPv6:
        addr_families = AF_INET6;
        addr_len = 16;
        break;
    }

    int code = inet_pton(addr_families, host_addr.c_str(), &addr);

    if (code == 0)
    {
        std::stringstream ss;
        ss << "host_addr: "
           << "invalid format." << std::endl;
        log(ss.str());
        return host_name;
    }
    else if (code == -1)
    {
        std::stringstream ss;
        ss << "inet_pton returns error." << std::endl;
        log(ss.str());
        return host_name;
    }

    htnt = gethostbyaddr(&addr, addr_len, addr_families);

    if (!htnt)
    {
        log("Host name didn't found.");
        return host_name;
    }

    host_name = std::string(htnt->h_name);

    return host_name;
}

void Resolver::print_addrinfo(const addrinfo &ref_addrinfo) const
{
    std::stringstream ss;

    ss << "Canonical name: ";
    if (ref_addrinfo.ai_canonname)
        ss << ref_addrinfo.ai_canonname;
    ss << "\n";

    assert(ref_addrinfo.ai_family == ref_addrinfo.ai_addr->sa_family);
    ss << "Address type: ";

    if (AF_INET == ref_addrinfo.ai_family)
    {
        char ip[INET_ADDRSTRLEN];
        ss << "AF_INET\n";
        sockaddr_in const *const sin = reinterpret_cast<const sockaddr_in *const>(ref_addrinfo.ai_addr);
        ss << "Address length: " << sizeof(sin->sin_addr) << "\n";
        ss << "IP Address: " << inet_ntop(AF_INET, &(sin->sin_addr), ip, INET_ADDRSTRLEN) << "\n";
    }
    else if (AF_INET6 == ref_addrinfo.ai_family)
    {
        char ip6[INET6_ADDRSTRLEN];

        ss << "AF_INET6\n";
        const sockaddr_in6 *const sin = reinterpret_cast<const sockaddr_in6 *const>(ref_addrinfo.ai_addr);
        ss << "Address length: " << sizeof(sin->sin6_addr) << "\n";
        ss << "IP Address: " << inet_ntop(AF_INET6, &(sin->sin6_addr), ip6, INET6_ADDRSTRLEN) << "\n";
    }
    else
    {
        ss << ref_addrinfo.ai_family << "\n";
    }
    ss << std::endl;

    log(ss.str());
}

void Resolver::log(std::string &msg) const
{
    if (logger_ != nullptr)
        logger_->log(msg);
}
void Resolver::log(std::string &&msg) const
{
    if (logger_ != nullptr)
        logger_->log(msg);
}
