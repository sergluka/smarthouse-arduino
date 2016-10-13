#ifndef ARDUINO_NEWBUTTON_H
#define ARDUINO_NEWBUTTON_H

class NewButton
{
public:
    typedef void(* on_event_t)(void * user_data);

    NewButton(unsigned int pin,
              unsigned int short_press_time = 40,
              unsigned int long_press_time = 2000);

    void on_short_press(on_event_t callback, void * user_data = nullptr);
    void on_long_press(on_event_t callback, void * user_data = nullptr);
    void on_short_release(on_event_t callback, void * user_data = nullptr);
    void on_long_release(on_event_t callback, void * user_data = nullptr);

    void setup();
    void process();

private:
    enum class Status {
        NONE,
        SHORT,
        LONG,
    };

    enum EventType {
        SHORT_PRESS,
        LONG_PRESS,
        SHORT_RELEASE,
        LONG_RELEASE,
        SIZE
    };

    struct Callback {
        on_event_t cb;
        void * user_data;
    };

    unsigned int m_pin;
    unsigned int m_short_press_time;
    unsigned int m_long_press_time;

    bool m_prev_status;
    unsigned long m_pressed_time;
    Status m_prev_press_status;
    Status m_press_status;

    Callback m_callbacks[EventType::SIZE];
};

#endif //ARDUINO_NEWBUTTON_H
