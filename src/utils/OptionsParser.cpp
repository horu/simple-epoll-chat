#include <getopt.h>
#include <cctype>

#include <utils/Exception.h>
#include <utils/OptionsParser.h>

using namespace utils;

ServerOptionsParser::ServerOptionsParser()
    :daemon_(false),
     force_start_(false),
     lock_file_dir_("/tmp/"){

}

void ServerOptionsParser::PrintUsage() {
    throw Exception("usage", "chat_server [-hdflp] [-l lock_file_dir] -p bind_port");
}

ServerOptionsParser::~ServerOptionsParser() {
}

void ServerOptionsParser::Parse(int argc, char** argv) {
    if (argc == 1) {
        PrintUsage();
    }

    opterr = 0;
    int c;
    while ((c = getopt(argc, argv, "hdfp:l:")) != -1) {
        switch (c) {
        case 'h':
            PrintUsage();
            break;
        case 'd':
            daemon_ = true;
            break;
        case 'f':
            force_start_ = true;
            break;
        case 'p':
            port_ = optarg;
            break;
        case 'l':
            lock_file_dir_ = optarg;
            break;
        case '?':
            if ((optopt == 'p') || (optopt == 'l')) {
                throw Exception("options", std::string("Option -") + (char) optopt + " requires an argument.");
            } else if (isprint(optopt)) {
                throw Exception("options", std::string("Unknown option '-") + (char) optopt + "'.");
            } else {
                throw Exception("options", std::string("Unknown option character '") + (char) optopt + "'.");
            }
        default:
            throw Exception("options", "error parse");
        }
    }

    if (optind < argc) {
        throw Exception("options", std::string("Non-option argument ") + argv[optind]);
    }

    if (port_.empty()) {
        throw Exception("options", "Requires argument option -p bind_port");
    }
}

bool ServerOptionsParser::Deamon() const {
    return daemon_;
}

bool ServerOptionsParser::ForceStart() const {
    return force_start_;
}

const std::string& ServerOptionsParser::GetLockFileDir() const {
    return lock_file_dir_;
}

const std::string& ServerOptionsParser::GetPort() const {
    return port_;
}
