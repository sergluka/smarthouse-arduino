#ifndef ARDUINO_NEWBUTTON_H
#define ARDUINO_NEWBUTTON_H

class NewButton
{
public:
    typedef void(* on_event_t)();

    NewButton(unsigned int pin,
              unsigned int short_press_time = 50,
              unsigned int long_press_time = 2000);

    void on_short_press(on_event_t callback);
    void on_long_press(on_event_t callback);
    void on_short_release(on_event_t callback);
    void on_long_release(on_event_t callback);

    void setup();
    void process();

private:
    enum class Status
    {
        NONE,
        SHORT,
        LONG,
    };

    unsigned int m_pin;
    unsigned int m_short_press_time;
    unsigned int m_long_press_time;

    bool m_prev_status;
    unsigned long m_pressed_time;
    Status m_prev_press_status;
    Status m_press_status;

    on_event_t m_on_short_press_cb;
    on_event_t m_on_long_press_cb;
    on_event_t m_on_short_release_cb;
    on_event_t m_on_long_release_cb;
};


#endif //ARDUINO_NEWBUTTON_H
