
#ifndef HF43AE666_9B2D_44F6_800F_42847E4D4820
#define HF43AE666_9B2D_44F6_800F_42847E4D4820

#include <stddef.h>
#include <string>
#include <list>

namespace cli {

    // The writer to the stdout.
    class StdOutputWriter {
    public:
        StdOutputWriter();
        virtual ~StdOutputWriter();

        void WriteMessage(const std::string& message);
        void WriteError(const std::string& error);
        void UpdateClientsCount(size_t clients_count);
        void UpdateConnectionState(const std::string& state);
        void UpdateClientInfo(const std::string& info);
        void ResetInputLine();

        StdOutputWriter(const StdOutputWriter& copy) = delete;
        StdOutputWriter& operator =(const StdOutputWriter&) = delete;
    private:
        static const size_t MESSAGE_LIMIT = 100;

        size_t GetScreenHeight() const;
        size_t GetScreenWidth() const;
        void PrintText(std::string text);
        void UpdateScreen();

        std::list<std::string> messages_;
        std::string clients_count_line_;
        std::string connection_state_line_;
        std::string info_line_;
    };

} /* namespace cli */

#endif /* HF43AE666_9B2D_44F6_800F_42847E4D4820 */
