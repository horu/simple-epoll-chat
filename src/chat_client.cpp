#include <chat/Client.h>
#include <chat/ClientConnection.h>
#include <cli/StdInputReader.h>
#include <cli/StdOutputWriter.h>
#include <net/SocketAddress.h>
#include <net/TcpSocket.h>
#include <utils/Exception.h>
#include <utils/Poller.h>
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <fstream>

int main(int argc, char** argv) {

    if (argc < 3) {
        std::cerr << "Usage: chat_client server_ip server_port\n";
        return 1;
    }

    // Init Client
    try {
        // Parse the server address
        net::SocketAddress server_address(argv[1], argv[2]);

        // Init poller
        std::unique_ptr<utils::Poller> poller(new utils::Poller());

        // Connect to server
        std::unique_ptr<net::TcpSocket> client_socket(new net::TcpSocket(poller.get()));
        client_socket->Connect(server_address);

        std::unique_ptr<chat::ClientConnection> client_connection(new chat::ClientConnection(std::move(client_socket)));
        std::unique_ptr<cli::StdInputReader> input_reader(new cli::StdInputReader(poller.get()));
        std::unique_ptr<cli::StdOutputWriter> output_writer(new cli::StdOutputWriter());
        std::unique_ptr<chat::Client> client(new chat::Client(std::move(client_connection),
                                                              std::move(input_reader),
                                                              std::move(output_writer),
                                                              poller.get()));
        poller->Run();
        // Close app

        client.reset(); // First remove Client, because it may use Poller on connections closing
        poller.reset();
    } catch (utils::Exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
