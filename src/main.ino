/*****************************************************************************
  *                                                                           
  *  Copyright (c) 2024 www.der-frickler.net                   
  *                                                                           
  *                                                                           
  *****************************************************************************
  *                                                                           
  *  
  *                                                                           
  *                                                                           
  *****************************************************************************/

#include <GrblParserC.h>
#include <WS2812FX.h>

#define DEBUG 

#define LED_COUNT 21
#define LED_PIN 4
#define GRBL_FROM 2
#define GRBL_TO 3

WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

String lstate = "";

extern "C" void fnc_putchar(uint8_t c) {
    Serial1.write(c);
}

extern "C" int fnc_getchar() {
    if (Serial1.available()) {
        return Serial1.read();
    }
    return -1;
}

extern "C" void show_state(const char* state) {
    String sstate = state;
    // state will be one of:
    //  Idle Run Hold:(0/1) Jog Home Alarm Check Door(1:0:2) Sleep 

    // available Effects:
    //  https://github.com/kitesurfer1404/WS2812FX/tree/master#effects 
    if (sstate != lstate) {
        if(sstate == "Idle") {
            ws2812fx.setColor(WHITE);
            ws2812fx.setMode(FX_MODE_BREATH);
            ws2812fx.setSpeed(1000);
        } else if(sstate == "Run") {
            ws2812fx.setColor(WHITE);
            ws2812fx.setMode(FX_MODE_LARSON_SCANNER);
            ws2812fx.setSpeed(3000);
        } else if(sstate.startsWith("Hold")) {
            ws2812fx.setColor(RED);
            ws2812fx.setMode(FX_MODE_STATIC);
        } else if(sstate == "Jog") {
            ws2812fx.setColor(WHITE);
            ws2812fx.setMode(FX_MODE_BLINK);
            ws2812fx.setSpeed(500);
        } else if(sstate == "Home") {
            ws2812fx.setColor(WHITE);
            ws2812fx.setMode(FX_MODE_BLINK);
            ws2812fx.setSpeed(500);
        } else if(sstate == "Alarm") {
            ws2812fx.setColor(RED);
            ws2812fx.setMode(FX_MODE_STROBE);
            ws2812fx.setSpeed(500);
        } else if(sstate == "Check") {
            ws2812fx.setColor(RED);
            ws2812fx.setMode(FX_MODE_STROBE);
            ws2812fx.setSpeed(1000);
        } else if(sstate.startsWith("Door")) {
            if(sstate.startsWith("Door:1") || sstate.startsWith("Door:2")) {
                ws2812fx.setColor(WHITE);
                ws2812fx.setMode(FX_MODE_STATIC);
            } else {
                ws2812fx.setColor(WHITE);
                ws2812fx.setMode(FX_MODE_BLINK);
                ws2812fx.setSpeed(500);
            }
        } else if(sstate == "Sleep") {
            ws2812fx.setColor(RED);
            ws2812fx.setMode(FX_MODE_BREATH);
            ws2812fx.setSpeed(1000);
        } 
        lstate = state;
    }

#ifdef DEBUG 
    Serial.print(state);
#endif
}

/*
extern "C" void show_dro(const pos_t* axes, const pos_t* wcos, bool isMpos, bool* limits, size_t n_axis) {
    char delim = ' ';
    for (size_t i = 0; i < n_axis; i++) {
        Serial.print(delim); delim = ','; String a(axes[i]); Serial.print(a.c_str());
    }
}
extern "C" void show_file(const char* filename, file_percent_t percent) {
    Serial.print(" File: "); Serial.print(filename);Serial.print(" Progress: ");Serial.print(percent);
}
extern "C" void show_spindle_coolant(int spindle, bool flood, bool mist) {
    Serial.print(" Spindle: "); Serial.print(spindle);Serial.print(" Coolant: F:");Serial.print(flood);Serial.print(" M: ");Serial.print(mist);
}
extern "C" void show_feed_spindle(uint32_t feedrate, uint32_t spindle_speed) {
    Serial.print(" Feed: "); Serial.print(feedrate);Serial.print(" Spindle RPM:");Serial.print(spindle_speed);
}
extern "C" void show_overrides(override_percent_t feed_ovr, override_percent_t rapid_ovr, override_percent_t spindle_ovr) {
}
*/

extern "C" void end_status_report() {
#ifdef DEBUG 
    Serial.println("");
#endif
}

extern "C" int milliseconds() {
    return millis();
}

void setup() {
    ws2812fx.init();
    ws2812fx.setBrightness(200);
    ws2812fx.setSpeed(2000);
    ws2812fx.setMode(FX_MODE_CHASE_RAINBOW);
    ws2812fx.start();

    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, GRBL_FROM, GRBL_TO);

    Serial.println("FluidNC ws2812 status");

    fnc_wait_ready();
    fnc_putchar('?');           // Initial status report
    fnc_send_line("$G", 1000);  // Initial modes report
}

void loop() {
    fnc_poll();

    ws2812fx.service();


}
