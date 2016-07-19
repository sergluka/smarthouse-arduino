# Arduino sketches for nodes controlled via MySensors

## Kitchen (RGBW leds)

There are two MySensors devices, one for color leds (RGB), and one for whites only (W). All devices are controlled independently.
 
MySensors commands:

- V_LIGHT

    - 0 - off,

    - 1 - on, until saved value (see V_VAR1 mode 1)

- V_VAR1 - modes switch in text format:

    - Color change: "0,color" - maximal value change and save for both short button press and next mode (\#1),

    - Fade: "1,color_start,color_end,time" - transition from 'color_start' to 'color_end' per 'time',

    - Pulse: "2,color_start,color_end,time" - transition from 'color_start' to 'color_end' and back to 'color_start' per 'time'


Parameters:

- color - 2 HEX for white leds, and 6 HEX for color leds

    - time - in milliseconds


Button:

- short press (on release). 

    - if some leds are one, switch on all leds,

    - if all leds are on, switch off all leds

- long press (on press), change by the loop which leds are used:

    1. white - on, colors - off

    2. white - off, colors - on

    3. white - on, colors - oon


## LightSwitchAC (Phase control dimmer for 220V) 

MySensors input commands:

- V_LIGHT_LEVEL - set maximum brightness (0 - 100)

- V_LIGHT - on/off

- V_VAR1 - dump internal stats into serial console
 

## Libraries:


Custom library:

- Logging 

- NewButton 

 
Patched library:

- LEDFader, based on https://github.com/jgillick/arduino-LEDFader

 
External libraries:

- SimpleTimer: http://playground.arduino.cc/Code/SimpleTimer

- SoftTimer: https://github.com/prampec/arduino-softtimer

- TimerOne: https://github.com/PaulStoffregen/TimerOne
