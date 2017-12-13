#include <sys/time.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <cstdint>
#include <cstring>
#include <ctime>

#include <utils/Exception.h>
#include <utils/Poller.h>
#include <utils/Timer.h>
#include <utils/TimerListener.h>




using namespace utils;

Timer::Timer(Poller* poller, TimerListener* listener, Timeout timeout)
        : timer_fd_(-1), poller_(poller), listener_(listener) {
    timer_fd_ = timerfd_create(CLOCK_MONOTONIC, 0);
    if (timer_fd_ == -1) {
        throw Exception("timerfd_create", strerror(errno));
    }

    itimerspec timer_spec_new;
    bzero(&timer_spec_new,sizeof(timer_spec_new));
    timer_spec_new.it_value.tv_sec = timeout/1000;
    timer_spec_new.it_value.tv_nsec = 1000*(timeout%1000);

    int setting_result = timerfd_settime(timer_fd_, 0, &timer_spec_new, 0);
    if (setting_result) {
        throw Exception("timerfd_settime", strerror(errno));
    }

    poller_->Subscribe(this);
}

Timer::~Timer() {
    poller_->Unsubscribe(this);
    close(timer_fd_);
}

const FileDescriptor& Timer::GetFileDescriptor() const {
    return timer_fd_;
}

void Timer::OnIn() {
    // Read number of experations from fd to clean it
    uint64_t experations_number;
    while (read(timer_fd_, &experations_number, sizeof(experations_number)) >= 0);

    listener_->OnTimeout(this);
}

void Timer::OnOut() {
}

void Timer::OnError() {
}

void Timer::OnClosed() {
}
