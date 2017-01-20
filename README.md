ESP32 MP3 decoder
=======================

This project is a port of Sprite_tm MP3 web radio project for the ESP8266: https://github.com/espressif/ESP8266_MP3_DECODER

## What Changed

You don't need the SPI RAM anymore, the ESP32 has enough memory by itself.
We can also use the built-in 8Bit DAC instead of the external I2S codec.

## Configuration

Configuration options are kept in /main/include/playerconfig.h.

To configure wifi, set the AP_NAME and AP_PASS environment variables before building.

To configure your radio station, modify PLAY_SERVER, PLAY_PATH and PLAY_PORT .

## Building

Set the IDF_PATH environment variable, and point it to the latest SDK version (not 1.0!).

Execute 'make menuconfig' and configure your serial port, then execute 'make flash'.

## Connecting the I2S codec

## Connecting the I2S codec

I tested several I2S codecs, and was happiest with the MAX98357A, because it does not require MCLCK and also amplifies the audio. There is a breakout from Adafruit: https://www.adafruit.com/product/3006

```
ESP pin   - I2S signal
----------------------
GPIO25/DAC1   - LRCK
GPIO26/DAC2   - BCLK
GPIO22        - DATA
```

