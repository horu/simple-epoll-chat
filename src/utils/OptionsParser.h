
#ifndef HE1CDB70A_643D_4A70_B47A_3E54F1F84E98
#define HE1CDB70A_643D_4A70_B47A_3E54F1F84E98

#include <string>

namespace utils {

    // The parser command oprions of server.
    class ServerOptionsParser {
    public:
        ServerOptionsParser();
        virtual ~ServerOptionsParser();

        // Throw utls::Exception on error.
        void Parse(int argc, char** argv);

        bool Deamon() const;
        bool ForceStart() const;
        const std::string& GetLockFileDir() const;
        const std::string& GetPort() const;

    private:
        void PrintUsage();

        bool daemon_;
        bool force_start_;
        std::string lock_file_dir_;
        std::string port_;
    };

} /* namespace utils */

#endif /* HE1CDB70A_643D_4A70_B47A_3E54F1F84E98 */
