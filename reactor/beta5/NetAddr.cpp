#include "NetAddr.h"
#include "SocketUtil.h"

#include <strings.h>
#include <netinet/in.h>
#include "netlib/base/Logging.h"

using namespace muduo;

static const in_addr_t kInAddrAny = INADDR_ANY;

NetAddr::NetAddr(uint16_t port) {
    bzero(&m_addr, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = SocketUtil::hostToNetwork32(kInAddrAny);
    m_addr.sin_port = SocketUtil::hostToNetwork16(port);
}

NetAddr::NetAddr(const std::string& ip, uint16_t port) {
    bzero(&m_addr, sizeof(m_addr));
    SocketUtil::fromHostPort(ip.c_str(), port, &m_addr);
}

NetAddr::NetAddr(const struct sockaddr_in& addr)
    : m_addr(addr) 
{
}


std::string NetAddr::ToHostPort() const {
    char buf[32];
    SocketUtil::toHostPort(buf, sizeof(buf), m_addr);
    return buf;
}

const struct sockaddr_in& NetAddr::GetSockAddr() const {
    return m_addr;
}

void NetAddr::SetSockAddr(const struct sockaddr_in& addr) {
    m_addr = addr;
}