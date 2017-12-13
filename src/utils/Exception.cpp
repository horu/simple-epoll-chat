
#include <utils/Exception.h>

namespace utils {


    Exception::Exception(const std::string& source_error, const std::string& error_description)
            : what_(source_error + ": " + error_description) {
    }

    Exception::~Exception() {
    }

    const char* Exception::what() const _GLIBCXX_USE_NOEXCEPT {
        return what_.c_str();
    }

} /* namespace poller */
