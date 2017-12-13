
#ifndef H751A45F7_B5DC_4FE9_B873_FAC526F84067
#define H751A45F7_B5DC_4FE9_B873_FAC526F84067

#include <utils/PollerSubscriber.h>

namespace utils {
    class Poller;
}

namespace cli {
    class StdInputReaderListener;

    // This is STDIN_FILENO wrapper. Provides async read from stdin.
    class StdInputReader : private utils::PollerSubscriber {
    public:
        StdInputReader(utils::Poller* poller);
        virtual ~StdInputReader();

        void SetListener(StdInputReaderListener* listener);

        StdInputReader(const StdInputReader& copy) = delete;
        StdInputReader& operator =(const StdInputReader&) = delete;
    private:
        // PollerSubscriber interface
        const utils::FileDescriptor& GetFileDescriptor() const override;
        void OnIn() override;
        void OnOut() override;
        void OnError() override;
        void OnClosed() override;

        utils::Poller* const poller_;
        StdInputReaderListener* listener_;

        utils::FileDescriptor std_input_fd_;
    };
}
#endif /* H751A45F7_B5DC_4FE9_B873_FAC526F84067 */
