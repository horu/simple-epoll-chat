/*
 * StdInputReaderListener.h
 *
 */

#ifndef H58E54A61_33B0_47F5_A965_43B97408D8F9
#define H58E54A61_33B0_47F5_A965_43B97408D8F9

#include "string"

namespace cli {

    class StdInputReaderListener {
    public:
        virtual ~StdInputReaderListener() {
        }

        virtual void OnMessageRead(const std::string& message) = 0;
        virtual void OnReaderClosed() = 0;
    };

}

#endif /* H58E54A61_33B0_47F5_A965_43B97408D8F9 */
