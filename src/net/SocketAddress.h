
#ifndef H419377D0_29FE_4CEA_AD14_3C3A46EB4AEA
#define H419377D0_29FE_4CEA_AD14_3C3A46EB4AEA

#include <netinet/in.h>

#include <string>

namespace net {

    // This class is wrapper for a sockaddr_in
    class SocketAddress {
    public:
        // throw utils::Exception on parsing error.
        SocketAddress(const std::string& ip, const std::string& port);
        virtual ~SocketAddress();

        sockaddr_in GetSockAddr() const;
    private:
        // Set zero address
        void Reset();

        void ParseIp(const std::string& ip);
        void ParsePort(const std::string& port);

        sockaddr_in address_;
    };
}

#endif /* H419377D0_29FE_4CEA_AD14_3C3A46EB4AEA */
