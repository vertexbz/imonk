#pragma once

#include <LovyanGFX.h>

class LGFX_GC9A01 : public lgfx::LGFX_Device {
private:
    lgfx::Panel_GC9A01 _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;

public:
    bool begin(
        int16_t pin_sclk,
        int16_t pin_mosi,
        int16_t pin_dc,
        int16_t pin_cs,
        int16_t pin_rst,
        int16_t pin_bl
    );
};
