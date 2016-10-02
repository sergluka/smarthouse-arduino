#ifndef ARDUINO_NEWBUTTON_H
#define ARDUINO_NEWBUTTON_H

#include <gmock/gmock.h>
#include <base/NiceMock.h>

using namespace testing;

class NewButton : public NiceMock<void>
{
public:
    typedef void(* on_event_t)(void * user_data);

    NewButton(unsigned int pin, unsigned int short_press_time = 40, unsigned int long_press_time = 2000)
    {
        ON_CALL(*this, process()).WillByDefault(Return());
        allow(this);
    }

    MOCK_METHOD2(on_short_press, void(on_event_t callback, void * user_data));
    MOCK_METHOD2(on_long_press, void(on_event_t callback, void * user_data));
    MOCK_METHOD2(on_short_release, void(on_event_t callback, void * user_data));
    MOCK_METHOD2(on_long_release, void(on_event_t callback, void * user_data));

    MOCK_METHOD0(setup, void());
    MOCK_METHOD0(process, void());
};

#endif //ARDUINO_NEWBUTTON_H
