#include "LEDs.h"

#include <LEDFader.h>
#include <Logging.h>

#include "Types.h"
#include "Parsing.h"
#include "Networking.h"

#define PIN_IN_RED      3
#define PIN_IN_GREEN    5
#define PIN_IN_BLUE     6
#define PIN_IN_WHITE    9

static struct TransitionRunning
{
    bool running;
    Transition transition;
    bool direction;
} trans_run[LedType::MAX_SIZE];

LEDFader ledR(PIN_IN_RED);
LEDFader ledG(PIN_IN_GREEN);
LEDFader ledB(PIN_IN_BLUE);
LEDFader ledW(PIN_IN_WHITE);

bool is_leds_has_colors(const RGBW & color)
{
    return ledR.get_value() == color.R && ledG.get_value() == color.G && ledB.get_value() == color.B;
}

bool is_leds_has_white(const RGBW & color)
{
    return ledW.get_value() == color.W;
}

void led_process_loop()
{
    auto & trans_color = trans_run[LedType::Color];
    if (trans_color.running && trans_color.transition.is_loop) {
        if (is_leds_has_colors(trans_color.transition.stop) && !trans_color.direction) {
            leds_color_fade(trans_color.transition.stop, trans_color.transition.start, trans_color.transition.time);
            trans_color.direction = true;
        }
        else if (is_leds_has_colors(trans_color.transition.start) && trans_color.direction) {
            leds_color_fade(trans_color.transition.start, trans_color.transition.stop, trans_color.transition.time);
            trans_color.direction = false;
        }
    }

    auto & trans_white = trans_run[LedType::White];
    if (trans_white.running && trans_white.transition.is_loop) {
        if (is_leds_has_white(trans_white.transition.stop) && !trans_white.direction) {
            leds_white_fade(trans_white.transition.stop, trans_white.transition.start, trans_white.transition.time);
            trans_white.direction = true;
        }
        else if (is_leds_has_white(trans_white.transition.start) && trans_white.direction) {
            leds_white_fade(trans_white.transition.start, trans_white.transition.stop, trans_white.transition.time);
            trans_white.direction = false;
        }
    }
}

void leds_process()
{
    ledR.update();
    ledG.update();
    ledB.update();
    ledW.update();

    led_process_loop();
}

void leds_setup()
{
    switch_leds_off();
}

void leds_color_fade(const RGBW & start, const RGBW & stop, unsigned long time)
{
    LOG_INFO("LEDs are fading R=0x%x, G=0x%x, B=0x%x => R=0x%x, G=0x%x, B=0x%x, time=%lums",
             start.R, start.G, start.B, stop.R, stop.G, stop.B, time);
    ledR.set_value(start.R);
    ledG.set_value(start.G);
    ledB.set_value(start.B);
    ledR.fade(stop.R, time);
    ledG.fade(stop.G, time);
    ledB.fade(stop.B, time);
}

void leds_white_fade(const RGBW & start, const RGBW & stop, unsigned long time)
{
    LOG_INFO("LEDs are fading W=0x%x => W=0x%x, time=%lums", start.W, stop.W, time);
    ledW.set_value(start.W);
    ledW.fade(stop.W, time);
}

void leds_color_fade(byte R, byte G, byte B, unsigned long time)
{
    ledR.fade(R, time);
    ledG.fade(G, time);
    ledB.fade(B, time);
    LOG_INFO("LEDs are fading to R=0x%x, G=0x%x, B=0x%x, time=%lums", R, G, B, time);
}

void leds_white_fade(byte W, unsigned long time)
{
    ledW.fade(W, time);
    LOG_INFO("LED are fading to W=0x%x, time=%lums", W, time);
}

void leds_fade(byte R, byte G, byte B, byte W, unsigned long time)
{
    leds_color_fade(R, G, B, time);
    leds_white_fade(W, time);
}

void switch_leds_off()
{
    leds_fade(0, 0, 0, 0);
    network_send_color_status(false);
    network_send_white_status(false);
}

bool is_leds_on()
{
    return ledR.get_value() > 0 || ledG.get_value() > 0 || ledB.get_value() > 0 ||
           ledW.get_value() > 0;
}

void leds_set_transition(LedType led_type, const Transition & transition_)
{
    trans_run[led_type].transition = transition_;
}

void leds_start_transition(LedType led_type, bool start)
{
    if (start) {
        auto trans = trans_run[led_type].transition;
        if (led_type == LedType::Color) {
            leds_color_fade(trans.start, trans.stop, trans.time);
        }
        else if (led_type == LedType::White) {
            leds_white_fade(trans.start, trans.stop, trans.time);
        }

        trans_run[led_type].direction = true;

    }
    trans_run[led_type].running = start;
}
