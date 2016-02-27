#include "NewButton.h"

#include <Arduino.h>

NewButton::NewButton(unsigned int pin, unsigned int short_press_time, unsigned int long_press_time):
    m_pin{pin},
    m_short_press_time{short_press_time},
    m_long_press_time{long_press_time},

    m_prev_status{false},
    m_pressed_time{0},
    m_prev_press_status{Status::NONE},
    m_press_status{Status::NONE}
{
}

void NewButton::on_short_press(on_event_t callback)
{
    m_on_short_press_cb = callback;
}

void NewButton::on_long_press(on_event_t callback)
{
    m_on_long_press_cb = callback;
}

void NewButton::on_short_release(on_event_t callback)
{
    m_on_short_release_cb = callback;
}

void NewButton::on_long_release(on_event_t callback)
{
    m_on_long_release_cb = callback;
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
        if (m_press_status == Status::SHORT && m_on_short_release_cb) {
            m_on_short_release_cb();
        }
        if (m_press_status == Status::LONG && m_on_long_release_cb) {
            m_on_long_release_cb();
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
        if (m_press_status == Status::SHORT && m_on_short_press_cb) {
            m_on_short_press_cb();
        }
        if (m_press_status == Status::LONG && m_on_long_press_cb) {
            m_on_long_press_cb();
        }
    }

    m_prev_status = status;
    m_prev_press_status = m_press_status;
}
