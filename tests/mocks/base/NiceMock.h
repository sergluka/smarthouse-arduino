#ifndef ARDUINO_NICEMOCK_H
#define ARDUINO_NICEMOCK_H

#include <gmock/gmock-spec-builders.h>

namespace testing
{
    // From http://stackoverflow.com/questions/22745349/ignoring-mock-calls-during-setup-phase
    //
    // HACK: NiceMock<> is a friend of Mock so we specialize it here to a type that
    // is never used to be able to temporarily make a mock nice. If this feature
    // would just be supported, we wouldn't need this hack...
    template <>
    struct NiceMock<void>
    {
        static void allow(const void * mock)
        {
            Mock::AllowUninterestingCalls(mock);
        }

        static void warn(const void * mock)
        {
            Mock::WarnUninterestingCalls(mock);
        }

        static void fail(const void * mock)
        {
            Mock::FailUninterestingCalls(mock);
        }
    };
}

#endif //ARDUINO_NICEMOCK_H
