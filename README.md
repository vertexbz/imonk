# IMONK

If you kno' me, you had a chance to say halo ;)

* No Wi-Fi required
* Full control
* Klipper integration

## BOM

### Printed parts
* [KNOMI v2 StealthBurner body & mount plate](https://github.com/bigtreetech/KNOMI/tree/master/KNOMI2/STL) - mount plate will be modified

### SPI Version
* https://www.waveshare.com/rp2040-lcd-1.28.htm
* some wires


## Software setup

This repository contains all required bits and pieces required to bring IMONK to life, to make things easier clone it to your host machine. 
```
cd ~
git clone ...
```

### IMONK Display firmware

> For the first time only it is required to flash the firmware manually. 

* Connect display via usb-c
* Enter the cloned copy of repository
* Install [PlatformIO](https://docs.platformio.org/en/latest/core/installation/methods/installer-script.html#super-quick-macos-linux)
* Build & flash screen module firmware 
    ```
    cd ~/...
    pio run -e release -t upload
    ```

### Assembly
* connect wires correctly to display
* pull trough SB Body
* connect wires correctly to controller/header

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

**Main configuration** block tells Klipper ho to connect to the display, will vary depending on your board. 
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
background: 333333
widgets: [
    {
      "type": "gauge",
      "x": 120,
      "y": 120,
      "id": "progress",
      "r-end": 80,
      "r-start": 60,
      "color": "FF0000",
      "value": 10,
      "background": "000000",
      "rotation"
      "arc"
    }
  ]
[imonk view test-text]
background: 333333
widgets: [
    {
        "type": "string",
        "x": 120,
        "y": 120,
        "id": "text",
        "color": "FF0000",
        "value": "Klipper Test Text"
        "align"
        "font"
    }
  ]
[imonk view test-images]
background: 333333
widgets: [
    {
        "type": "image",
        "x": 70,
        "y": 50,
        "name": "smily-face" # same as in [imonk image ...] section 
    },
    {
         "type": "string",
         "x": 120,
         "y": 120,
         "id": "text",
         "color": "FF0000",
         "value": "Klipper Test Text"
         "align"
         "font"
    },
    {
        "type": "image",
        "x": 70,
        "y": 140,
        "name": "smily-face" # same as in [imonk image ...] section
    }
  ]
```
_Can appear multiple times, each section defines one view that can be identified by name provided in section brackets. Views are synchronized to device on Klipper start (or with g-code commands) to save on communication during print and operation._

#### Klipper commands
Checking


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