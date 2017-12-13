
#include <chat/ServerConnection.h>
#include <chat/ServerConnectionListener.h>

using namespace chat;

ServerConnection::ServerConnection(std::unique_ptr<net::TcpSocket> socket,
                                   const std::string& nick_name,
                                   ServerConnectionListener* listener)
        : socket_(socket.release()), nick_name_(nick_name), listener_(listener) {
    socket_->SetListener(this);
}

ServerConnection::~ServerConnection() {
}

const std::string& chat::ServerConnection::GetNickName() const {
    return nick_name_;
}


void ServerConnection::SendTextMessage(const std::string& sender_nick_name, const std::string& text_message) {
    socket_->SendData(BuildTextMessageToClient(sender_nick_name, text_message));
}

void ServerConnection::SendClientLeaveMessage(const std::string& nick_name) {
    socket_->SendData(BuildClientLeaveMessage(nick_name));
}

void ServerConnection::SendServerInfoMessage(size_t clients_count) {
    socket_->SendData(BuildServerInfoMessage(clients_count));
}


void ServerConnection::OnDataReceived(const net::DataBuffer& data) {
    ParseData(data);
}

void ServerConnection::OnTextMessageToServer(const std::string& text_message) {
    listener_->OnTextMessage(this, text_message);
}

void ServerConnection::OnError() {
    listener_->OnConnectionClosed(this);
}

void ServerConnection::OnConnected() {
    // Ignore
}

void ServerConnection::OnConnectionTimeout() {
    // Ignore
}

void ServerConnection::OnClosed() {
    listener_->OnConnectionClosed(this);
}
