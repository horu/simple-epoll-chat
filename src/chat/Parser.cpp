
#include <stddef.h>
#include <exception>
#include <string>
#include <vector>

#include <chat/ClientConnection.h>
#include <chat/Parser.h>
#include <net/TcpSocket.h>


using namespace chat;

const size_t Parser::MAX_MESSAGE_LENGTH = 1024;
const char Parser::SEPARATOR = '\0';
const char Parser::TEXT_MESSAGE_TO_CLIENT_HEADER = 'c';
const char Parser::TEXT_MESSAGE_TO_SERVER_HEADER = 's';
const char Parser::CLIENT_LEAVE_MESSAGE_HEADER = 'l';
const char Parser::SERVER_INFO_MESSAGE_HEADER = 'i';

chat::Parser::Parser() :current_message_lenght_(0) {
}

Parser::~Parser() {
}

net::DataBuffer Parser::BuildTextMessageToServer(const std::string& text_message) const {
    // The message template: header\0text\0
    net::DataBuffer result( { TEXT_MESSAGE_TO_SERVER_HEADER, SEPARATOR });
    BuildText(&result, text_message);
    return result;
}

net::DataBuffer Parser::BuildTextMessageToClient(const std::string& nick_name, const std::string& text_message) const {
    // The message template: header\0nick_name\0text\0
    net::DataBuffer result( { TEXT_MESSAGE_TO_CLIENT_HEADER, SEPARATOR });
    BuildText(&result, nick_name);
    BuildText(&result, text_message);
    return result;
}

net::DataBuffer Parser::BuildClientLeaveMessage(const std::string& nick_name) const {
    // The message template: header\0nick_name\0
    net::DataBuffer result( { CLIENT_LEAVE_MESSAGE_HEADER, SEPARATOR });
    BuildText(&result, nick_name);
    return result;
}

net::DataBuffer Parser::BuildServerInfoMessage(size_t clients_count) const {
    net::DataBuffer result( { SERVER_INFO_MESSAGE_HEADER, SEPARATOR });
    // The message template: header\0clients_count\0
    std::string str_client_count = std::to_string(clients_count);
    BuildText(&result, str_client_count);
    return result;
}

void Parser::BuildText(net::DataBuffer* buffer, const std::string& text) const {
    buffer->insert(buffer->end(), text.begin(), text.end());
    buffer->push_back(SEPARATOR);
}



void Parser::ParseData(const net::DataBuffer& data) {
    data_to_parse_.insert(data_to_parse_.end(), data.begin(), data.end());

    while (!data_to_parse_.empty()) {
        // Prepare the parser to process new message
        current_it_byte_ = data_to_parse_.begin();
        current_message_lenght_ = 0;

        try {
            ParseMessage();
        } catch (const ErrorMessage& e) {

        } catch (const IncompleteMessage& e) {
            break; // Don't remove old data and wait new data
        }
        // Remove the processed data
        data_to_parse_.erase(data_to_parse_.begin(), current_it_byte_);
    }
}

void Parser::ParseMessage() {
    switch (ParseHeader()) {
    case TEXT_MESSAGE_TO_SERVER_HEADER: {
        // Detected text message from client
        std::string text_message = ParseText();
        OnTextMessageToServer(text_message);
        break;
    }
    case TEXT_MESSAGE_TO_CLIENT_HEADER: {
        // Detected text message from server
        std::string nick_name = ParseText();
        std::string text_message = ParseText();
        OnTextMessageToClient(nick_name, text_message);
        break;
    }
    case CLIENT_LEAVE_MESSAGE_HEADER: {
        // Detected client leave notification
        std::string nick_name = ParseText();
        OnClientLeaveMessage(nick_name);
        break;
    }
    case SERVER_INFO_MESSAGE_HEADER: {
        // Detected server info notification
        size_t clients_count = ParseInteger();
        OnServerInfoMessage(clients_count);
        break;
    }
    default:
        throw ErrorMessage();
    }
}

char Parser::GetNextByte() {
    if (current_message_lenght_ > MAX_MESSAGE_LENGTH) {
        // Overflow max message length
        throw ErrorMessage();
    }

    if (current_it_byte_ == data_to_parse_.end()) {
        // End of the message not found. Wait new data.
        throw IncompleteMessage();
    }

    current_message_lenght_++;
    return *(current_it_byte_++);
}

char Parser::ParseHeader() {
    char header = GetNextByte();
    if (GetNextByte() != SEPARATOR) { // detect separator after header
        throw ErrorMessage();
    }

    return header;
}

std::string Parser::ParseText() {
    std::string result;
    // While no separator iterate bytes. End of string is separator.
    for (char byte = GetNextByte(); byte != SEPARATOR; byte = GetNextByte()) {
        result.push_back(byte);
    }

    return result;
}

size_t Parser::ParseInteger() {
    size_t result = 0;
    try {
        result = std::stoul(ParseText());
    } catch (const std::exception& e) {
        throw ErrorMessage();
    }
    return result;
}

void Parser::OnTextMessageToServer(const std::string& text_message) {
}

void Parser::OnTextMessageToClient(const std::string& nick_name, const std::string& text_message) {
}

void Parser::OnClientLeaveMessage(const std::string& nick_name) {
}

void Parser::OnServerInfoMessage(size_t clients_count) {
}

void Parser::OnParseError() {
}

