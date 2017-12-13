
#ifndef H6041E30E_C2C8_4C44_A0B3_88221C887D77
#define H6041E30E_C2C8_4C44_A0B3_88221C887D77

#include <signal.h>
#include <utils/Poller.h>

namespace utils {

    // The class provide single instance process and daemon for the server.
    class Process {
    public:
        // BecomeDaemon must be called before BecomeSigleInstance
        static void BecomeDaemon();

        //Throw utls::Exception on error.
        static void BecomeSigleInstance(const std::string& lock_file_dir, bool force_start);

    private:
        static const char LOCK_FILE_NAME[];
        static FileDescriptor lock_file_;

        static bool Lock();
        static void KillLockerProcess();

    };

} /* namespace utils */

#endif /* H6041E30E_C2C8_4C44_A0B3_88221C887D77 */
