# IMONK

If you kno' me, you had a chance to say halo ;)

* No Wi-Fi required
* Full control via GCode commands
* Customizable views
* Custom images support

## BOM

### Printed parts
* [KNOMI v2 StealthBurner body & mount plate](https://github.com/bigtreetech/KNOMI/tree/master/KNOMI2/STL) - mount plate to be modified, original kinda fits

### SPI Version
* https://www.waveshare.com/rp2040-lcd-1.28.htm
* wires, connectors  (jst, gold-pin), probably soldering iron


## Software setup

This repository contains all bits and pieces required to bring IMONK to life, to make things easier clone it to your host machine. 
```
cd ~
git clone ...
```

### IMONK Display firmware

> For the first time only it is required to flash the firmware manually. 

* Install [PlatformIO](https://docs.platformio.org/en/latest/core/installation/methods/installer-script.html#super-quick-macos-linux)
* Connect display via usb-c
* Enter the cloned copy of repository
* Build & flash screen module firmware 
    ```
    cd ~/...
    pio run -e release -t upload
    ```

### Assembly
* Minimal wiring with RPi Pico's main SPI
![connections.png](doc%2Fconnections.png)


### Extensions

For proper operation IMONK needs Klipper and Monraker extensions

```
cd ~/...
ln -s ./klipper/imonk ~/klipper/klippy/extras/
ln -s ./moonraker/imonk ~/moonraker/moonraker/components/
```

#### Moonraker configuration
```ini
[imonk]
```

#### Klipper configuration

**Main configuration** block tells Klipper how to connect to the display, will vary depending on your board. 

* SB2240/2209 board
```ini
[imonk]
cs_pin: PA10
spi_software_sclk_pin: PB10
spi_software_mosi_pin: PB11
spi_software_miso_pin: PB2
```

* RPi Pico main SPI
```ini
[imonk]
cs_pin: gpio1
spi_bus: spi0a
```
  

**Image configuration**
> Currently only PNG images are supported
```ini
[imonk image smily-face]
path: ~/smily-face.png
```
_Can appear multiple times, each section defines one image that can be identified by name provided in section brackets. Images are synchronized to device on Klipper start (or with g-code commands) to save on communication during print and operation._


**View configuration**
```ini
[imonk view test-gauge]
background: 333333  # View background color
widgets: [  # List of widgets to render on the view
    {
        "type": "gauge",
        "x": 120,  # X Coordinate of the widget
        "y": 120,  # Y Coordinate of the widget
        "id": "progress",  # Handle to set gauge value
        "r-end": 80,  # End radius of the gauge
        "r-start": 60,  # Start radius of the gauge
        "color": "FF0000",  # Gauge fill color
        "background": "000000",  # (optional) Gauge background color (default: "000000") 
        "value": 0,  # (optional) Gauge value, i.e. how much of the gauge should be filled (default: 0) 
        "rotation": 90.0,  # (optional) Gauge widget rotation (in degrees) (default: 90.0) 
        "arc": 260.0  # (optional) How much of the circle should be used as gauge (in degrees) (default: 260.0) 
    }
  ]
[imonk view test-text]
background: 333333
widgets: [
    {
        "type": "string",
        "x": 120,  # X Coordinate of the widget
        "y": 120,  # Y Coordinate of the widget
        "id": "text1",  # Handle to set widgets text
        "color": "FFFFFF",  # Text color
        "value": "Testy text",  # (optional) Text to display on screen (default: "") 
        "font": "roboto24",  # (optional) Font to use to render the text (default: "font0") 
        "align": "left"  # (optional) Text alignment in relation to provided coordinates (default: "center") 
    }
  ]
[imonk view test-images]
background: 333333
widgets: [
    {
        "type": "image",
        "x": 120,  # X Coordinate of the widget
        "y": 120,  # Y Coordinate of the widget
        "name": "smily-face"  # Name of the image to display - same as in [imonk image ...] section
    },
    {
         "type": "string",
         "x": 120,
         "y": 120,
         "id": "text",
         "color": "FF0000",
         "value": "Klipper Test Text"
    },
    {
        "type": "image",
        "x": 70,
        "y": 140,
        "name": "smily-face"
    }
  ]
```
_Can appear multiple times, each section defines one view that can be identified by name provided in section brackets. Views are synchronized to device on Klipper start (or with g-code commands) to save on communication during print and operation._

#### Klipper commands

GCode Macro
```jinja
{% set stage_id = printer.imonk.stage('test-images') %}
```
```jinja
{% set stage_id, is_current = printer.imonk.stage_if_needed('test-images') %}
```
```jinja
{% set stage_id = printer.imonk.set_view('test-images', {'text': 'Value to display'}) %}
```


Testing setup
```ini
[gcode_macro IMONK_TEST_IMAGES]
gcode:
    {% set stage_id, is_current = printer.imonk.stage_if_needed('test-images') %}
    
    {% if not is_current %}
        IMONK_COMMIT_VIEW SID={stage_id}
    {% endif %}
```

#### Moonraker API endpoints

## Motivation

This project came to live mainly to prove that additional display can be fully configured and controlled directly via klipper without wi-fi connection, but I'm happy to further maintain and develop the project if it gets interest
