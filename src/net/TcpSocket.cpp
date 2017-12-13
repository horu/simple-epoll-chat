#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

#include <utils/Exception.h>
#include <net/TcpSocket.h>
#include <net/TcpSocketListener.h>

using namespace net;

TcpSocket::TcpSocket(utils::Poller* poller)
    :connect_socket_fd_(-1),
     poller_(poller),
     listener_(nullptr),
     connected_(false) {
}

TcpSocket::TcpSocket(utils::Poller* poller, const utils::FileDescriptor& incoming_socket_fd)
    :connect_socket_fd_(incoming_socket_fd),
     poller_(poller),
     listener_(nullptr),
     connected_(true) {
    poller_->Subscribe(this);
}

TcpSocket::~TcpSocket() {
    poller_->Unsubscribe(this);
    close(connect_socket_fd_);
}

bool net::TcpSocket::IsConnected() const {
    return connected_;
}

void TcpSocket::Connect(const SocketAddress& peer_address) {
    // Create socket
    connect_socket_fd_ = socket(PF_INET, SOCK_STREAM, 0);
    if (connect_socket_fd_ == -1) {
        throw utils::Exception("socket", strerror(errno));
    }
    // Subscribe to the poller for
    poller_->Subscribe(this);

    // Connect to server
    sockaddr_in server_sock_addr = peer_address.GetSockAddr();
    int connect_result = connect(connect_socket_fd_, (sockaddr*) &server_sock_addr, sizeof(server_sock_addr));
    if (connect_result && errno != EINPROGRESS) {
        throw utils::Exception("connect", strerror(errno));
    }

    // Start connection timeout
    connection_timeout_.reset(new utils::Timer(poller_, this, CONNECT_TIMEOUT));
}

void TcpSocket::SetListener(TcpSocketListener* listener) {
    listener_ = listener;
}

void TcpSocket::SendData(const DataBuffer& data) {
    ssize_t send_result = send(connect_socket_fd_, data.data(), data.size(), 0);
    if (send_result < 0) {
        //network error
    }
}

const utils::FileDescriptor& TcpSocket::GetFileDescriptor() const {
    return connect_socket_fd_;
}

void TcpSocket::OnIn() {
    DataBuffer buffer;
    for (;;) {
        char temp_buffer[32];

        ssize_t count = read(connect_socket_fd_, temp_buffer, sizeof temp_buffer);
        if (count == -1 && errno == EAGAIN) {
            // All data is read
            if (!buffer.empty()) {
                // if data is empty then notification is no need
                listener_->OnDataReceived(buffer);
            }
            break;
        }
        else if (count == 0) {
            // End of file
            listener_->OnClosed();
            break;
        }
        else if (count == -1) {
            // Error
            listener_->OnError();
            break;
        }

        // Copy data from temp buffer to main buffer
        buffer.insert(buffer.end(), temp_buffer, temp_buffer + count);
    }
}

void TcpSocket::OnOut() {
    // First call OnOut means the process of the connection is completed
    if (!connected_) {
        connection_timeout_.reset();
        // Check connection state
        if (ConnectionIsSuccess()) {
            // Success
            connected_ = true;
            listener_->OnConnected();
        }
        else {
            // Fail
            listener_->OnError();
        }
    }
    // else nothing to do
}

void TcpSocket::OnError() {
    listener_->OnError();
}

void TcpSocket::OnClosed() {
    listener_->OnClosed();
}

void TcpSocket::OnTimeout(utils::Timer* timer) {
    listener_->OnConnectionTimeout();
}

bool TcpSocket::ConnectionIsSuccess() const {
    int result;
    socklen_t result_len = sizeof(result);
    int get_sock_opt_result = getsockopt(connect_socket_fd_, SOL_SOCKET, SO_ERROR, &result, &result_len);
    if (get_sock_opt_result) {
        throw utils::Exception("getsockopt", strerror(errno));
    }

    return result == 0;
}
