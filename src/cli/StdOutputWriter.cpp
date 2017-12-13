#include <sys/ioctl.h>
#include <cerrno>
#include <cstring>
#include <iostream>

#include <cli/StdOutputWriter.h>
#include <utils/Exception.h>

using namespace cli;

StdOutputWriter::StdOutputWriter() {
    // Clear screen
    for (int i = 0; i < GetScreenHeight(); i++) {
        std::cout << std::endl;
    }
    ResetInputLine();
}

StdOutputWriter::~StdOutputWriter() {
}


void StdOutputWriter::UpdateScreen() {
    // Update all lines on the screen after a some event

    std::cout << "\e[s"; // save cursor
    // Update clients info
    PrintText(" ");
    PrintText(info_line_);
    PrintText(connection_state_line_);
    PrintText(clients_count_line_);
    PrintText(" ");

    // Update messages lines
    for (const auto& message_to_print : messages_) {
        PrintText(message_to_print);
    }
    std::cout << "\e[u"; // restore cursor

    std::cout.flush();
}

void cli::StdOutputWriter::WriteMessage(const std::string& message) {
    if (messages_.size() >= MESSAGE_LIMIT) {
        messages_.pop_back();
    }
    messages_.push_front(message);
    UpdateScreen();
}

void cli::StdOutputWriter::UpdateClientsCount(size_t clients_count) {
    clients_count_line_ = std::string("Clients count: ") + std::to_string(clients_count);
    UpdateScreen();
}

void cli::StdOutputWriter::WriteError(const std::string& error) {
    std::cerr << std::endl;
    std::cerr << error << std::endl;
}

void cli::StdOutputWriter::ResetInputLine() {
    std::cout << "\e[1F"; // cursor up
    std::cout << "\e[1M"; // remove line
    UpdateScreen();
}

void cli::StdOutputWriter::UpdateConnectionState(const std::string& state) {
    connection_state_line_ = std::string("Connection state: ") + state;
    UpdateScreen();
}

void cli::StdOutputWriter::UpdateClientInfo(const std::string& info) {
    info_line_ = info;
    UpdateScreen();
}

size_t StdOutputWriter::GetScreenHeight() const {
    struct winsize w;
    int io_result = ioctl(0, TIOCGWINSZ, &w);
    if (io_result) {
        throw utils::Exception("ioctl", strerror(errno));
    }
    return w.ws_row;
}

size_t StdOutputWriter::GetScreenWidth() const {
    struct winsize w;
    int io_result = ioctl(0, TIOCGWINSZ, &w);
    if (io_result) {
        throw utils::Exception("ioctl", strerror(errno));
    }
    return w.ws_col;
}

void StdOutputWriter::PrintText(std::string text) {
    // Update a value of line on the screen
    size_t screen_width = GetScreenWidth();

    // Split text on lines if the text too long
    std::list<std::string> parts_to_print;
    if (text.empty()) {
        parts_to_print.push_front(" ");
    }
    while (!text.empty()) {
        std::string part_to_print;
        if (text.size() > screen_width) {
            part_to_print = text.substr(0, screen_width);
        }
        else {
            part_to_print = text;
        }
        text.erase(0, part_to_print.size());
        parts_to_print.push_front(part_to_print);
    }

    // Print parts of text
    for (const auto& part_to_print : parts_to_print) {
        std::cout << "\e[1F"; // cursor up
        std::cout << "\e[2K"; // erase line
        std::cout << part_to_print;
    }
}

