# Display temperature app

Reads temperature from 1 wire sensors (WP UART1)
Displays it on a 20x4 I2C display

This app uses
## C++ i2c LCD library
===================

C++ i2c LCD display library for Linux devices using the i2c device nodes.

Based on the "New LiquidCrystal" library from https://bitbucket.org/fmalpartida/new-liquidcrystal/

Only the i2c functionality is kept from the arduino original library, there is no support for
other methods of accessing the LCD screens.

License as the original (CC-BY-SA), changes and Linux i2c device adaptions are (C) Kaj-Michael Lang <milang@tal.org>

## Also C code from onewire-over-uart
https://github.com/dword1511/onewire-over-uart

## Notes on WPx5xx

/dev/HS0 = UART1
/dev/HSL1 = UART2

at!mapuart has been use to configure UART2 as a linux console port and HSO for the customer app