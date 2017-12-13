
#ifndef H61A99DD0_905D_4899_952C_B2D79291C594
#define H61A99DD0_905D_4899_952C_B2D79291C594

#include <stddef.h>
#include <string>

namespace chat {
    class ClientConnection;
    class ClientConnectionListener {
    public:
        virtual ~ClientConnectionListener() {}

        // Data
        virtual void OnTextMessage(ClientConnection* connection,
                                   const std::string& nick_name,
                                   const std::string& text_message) = 0;
        virtual void OnClientLeaveMessage(ClientConnection* connection, const std::string& nick_name) = 0;
        virtual void OnServerInfoMessage(ClientConnection* connection, size_t clients_count) = 0;

        // Network events
        virtual void OnConnected(ClientConnection* connection) = 0;
        virtual void OnConnectionTimeout(ClientConnection* connection) = 0;
        virtual void OnConnectionError(ClientConnection* connection) = 0;
        virtual void OnConnectionClosed(ClientConnection* connection) = 0;

    };

} /* namespace chat */

#endif /* H61A99DD0_905D_4899_952C_B2D79291C594 */
