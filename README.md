# Raspberry pico support library for Ilitek 9341 TFT / XPT 2046 Touch displays

This work is derived from [ra3xdh's work](https://github.com/ra3xdh/RP2040_ILI9341_XPT2046).
Credits also to [nbrugger-tgm](https://github.com/nbrugger-tgm/pico_lib_template/tree/master) for his/her guidance to build pico libraries.

It uses just SPI1 from the pico, to keep SPI0 free for other applications. 

## Todo
- cleanup initialization: consistent way to configure pins
- check SPI speed
- Fix startup issue on IRQ pin

## Description

This repository contains a demo project for Raspberry Pi PICO RP2040 board using 
the ILI9341 320x240 TFT display with XPT2046 touchscreen module. The uGUI library 
is used for GUI design. See the source for connection circuit. 

