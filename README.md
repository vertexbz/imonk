# IMONK

If you kno' me, you had a chance to say halo ;)

## Printed parts
* [KNOMI v2 StealthBurner body & mount plate](https://github.com/bigtreetech/KNOMI/tree/master/KNOMI2/STL) - mount plate will be modified

## BOM

### SPI Version
* https://www.waveshare.com/rp2040-lcd-1.28.htm
* some wires


## Core firmware
Build & flash screen module firmware using 
```
pio run -e release -t upload
```

## Installation
```
cd ~
git clone ...
cd ...

ln -s ./klipper/imonk ~/klipper/klippy/extras/
ln -s ./moonraker/imonk ~/moonraker/moonraker/components/
```

## Setup

### Moonraker
```ini
[imonk]
```

### Klipper
```ini
[imonk]
cs_pin: gpio1
spi_bus: spi0a
```

#### Images
```ini
[imonk image face2.png]
path: ~/face.png
```

#### Scenes
```ini
[imonk scene test]
background: CCCCCC
widgets: [
    {
      "type": "gauge",
      "x": 120,
      "y": 120,
      "id": "progress",
      "r-end": 80,
      "r-start": 60,
      "color": "FF0000",
      "background": "000000",
      "value": 10
    }
  ]
```

## Commands