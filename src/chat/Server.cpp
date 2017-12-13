#include <algorithm>
#include <functional>

#include <chat/ServerConnection.h>
#include <chat/Server.h>
#include <net/TcpConnectionReceiver.h>
#include <net/TcpSocket.h>

using namespace chat;

Server::Server(std::unique_ptr<net::TcpConnectionReceiver> connection_receiver, size_t incoming_connections_limit)
        : connection_receiver_(connection_receiver.release()),
          incoming_connections_limit_(incoming_connections_limit),
          incoming_connections_counter_(0) {
    connection_receiver_->SetListener(this);
}

Server::~Server() {
}

void Server::OnNewConnection(std::unique_ptr<net::TcpSocket> new_incoming_socket) {
    if (incoming_connections_.size() >= incoming_connections_limit_) {
        // Connections limit is exceeded. Disconnect the client.
        new_incoming_socket.reset();
        return;
    }

    // Increment connection counter
    incoming_connections_counter_++;

    // Generate a nick name for the new client
    std::string new_nick_name = std::string("Client") + std::to_string(incoming_connections_counter_);

    // Save connection
    ConnectionPtr new_incoming_connection(new ServerConnection(std::move(new_incoming_socket),
                                                               new_nick_name,
                                                               this));
    incoming_connections_.push_back(std::move(new_incoming_connection));
    NotifyServerInfo();
}

void Server::OnListenError() {
}

void Server::OnTextMessage(ServerConnection* connection, const std::string& text_message) {
    // Send received text message to all client except the sender
    for (auto& connection_to_send : incoming_connections_) {
        if (connection_to_send.get() != connection) {
            connection_to_send->SendTextMessage(connection->GetNickName(), text_message);
        }
    }
}

void Server::OnConnectionClosed(ServerConnection* connection) {
    RemoveClient(connection);
}

void chat::Server::RemoveClient(ServerConnection* connection) {
    std::string removed_nick_name = connection->GetNickName();
    // Find and erase the connection from list
    incoming_connections_.erase(std::find_if(incoming_connections_.begin(),
                                             incoming_connections_.end(),
                                             [connection](const ConnectionPtr& it) {
                                                 return it.get() == connection;
                                             }));

    NotifyServerInfo();
    NotifyClientLeave(removed_nick_name);
}

void chat::Server::NotifyServerInfo() {
    // Notify clients about new client connection
    for (auto& connection_to_send : incoming_connections_) {
        connection_to_send->SendServerInfoMessage(incoming_connections_.size());
    }
}

void chat::Server::NotifyClientLeave(const std::string& nick_name) {
    // Notify other clients about client leave
    for (auto& connection_to_send : incoming_connections_) {
        connection_to_send->SendClientLeaveMessage(nick_name);
    }
}
