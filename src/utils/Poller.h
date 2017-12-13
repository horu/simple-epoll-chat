/*
 * Poller.h
 *
 */

#ifndef H8D324546_DE26_4BD2_BD33_156293323FBB
#define H8D324546_DE26_4BD2_BD33_156293323FBB

#include <stddef.h>

#include <set>


struct epoll_event;

namespace utils {
    typedef int FileDescriptor;
    class PollerSubscriber;

    // This class is epoll wrapper, this is a base element of the system.
    // It polls the fd events and transmits them to TcpScoket/TcpConnectionReceiver/Timer/StdInputReader
    class Poller {
    public:
        Poller();
        virtual ~Poller();

        //Subscribe to the poller to receive event notifications associated with the subscriber fd
        void Subscribe(PollerSubscriber* subscriber);
        //Remove subscription
        void Unsubscribe(PollerSubscriber* subscriber);

        void Run();
        void Stop();

        Poller(const Poller& copy) = delete;
        Poller& operator =(const Poller&) = delete;
    private:
        typedef std::set<PollerSubscriber*> Subscribers;

        static const size_t MAX_EVENTS = 64;

        void MakeFileDescriptorNonBlocking(const FileDescriptor& fd) const;
        void NotifySubscriber(const epoll_event& ev) const;

        bool SubscriberIsExist(PollerSubscriber* subscriber) const;

        // Find PollerSubscriber in list of subscribers to notify about the event
        PollerSubscriber* GetSubscriberByFileDescriptor(const FileDescriptor& fd) const;

        FileDescriptor epoll_;

        Subscribers subscribers_;

        bool is_stopped_;
    };

}

#endif /* H8D324546_DE26_4BD2_BD33_156293323FBB */
