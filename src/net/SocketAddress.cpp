
#include <arpa/inet.h>
#include <sys/socket.h>

#include <net/SocketAddress.h>
#include <utils/Exception.h>


using namespace net;

SocketAddress::SocketAddress(const std::string& ip, const std::string& port) {
    Reset();
    ParseIp(ip);
    ParsePort(port);
}

SocketAddress::~SocketAddress() {
}

sockaddr_in SocketAddress::GetSockAddr() const {
    return address_;
}

void SocketAddress::Reset() {
    address_.sin_family = PF_INET;
    address_.sin_port = 0;
    address_.sin_addr.s_addr = 0;
}

void SocketAddress::ParseIp(const std::string& ip) {
    int ip_set_result = inet_pton(AF_INET, ip.c_str(), &address_.sin_addr);
    if (ip_set_result != 1) {
        // Convert the ip from string is failed
        throw utils::Exception("network address", "IP value is incorrect");
    }
}

void SocketAddress::ParsePort(const std::string& port) {
    unsigned long result;
    try {
        result = std::stoul(port);
    }
    catch(const std::exception& e) {
        // Convert the port number from string is failed
        throw utils::Exception("network address", "Port value is incorrect");
    }

    if (result == 0 || result > 65535) {
        // Port number is incorrect
        throw utils::Exception("network address", "Port number is incorrect");
    }

    address_.sin_port = htons(result);
}
