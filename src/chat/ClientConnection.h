#ifndef H48CB4343_8418_4E8B_BA73_09B3AD3539ED
#define H48CB4343_8418_4E8B_BA73_09B3AD3539ED

#include <memory>
#include <string>
#include <stddef.h>

#include <chat/Parser.h>
#include <net/TcpSocket.h>
#include <net/TcpSocketListener.h>

namespace chat {
    class ClientConnectionListener;

    // This class associated with connection on the client side.
    class ClientConnection : private net::TcpSocketListener, private Parser {
    public:
        ClientConnection(std::unique_ptr<net::TcpSocket> socket);
        virtual ~ClientConnection();

        void SetListener(ClientConnectionListener* listener);

        void SendTextMessage(const std::string& text_message);

        bool IsConnected() const;

        ClientConnection(const ClientConnection& copy) = delete;
        ClientConnection& operator =(const ClientConnection&) = delete;
    private:

        // Parse callback
        void OnTextMessageToClient(const std::string& nick_name, const std::string& text_message) override;
        void OnClientLeaveMessage(const std::string& nick_name) override;
        void OnServerInfoMessage(size_t clients_count) override;

        //TcpSocketListener interface
        void OnDataReceived(const net::DataBuffer& data) override;
        void OnConnected() override;
        void OnConnectionTimeout() override;
        void OnError() override;
        void OnClosed() override;

        std::unique_ptr<net::TcpSocket> socket_;
        ClientConnectionListener* listener_;
    };
}

#endif /* H48CB4343_8418_4E8B_BA73_09B3AD3539ED */
