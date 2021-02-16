#ifndef NETADDR_H
#define NETADDR_H

#include <string>
#include <netinet/in.h>

class NetAddr
{
public:
    explicit NetAddr(uint16_t port);
    NetAddr(const std::string& ip, uint16_t port);
    NetAddr(const struct sockaddr_in& addr);

    std::string ToHostPort() const;

    const struct sockaddr_in& GetSockAddr() const;
    void SetSockAddr(const struct sockaddr_in& addr);
private:
    struct sockaddr_in m_addr;
};

#endif // NETADDR_H