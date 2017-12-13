
#ifndef H85BA6DDD_80F1_476C_88E5_AA9BD32B6E8F
#define H85BA6DDD_80F1_476C_88E5_AA9BD32B6E8F

#include <utils/Poller.h>

struct epoll_event;
namespace utils {
    class PollerSubscriber {
    public:
        virtual ~PollerSubscriber() {}

        virtual const FileDescriptor& GetFileDescriptor() const = 0;

        virtual void OnIn() = 0;
        virtual void OnOut() = 0;
        virtual void OnError() = 0;
        virtual void OnClosed() = 0;
    };
}

#endif /* H85BA6DDD_80F1_476C_88E5_AA9BD32B6E8F */
