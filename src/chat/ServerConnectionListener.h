
#ifndef HDCF88770_ECC2_40F9_BFB4_36FD07456AC2
#define HDCF88770_ECC2_40F9_BFB4_36FD07456AC2

#include <string>

namespace chat {
    class ServerConnection;

    class ServerConnectionListener {
    public:
        virtual ~ServerConnectionListener() {}

        // Data
        virtual void OnTextMessage(ServerConnection* connection, const std::string& text_message) = 0;

        // Network events
        virtual void OnConnectionClosed(ServerConnection* connection) = 0;
    };

} /* namespace chat */

#endif /* HDCF88770_ECC2_40F9_BFB4_36FD07456AC2 */
