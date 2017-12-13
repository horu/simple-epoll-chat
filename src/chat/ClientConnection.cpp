
#include <exception>
#include <iterator>

#include <chat/ClientConnection.h>
#include <chat/ClientConnectionListener.h>
#include <net/TcpSocket.h>

using namespace chat;


ClientConnection::ClientConnection(std::unique_ptr<net::TcpSocket> socket)
        : socket_(socket.release()), listener_(nullptr) {
    socket_->SetListener(this);
}

ClientConnection::~ClientConnection() {
}

void chat::ClientConnection::SetListener(ClientConnectionListener* listener) {
    listener_ = listener;
}


void chat::ClientConnection::SendTextMessage(const std::string& text_message) {
    socket_->SendData(BuildTextMessageToServer(text_message));
}


void ClientConnection::OnDataReceived(const net::DataBuffer& data) {
    ParseData(data);
}

void chat::ClientConnection::OnTextMessageToClient(const std::string& nick_name, const std::string& text_message) {
    listener_->OnTextMessage(this, nick_name, text_message);
}

void chat::ClientConnection::OnClientLeaveMessage(const std::string& nick_name) {
    listener_->OnClientLeaveMessage(this, nick_name);
}

void chat::ClientConnection::OnServerInfoMessage(size_t clients_count) {
    listener_->OnServerInfoMessage(this, clients_count);
}


void chat::ClientConnection::OnConnectionTimeout() {
    listener_->OnConnectionTimeout(this);
}

void ClientConnection::OnConnected() {
    listener_->OnConnected(this);
}

void ClientConnection::OnError() {
    listener_->OnConnectionError(this);
}

void ClientConnection::OnClosed() {
    listener_->OnConnectionClosed(this);
}

bool chat::ClientConnection::IsConnected() const {
    return socket_->IsConnected();
}
