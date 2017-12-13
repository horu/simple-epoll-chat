/*
 * TimerListener.h
 *
 */

#ifndef H3A01C815_5E4B_41B0_972C_8F2487C365F5
#define H3A01C815_5E4B_41B0_972C_8F2487C365F5

namespace utils {
    class Timer;

    class TimerListener {
    public:
        virtual ~TimerListener() {}

        virtual void OnTimeout(Timer* timer) = 0;
    };
}

#endif /* H3A01C815_5E4B_41B0_972C_8F2487C365F5 */
