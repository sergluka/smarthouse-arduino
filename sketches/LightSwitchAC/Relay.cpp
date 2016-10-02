#include "Relay.h"

#include <Logging.h>

Relay::Relay(NewButton & button, byte output_pin) :
    m_status{false},
    m_button(button),
    m_output_pin{output_pin}
{
    pinMode(output_pin, OUTPUT);
    m_button.on_short_release(Relay::on_button_short_release, this);
}

void Relay::set_status(bool on)
{
    m_status = on;
    digitalWrite(m_output_pin, on ? HIGH : LOW);
}

void Relay::switch_status()
{
    m_status = !m_status;
    set_status(m_status);
}

void Relay::process()
{
    m_button.process();
}

void Relay::on_button_short_release(void * data)
{
    Relay * relay = static_cast<Relay*>(data);
    if (!relay) {
        LOG_ERROR("User data is not Relay object");
        return;
    }

    relay->switch_status();
}



