#include <chat/Server.h>
#include <net/SocketAddress.h>
#include <net/TcpConnectionReceiver.h>
#include <stddef.h>
#include <syslog.h>
#include <utils/Exception.h>
#include <utils/OptionsParser.h>
#include <utils/Poller.h>
#include <utils/Process.h>

#include <algorithm>
#include <iostream>
#include <memory>


const size_t INCOMING_CONNECTIONS_LIMIT = 10;



int main(int argc, char** argv) {
    utils::ServerOptionsParser options;
    try {
        // Parse command options
        options.Parse(argc, argv);

        // Parse port number
        net::SocketAddress bind_address("0.0.0.0", options.GetPort());

        if (options.Deamon()) {
            // Make the daemon process
            utils::Process::BecomeDaemon();
        }

        // Make the single instance process
        utils::Process::BecomeSigleInstance(options.GetLockFileDir(), options.ForceStart());

        std::unique_ptr<utils::Poller> poller(new utils::Poller());

        std::unique_ptr<net::TcpConnectionReceiver>
        connection_receiver(new net::TcpConnectionReceiver(poller.get(),
                                                           INCOMING_CONNECTIONS_LIMIT));
        // Bind port
        connection_receiver->Bind(bind_address);

        std::unique_ptr<chat::Server> server(new chat::Server(std::move(connection_receiver),
                                                              INCOMING_CONNECTIONS_LIMIT));

        poller->Run();

        server.reset();
    } catch (utils::Exception& e) {
        if (options.Deamon()) {
            syslog(LOG_ERR, "%s", e.what());
            closelog();
        }
        else {
            std::cerr << e.what() << std::endl;
        }

        return 1;
    }
}
