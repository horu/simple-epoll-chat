
#ifndef HDCF99D11_B885_4A9B_91EC_85A1EEDFCF34
#define HDCF99D11_B885_4A9B_91EC_85A1EEDFCF34

#include <stddef.h>
#include <string>
#include <vector>

#include <net/TcpSocket.h>

namespace chat {

    // This class is the builder text messages and notifications to bytes data for send to server/client,
    // and parser bytes data.
    class Parser {
    public:
        Parser();
        virtual ~Parser();

        Parser(const Parser& copy) = delete;
        Parser& operator =(const Parser&) = delete;
    protected:
        // Parse all received data
        void ParseData(const net::DataBuffer& data);

        // Parse callback
        virtual void OnTextMessageToServer(const std::string& text_message);
        virtual void OnTextMessageToClient(const std::string& nick_name, const std::string& text_message);
        virtual void OnClientLeaveMessage(const std::string& nick_name);
        virtual void OnServerInfoMessage(size_t clients_count);
        virtual void OnParseError();

        // Building data buffers to send
        net::DataBuffer BuildTextMessageToServer(const std::string& text_message) const;
        net::DataBuffer BuildTextMessageToClient(const std::string& nick_name, const std::string& text_message) const;
        net::DataBuffer BuildClientLeaveMessage(const std::string& nick_name) const;
        net::DataBuffer BuildServerInfoMessage(size_t clients_count) const;

    private:
        // This classes are helpers to break the parsing and indicate about errors
        class ErrorMessage {};
        class IncompleteMessage {};

        // The constants for building/parsing messages
        static const size_t MAX_MESSAGE_LENGTH;
        static const char SEPARATOR;
        static const char TEXT_MESSAGE_TO_CLIENT_HEADER;
        static const char TEXT_MESSAGE_TO_SERVER_HEADER;
        static const char CLIENT_LEAVE_MESSAGE_HEADER;
        static const char SERVER_INFO_MESSAGE_HEADER;

        void BuildText(net::DataBuffer* buffer, const std::string& text) const;

        // Parse one message
        void ParseMessage();
        char GetNextByte();
        char ParseHeader();
        std::string ParseText();
        size_t ParseInteger();

        net::DataBuffer data_to_parse_;
        net::DataBuffer::iterator current_it_byte_;
        size_t current_message_lenght_;
    };

} /* namespace chat */

#endif /* HDCF99D11_B885_4A9B_91EC_85A1EEDFCF34 */
