
#ifndef H93B137FE_49D2_472C_9B16_224CF4694580
#define H93B137FE_49D2_472C_9B16_224CF4694580

#include <stddef.h>
#include <utils/PollerSubscriber.h>

namespace utils {
    typedef size_t Timeout; // millisec
    class TimerListener;
    class Poller;

    // This class is timerfd wrapper.
    class Timer : private PollerSubscriber {
    public:
        Timer(Poller* poller, TimerListener* listener, Timeout timeout);
        virtual ~Timer();

        Timer(const Timer& copy) = delete;
        Timer& operator =(const Timer&) = delete;
    private:
        // PollerSubscriber interface
        const FileDescriptor& GetFileDescriptor() const override;
        void OnIn() override;
        void OnOut() override;
        void OnError() override;
        void OnClosed() override;

        FileDescriptor timer_fd_;

        Poller* const poller_;
        TimerListener* const listener_;
    };
}

#endif /* H93B137FE_49D2_472C_9B16_224CF4694580 */
