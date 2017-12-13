
#ifndef HB789294E_9A94_480F_BF57_5C1DE974C960
#define HB789294E_9A94_480F_BF57_5C1DE974C960

#include <stddef.h>
#include <memory>
#include <string>

#include <chat/Parser.h>
#include <net/TcpSocket.h>
#include <net/TcpSocketListener.h>

namespace chat {
    class ServerConnectionListener;

    // This class associated with connection on the server side
    class ServerConnection : private net::TcpSocketListener, private Parser {
    public:
        ServerConnection(std::unique_ptr<net::TcpSocket> socket,
                         const std::string& nick_name,
                         ServerConnectionListener* listener);
        virtual ~ServerConnection();

        const std::string& GetNickName() const;

        void SendTextMessage(const std::string& sender_nick_name, const std::string& text_message);
        void SendClientLeaveMessage(const std::string& nick_name);
        void SendServerInfoMessage(size_t clients_count);

        ServerConnection(const ServerConnection& copy) = delete;
        ServerConnection& operator =(const ServerConnection&) = delete;
    private:

        // Parse callback
        void OnTextMessageToServer(const std::string& text_message) override;

        //TcpSocketListener interface
        void OnDataReceived(const net::DataBuffer& data) override;
        void OnConnected() override;
        void OnConnectionTimeout() override;
        void OnError() override;
        void OnClosed() override;

        std::unique_ptr<net::TcpSocket> socket_;
        const std::string nick_name_;
        ServerConnectionListener* const listener_;
    };

} /* namespace chat */

#endif /* HB789294E_9A94_480F_BF57_5C1DE974C960 */
