
#ifndef H5556D845_7655_4F9A_A5D1_86D74DA91AE4
#define H5556D845_7655_4F9A_A5D1_86D74DA91AE4

#include <stddef.h>

#include <utils/Poller.h>
#include <utils/PollerSubscriber.h>

namespace net {
    class SocketAddress;
    class TcpConnectionReceiverListener;

    // This is server socket wrapper. It receives new incoming socket connections and
    // transmits them to Server.
    class TcpConnectionReceiver : private utils::PollerSubscriber {
    public:
        TcpConnectionReceiver(utils::Poller* poller, size_t pending_connection_limit);
        virtual ~TcpConnectionReceiver();

        // Throw utils;;Exception on errors
        void Bind(const SocketAddress& bind_address);
        void SetListener(TcpConnectionReceiverListener* listener);

        TcpConnectionReceiver(const TcpConnectionReceiver& copy) = delete;
        TcpConnectionReceiver& operator =(const TcpConnectionReceiver&) = delete;
    private:
        // PollerSubscriber interface
        const utils::FileDescriptor& GetFileDescriptor() const override;
        void OnIn() override;
        void OnOut() override;
        void OnError() override;
        void OnClosed() override;


        utils::Poller* const poller_;
        TcpConnectionReceiverListener* listener_;
        utils::FileDescriptor listen_socket_fd_;
        const size_t pending_connection_limit_;
    };
}
#endif /* H5556D845_7655_4F9A_A5D1_86D74DA91AE4 */
