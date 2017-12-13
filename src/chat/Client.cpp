
#include <chat/Client.h>
#include <chat/ClientConnection.h>
#include <cli/StdInputReader.h>
#include <cli/StdOutputWriter.h>
#include <utils/Poller.h>

using namespace chat;

Client::Client(std::unique_ptr<ClientConnection> client_connection,
               std::unique_ptr<cli::StdInputReader> input_reader,
               std::unique_ptr<cli::StdOutputWriter> output_writer,
               utils::Poller* poller)
        : client_connection_(client_connection.release()),
          input_reader_(input_reader.release()),
          output_writer_(output_writer.release()),
          poller_(poller) {
    client_connection_->SetListener(this);
    input_reader_->SetListener(this);
    output_writer_->UpdateClientInfo("To exit from the chat press Ctrl+D or Ctrl+C.");
    output_writer_->UpdateConnectionState("Connecting to server...");
}

Client::~Client() {
}

void chat::Client::OnTextMessage(ClientConnection* connection,
                                         const std::string& nick_name,
                                         const std::string& text_message) {
    output_writer_->WriteMessage(nick_name + ": " + text_message);
}

void chat::Client::OnClientLeaveMessage(ClientConnection* connection, const std::string& nick_name) {
    output_writer_->WriteMessage(std::string("*") + nick_name + " has left the chat");
}

void chat::Client::OnServerInfoMessage(ClientConnection* connection, size_t clients_count) {
    output_writer_->UpdateClientsCount(clients_count);
}

void chat::Client::OnConnected(ClientConnection* connection) {
    output_writer_->UpdateConnectionState("Connected to server");
}

void chat::Client::OnConnectionTimeout(ClientConnection* connection) {
    output_writer_->WriteError("Connection timeout.");
    CloseClient();
}

void chat::Client::OnConnectionError(ClientConnection* connection) {
    output_writer_->WriteError("Connection error.");
    CloseClient();
}

void chat::Client::OnConnectionClosed(ClientConnection* connection) {
    output_writer_->WriteError("Connection closed.");
    CloseClient();
}


void chat::Client::OnMessageRead(const std::string& message) {
    // Erase user text from stdout
    output_writer_->ResetInputLine();
    if (!client_connection_->IsConnected()) {
        // Client is not connected
        return;
    }
    output_writer_->WriteMessage(std::string("You : ") + message);
    client_connection_->SendTextMessage(message);
}

void Client::OnReaderClosed() {
    CloseClient();
}

void chat::Client::CloseClient() {
    client_connection_.reset();
    // Break poller to close client
    poller_->Stop();
}
