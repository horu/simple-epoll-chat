
#ifndef H691CB5AE_0772_4832_9968_9A8D11A0F02B
#define H691CB5AE_0772_4832_9968_9A8D11A0F02B

#include <memory>

namespace net {
    class TcpSocket;
    class TcpConnectionReceiverListener {
    public:
        virtual ~TcpConnectionReceiverListener() {}

        // Transfer new_incoming_connection ownership to listener
        virtual void OnNewConnection(std::unique_ptr<TcpSocket> new_incoming_socket) = 0;
        virtual void OnListenError() = 0;
    };
}

#endif /* H691CB5AE_0772_4832_9968_9A8D11A0F02B */
