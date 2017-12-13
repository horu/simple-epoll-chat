/*
 * Poller.cpp
 *
 */

#include <utils/Exception.h>
#include <utils/Poller.h>
#include <utils/PollerSubscriber.h>

#include <fcntl.h>
#include <signal.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <cstring>

using namespace utils;

Poller::Poller()
        : epoll_(-1), is_stopped_(false) {
    signal(SIGPIPE, SIG_IGN); // Ignore SIGPIPE to prevent abort of application

    // Init epol
    epoll_ = epoll_create1(0);
    if (epoll_ == -1) {
        throw Exception("epoll_create", strerror(errno));
    }
}

Poller::~Poller() {
    close(epoll_);
}

void Poller::Subscribe(PollerSubscriber* subscriber) {
    epoll_event event;
    FileDescriptor subscriber_fd = subscriber->GetFileDescriptor();
    event.data.fd = subscriber_fd;
    event.events = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLOUT | EPOLLET;
    MakeFileDescriptorNonBlocking(subscriber_fd);

    // Register fd to epoll
    int addition_result = epoll_ctl(epoll_, EPOLL_CTL_ADD, subscriber_fd, &event);
    if (addition_result == -1) {
        throw Exception("epoll_ctl", strerror(errno));
    }

    subscribers_.insert(subscriber);
}

void Poller::Unsubscribe(PollerSubscriber* subscriber) {
    // Unregister fd from epoll
    int addition_result = epoll_ctl(epoll_, EPOLL_CTL_DEL, subscriber->GetFileDescriptor(), 0);
    if (addition_result == -1) {
        throw Exception("epoll_ctl", strerror(errno));
    }

    subscribers_.erase(subscriber);
}

void Poller::Run(){
    // Buffer where events are returned
    epoll_event events[MAX_EVENTS];

    // The event loop
    for (;;) {
        int n = epoll_wait(epoll_, events, MAX_EVENTS, 1000);
        for (int i = 0; i < n; i++) {
            NotifySubscriber(events[i]);
        }
        if (is_stopped_) {
            // Loop is stopped, need return control to caller function
            break;
        }
    }
}

void Poller::Stop() {
    is_stopped_ = true;
}

void Poller::MakeFileDescriptorNonBlocking(const FileDescriptor& fd) const {
    int flags, s;

    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        throw Exception("fcntl", strerror(errno));
    }

    flags |= O_NONBLOCK;
    s = fcntl(fd, F_SETFL, flags);
    if (s == -1) {
        throw Exception("fcntl", strerror(errno));
    }
}

void Poller::NotifySubscriber(const epoll_event& ev) const {
    PollerSubscriber* subscriber = GetSubscriberByFileDescriptor(ev.data.fd);

    // Detect a notification type and call a handler
    if ((ev.events & EPOLLOUT) && SubscriberIsExist(subscriber)) {
        subscriber->OnOut();
    }
    // After a call subscriber we need to check. The subscriber could be removed during a call processing.
    if ((ev.events & EPOLLIN) && SubscriberIsExist(subscriber)) {
        subscriber->OnIn();
    }
    if ((ev.events & EPOLLHUP) && SubscriberIsExist(subscriber)) {
        subscriber->OnClosed();
    }
    if ((ev.events & EPOLLERR) && SubscriberIsExist(subscriber)) {
        subscriber->OnError();
    }
}

PollerSubscriber* Poller::GetSubscriberByFileDescriptor(const FileDescriptor& fd) const {
    for (PollerSubscriber* subscription : subscribers_) {
        if (subscription->GetFileDescriptor() == fd) {
            return subscription;
        }
    }

    return nullptr;
}

bool Poller::SubscriberIsExist(PollerSubscriber* subscriber) const {
    return subscribers_.find(subscriber) != subscribers_.end();
}
