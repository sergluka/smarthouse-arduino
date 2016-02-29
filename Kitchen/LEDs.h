#ifndef ARDUINO_LEDS_H
#define ARDUINO_LEDS_H

#include <LEDFader.h>
#include <Logging.h>

#include "Types.h"
#include "Parsing.h"

#define PIN_IN_RED      3
#define PIN_IN_GREEN    5
#define PIN_IN_BLUE     6
#define PIN_IN_WHITE    9

#define FADE_TIME_MS    3000

struct TransitionRunning
{
    bool running;
    Transition transition;
    bool direction;
} trans_run[LedType::MAX_SIZE];

LEDFader ledR(PIN_IN_RED);
LEDFader ledG(PIN_IN_GREEN);
LEDFader ledB(PIN_IN_BLUE);
LEDFader ledW(PIN_IN_WHITE);

void led_process_loop();
void leds_color_fade(const RGBW & start, const RGBW & stop, unsigned int time);
void leds_white_fade(const RGBW & start, const RGBW & stop, unsigned int time);

bool is_leds_has_colors(const RGBW & color);
bool is_leds_has_white(const RGBW & color);

void leds_process()
{
    ledR.update();
    ledG.update();
    ledB.update();
    ledW.update();

    led_process_loop();
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

void leds_color_fade(const RGBW & start, const RGBW & stop, unsigned int time)
{
    LOG_INFO("LEDs are fading R=0x%x, G=0x%x, B=0x%x => R=0x%x, G=0x%x, B=0x%x, time=%ums",
             start.R, start.G, start.B, stop.R, stop.G, stop.B, time);
    ledR.set_value(start.R);
    ledG.set_value(start.G);
    ledB.set_value(start.B);
    ledR.fade(stop.R, time);
    ledG.fade(stop.G, time);
    ledB.fade(stop.B, time);
}

void leds_white_fade(const RGBW & start, const RGBW & stop, unsigned int time)
{
    LOG_INFO("LEDs are fading W=0x%x => W=0x%x, time=%ums", start.W, stop.W, time);
    ledW.set_value(start.W);
    ledW.fade(stop.W, time);
}

void leds_color_fade(byte R, byte G, byte B, unsigned int time = FADE_TIME_MS)
{
    ledR.fade(R, time);
    ledG.fade(G, time);
    ledB.fade(B, time);
    LOG_INFO("LEDs are fading to R=0x%x, G=0x%x, B=0x%x, time=%ums", R, G, B, time);
}

void leds_white_fade(byte W, unsigned int time = FADE_TIME_MS)
{
    ledW.fade(W, time);
    LOG_INFO("LED are fading to W=0x%x, time=%ums", W, time);
}

void leds_fade(byte R, byte G, byte B, byte W, unsigned int time = FADE_TIME_MS)
{
    leds_color_fade(R, G, B, time);
    leds_white_fade(W, time);
}

bool is_leds_on()
{
    return ledR.get_value() > 0 || ledG.get_value() > 0 || ledB.get_value() > 0 ||
        ledW.get_value() > 0;
}

bool is_leds_has_colors(const RGBW & color)
{
    return ledR.get_value() == color.R && ledG.get_value() == color.G && ledB.get_value() == color.B;
}

bool is_leds_has_white(const RGBW & color)
{
    return ledW.get_value() == color.W;
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

#endif //ARDUINO_LEDS_H
