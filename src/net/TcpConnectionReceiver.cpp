
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <memory>

#include <net/SocketAddress.h>
#include <net/TcpConnectionReceiver.h>
#include <net/TcpConnectionReceiverListener.h>
#include <net/TcpSocket.h>
#include <utils/Exception.h>

using namespace net;

TcpConnectionReceiver::TcpConnectionReceiver(utils::Poller* poller, size_t pending_connection_limit)
        : listen_socket_fd_(-1),
          poller_(poller),
          pending_connection_limit_(pending_connection_limit),
          listener_(nullptr) {
}

TcpConnectionReceiver::~TcpConnectionReceiver() {
    poller_->Unsubscribe(this);
    close(listen_socket_fd_);
}

void TcpConnectionReceiver::SetListener(TcpConnectionReceiverListener* listener) {
    listener_ = listener;
}

void TcpConnectionReceiver::Bind(const SocketAddress& bind_address) {
    listen_socket_fd_ = socket(PF_INET, SOCK_STREAM, 0);
    if (listen_socket_fd_ == -1){
        throw utils::Exception("socket", strerror(errno));
    }
    poller_->Subscribe(this);

    // Allow reuse local address for bind
    int reuseaddr=1;
    int set_opt_result = setsockopt(listen_socket_fd_, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));
    if (set_opt_result) {
        throw utils::Exception("setsockopt", strerror(errno));
    }

    // Bind socket
    sockaddr_in bind_sock_addr = bind_address.GetSockAddr();
    int bind_result = bind(listen_socket_fd_, (sockaddr*) &bind_sock_addr, sizeof(bind_sock_addr));
    if (bind_result) {
        throw utils::Exception("bind", strerror(errno));
    }

    // Listern socket
    int listen_result = listen(listen_socket_fd_, pending_connection_limit_);
    if (listen_result) {
        throw utils::Exception("listen", strerror(errno));
    }

}

const utils::FileDescriptor& TcpConnectionReceiver::GetFileDescriptor() const {
    return listen_socket_fd_;
}

void TcpConnectionReceiver::OnIn() {
    sockaddr_in remove_sock_addr;
    socklen_t socklen = sizeof(remove_sock_addr);

    // new_incoming_socket_fd may have one or more incoming connections
    for (;;) {
        utils::FileDescriptor new_incoming_socket_fd = accept(listen_socket_fd_,
                                                               (sockaddr*) &remove_sock_addr,
                                                               &socklen);
        if (new_incoming_socket_fd == -1) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                // all incoming connections have been processed
            }
            else {
                // Some error, maybe protocol. Nothing to do.
            }
            break;
        }

        std::unique_ptr<TcpSocket> new_incoming_socket(new TcpSocket(poller_, new_incoming_socket_fd));
        listener_->OnNewConnection(std::move(new_incoming_socket));
    }
}

void TcpConnectionReceiver::OnOut() {
}

void TcpConnectionReceiver::OnError() {
    listener_->OnListenError();
}


void TcpConnectionReceiver::OnClosed() {
    listener_->OnListenError();
}
