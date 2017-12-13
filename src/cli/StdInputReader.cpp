
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <string>

#include <cli/StdInputReader.h>
#include <cli/StdInputReaderListener.h>
#include <utils/Poller.h>


using namespace cli;

StdInputReader::StdInputReader(utils::Poller* poller)
        : poller_(poller), listener_(nullptr), std_input_fd_(STDIN_FILENO) {
    // fd for this subscriber is stdin

    poller_->Subscribe(this);
}

StdInputReader::~StdInputReader() {
    poller_->Unsubscribe(this);
}

const utils::FileDescriptor& StdInputReader::GetFileDescriptor() const {
    return std_input_fd_; // Use the stdin to detect a new input data
}

void StdInputReader::OnIn() {
    std::string buffer;
    for (;;) {
        char temp_buffer[32];
        bzero(temp_buffer, sizeof(temp_buffer));

        ssize_t count = read(std_input_fd_, temp_buffer, sizeof(temp_buffer)-1);
        // the temp_buffer always is a null terminated string with max size 31
        if (count == -1 && errno == EAGAIN) {
            // All data is read
            buffer.pop_back(); // remove last symbol "/n"
            listener_->OnMessageRead(buffer);
            break;
        }
        else if (count <= 0) {
            // End of file
            listener_->OnReaderClosed();
            break;
        }

        // Copy buffer
        buffer += temp_buffer;
    }
}

void StdInputReader::OnError() {
    listener_->OnReaderClosed();
}

void StdInputReader::OnOut() {
}

void cli::StdInputReader::SetListener(StdInputReaderListener* listener) {
    listener_ = listener;
}

void StdInputReader::OnClosed() {
    listener_->OnReaderClosed();

}
