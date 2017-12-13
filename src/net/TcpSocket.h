

#ifndef HC4034956_DDB6_4E81_B915_C0A2622FB67C
#define HC4034956_DDB6_4E81_B915_C0A2622FB67C

#include <memory>
#include <vector>

#include <net/SocketAddress.h>
#include <utils/Poller.h>
#include <utils/PollerSubscriber.h>
#include <utils/Timer.h>
#include <utils/TimerListener.h>

namespace utils {
    class Poller;
}

namespace net {
    class SocketAddress;
    class TcpSocketListener;

    typedef std::vector<char> DataBuffer;

    // This class is tcp socket wrapper. It sends/receives raw data from posix socket,
    // handles socket events and transmits them to ServerConnection/ClientConnection
    class TcpSocket : private utils::PollerSubscriber, private utils::TimerListener {
    public:

        // To outgoing connections
        TcpSocket(utils::Poller* poller);

        // To incoming connections
        TcpSocket(utils::Poller* poller, const utils::FileDescriptor& incoming_socket_fd);

        virtual ~TcpSocket();

        // Throw utils;;Exception on errors
        void Connect(const SocketAddress& peer_address);
        bool IsConnected() const;

        void SetListener(TcpSocketListener* listener);
        void SendData(const DataBuffer& data);

        TcpSocket(const TcpSocket& copy) = delete;
        TcpSocket& operator =(const TcpSocket&) = delete;
    private:
        static const utils::Timeout CONNECT_TIMEOUT = 10000; //ms

        // PollerSubscriber interface
        const utils::FileDescriptor& GetFileDescriptor() const override;
        void OnIn() override;
        void OnOut() override;
        void OnError() override;
        void OnClosed() override;

        // TimerListener interface
        void OnTimeout(utils::Timer* timer) override;

        bool ConnectionIsSuccess() const;

        utils::FileDescriptor connect_socket_fd_;
        bool connected_;

        std::unique_ptr<utils::Timer> connection_timeout_;

        utils::Poller* const poller_;
        TcpSocketListener* listener_;
    };
}

#endif /* HC4034956_DDB6_4E81_B915_C0A2622FB67C */
