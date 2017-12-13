
#ifndef H99BBB7C5_F0A1_4213_909F_1260A0595235
#define H99BBB7C5_F0A1_4213_909F_1260A0595235

#include <stddef.h>
#include <list>
#include <memory>
#include <string>

#include <chat/ServerConnectionListener.h>
#include <net/TcpConnectionReceiverListener.h>

namespace net {
    class TcpConnectionReceiver;
} /* namespace net */


namespace chat {

    // The Server receives incoming messages from clients, generates the events on the
    // new incoming connections and sends them to other clients.
    class Server : private net::TcpConnectionReceiverListener, private ServerConnectionListener {
    public:
        Server(std::unique_ptr<net::TcpConnectionReceiver> connection_receiver,
               size_t incoming_connections_limit);
        virtual ~Server();

        Server(const Server& copy) = delete;
        Server& operator =(const Server&) = delete;
    private:
        typedef std::unique_ptr<ServerConnection> ConnectionPtr;
        typedef std::list<ConnectionPtr> IncomingConnections;

        // TcpConnectionReceiverListener interface
        void OnNewConnection(std::unique_ptr<net::TcpSocket> new_incoming_socket) override;
        void OnListenError() override;

        // ConnectionListener interface
        // Data
        void OnTextMessage(ServerConnection* connection, const std::string& text_message) override;

        // Network events
        void OnConnectionClosed(ServerConnection* connection) override;

        void RemoveClient(ServerConnection* connection);
        void NotifyServerInfo();
        void NotifyClientLeave(const std::string& nick_name);

        std::unique_ptr<net::TcpConnectionReceiver> connection_receiver_;
        IncomingConnections incoming_connections_;
        size_t incoming_connections_counter_; // Just a count of all incoming connections for nick name generations

        const size_t incoming_connections_limit_;
    };

} /* namespace chat */

#endif /* H99BBB7C5_F0A1_4213_909F_1260A0595235 */
