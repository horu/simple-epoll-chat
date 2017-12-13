
#ifndef H31AD1A18_C3B9_41A1_86F8_A2B2AAAFD0DC
#define H31AD1A18_C3B9_41A1_86F8_A2B2AAAFD0DC

#include <exception>
#include <string>

namespace utils {

    // Uses for signal about uncultivated errors in the application
    class Exception : std::exception {
    public:
        Exception(const std::string& source_error, const std::string& error_description);
        ~Exception();

        const char* what() const _GLIBCXX_USE_NOEXCEPT override;

    private:
        std::string what_;
    };

} /* namespace poller */

#endif /* H31AD1A18_C3B9_41A1_86F8_A2B2AAAFD0DC */
