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

### Display firmware

Required only for the first time you need to flash the firmware manually. 

* Connect display via usb-c
* Enter the cloned copy of repository
* Install [PlatformIO](https://docs.platformio.org/en/latest/core/installation/methods/installer-script.html#super-quick-macos-linux)
* Build & flash screen module firmware 
    ```
    cd ~/...
    pio run -e release -t upload
    ```

## Installation
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
```ini
[imonk]
cs_pin: gpio1
spi_bus: spi0a
```

##### Images
```ini
[imonk image face2.png]
path: ~/face.png
```

##### Scenes
```ini
[imonk scene test]
background: CCCCCC
widgets: [
    {
      "id": "progress",
      "type": "gauge",
      "x": 120,
      "y": 120,
      "r-end": 80,
      "r-start": 60,
      "color": "FF0000",
      "background": "000000",
      "value": 10
    }
  ]
```

### Commands