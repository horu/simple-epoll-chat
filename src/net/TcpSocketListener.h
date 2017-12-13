/*
 * TcpSocketListener.h
 *
 */

#ifndef H41D612BD_BA45_4760_BD83_CE69D05DCCFB
#define H41D612BD_BA45_4760_BD83_CE69D05DCCFB

#include <net/TcpSocket.h>

namespace net {
    class TcpSocket;
    class TcpSocketListener {
    public:
        virtual ~TcpSocketListener() {
        }

        virtual void OnDataReceived(const DataBuffer& data) = 0;
        virtual void OnConnected() = 0;
        virtual void OnConnectionTimeout() = 0;
        virtual void OnError() = 0;
        virtual void OnClosed() = 0;
    };
}

#endif /* H41D612BD_BA45_4760_BD83_CE69D05DCCFB */
