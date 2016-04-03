#include "NewButton.h"

#include <Arduino.h>

NewButton::NewButton(unsigned int pin, unsigned int short_press_time, unsigned int long_press_time):
    m_pin{pin},
    m_short_press_time{short_press_time},
    m_long_press_time{long_press_time},

    m_prev_status{false},
    m_pressed_time{0},
    m_prev_press_status{Status::NONE},
    m_press_status{Status::NONE},
    m_callbacks{}
{
}

void NewButton::on_short_press(on_event_t callback, void * user_data)
{
    m_callbacks[EventType::SHORT_PRESS] = {callback, user_data};
}

void NewButton::on_long_press(on_event_t callback, void * user_data)
{
    m_callbacks[EventType::LONG_PRESS] = {callback, user_data};
}

void NewButton::on_short_release(on_event_t callback, void * user_data)
{
    m_callbacks[EventType::SHORT_RELEASE] = {callback, user_data};
}

void NewButton::on_long_release(on_event_t callback, void * user_data)
{
    m_callbacks[EventType::LONG_RELEASE] = {callback, user_data};
}

void NewButton::setup()
{
    pinMode(m_pin, INPUT);
}

void NewButton::process()
{
    bool status = digitalRead(m_pin) == HIGH;
    // on press
    if (status && !m_prev_status) {
        m_pressed_time = millis();
    }
        // on release
    else if (!status && m_prev_status) {
        m_pressed_time = 0;
        if (m_press_status == Status::SHORT && m_callbacks[EventType::SHORT_RELEASE].cb) {
            m_callbacks[EventType::SHORT_RELEASE].cb(m_callbacks[EventType::SHORT_RELEASE].user_data);
        }
        if (m_press_status == Status::LONG && m_callbacks[EventType::LONG_RELEASE].cb) {
            m_callbacks[EventType::LONG_RELEASE].cb(m_callbacks[EventType::LONG_RELEASE].user_data);
        }
        m_press_status = Status::NONE;
    }

    if (m_pressed_time > 0) {
        unsigned long hold_time = millis() - m_pressed_time;
        if (hold_time >= m_long_press_time) {
            m_press_status = Status::LONG;
        }
        else if (hold_time >= m_short_press_time) {
            m_press_status = Status::SHORT;
        }
    }

    if (m_prev_press_status != m_press_status) {
        if (m_press_status == Status::SHORT && m_callbacks[EventType::SHORT_PRESS].cb) {
            m_callbacks[EventType::SHORT_PRESS].cb(m_callbacks[EventType::SHORT_PRESS].user_data);
        }
        if (m_press_status == Status::LONG && m_callbacks[EventType::LONG_PRESS].cb) {
            m_callbacks[EventType::LONG_PRESS].cb(m_callbacks[EventType::LONG_PRESS].user_data);
        }
    }

    m_prev_status = status;
    m_prev_press_status = m_press_status;
}
