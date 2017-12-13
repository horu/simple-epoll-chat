
#ifndef H3037E514_4C6A_4AAE_A759_5DEBD0211848
#define H3037E514_4C6A_4AAE_A759_5DEBD0211848

#include <memory>
#include <string>
#include <stddef.h>

#include <chat/ClientConnectionListener.h>
#include <cli/StdInputReaderListener.h>

namespace utils {
    class Poller;
} /* namespace poller */

namespace cli {
    class StdInputReader;
    class StdOutputWriter;
} /* namespace cli */

namespace chat {
    class ClientConnection;

    /* The Client receives messages from ClientConnection and sends to StdOutputWriter.
       From StdInputReader the messages are sent to the server through ClientConnection.
       The Client handles the network errors - notifies and terminates the application.
     */
    class Client : private ClientConnectionListener, private cli::StdInputReaderListener {
    public:
        // Transfer ownership
        Client(std::unique_ptr<ClientConnection> client_connection,
               std::unique_ptr<cli::StdInputReader> input_reader,
               std::unique_ptr<cli::StdOutputWriter> output_writer,
               utils::Poller* poller);
        virtual ~Client();

        Client(const Client& copy) = delete;
        Client& operator =(const Client&) = delete;
    private:
        // ClientConnectionListener interface
        // Data
        void OnTextMessage(ClientConnection* connection,
                           const std::string& nick_name,
                           const std::string& text_message) override;
        void OnClientLeaveMessage(ClientConnection* connection,const std::string& nick_name) override;
        void OnServerInfoMessage(ClientConnection* connection,size_t clients_count) override;

        // Network events
        void OnConnected(ClientConnection* connection) override;
        void OnConnectionTimeout(ClientConnection* connection) override;
        void OnConnectionError(ClientConnection* connection) override;
        void OnConnectionClosed(ClientConnection* connection) override;

        // StdInputReaderListener interface
        void OnMessageRead(const std::string& message);
        void OnReaderClosed();

        void CloseClient();

        std::unique_ptr<ClientConnection> client_connection_;
        std::unique_ptr<cli::StdInputReader> input_reader_;
        std::unique_ptr<cli::StdOutputWriter> output_writer_;
        utils::Poller* const poller_;
    };

}
#endif /* H3037E514_4C6A_4AAE_A759_5DEBD0211848 */
