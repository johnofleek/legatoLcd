# Display temperature app

Reads temperature from 1 wire sensors (WP UART1)
Displays it on a 20x4 I2C display
Uploads temperatures via mqtt to Sierra Wireless airvantage cloud server

This app uses
## C++ i2c LCD library
===================

C++ i2c LCD display library for Linux devices using the i2c device nodes.

Based on the "New LiquidCrystal" library from https://bitbucket.org/fmalpartida/new-liquidcrystal/

Only the i2c functionality is kept from the arduino original library, there is no support for
other methods of accessing the LCD screens.

License as the original (CC-BY-SA), changes and Linux i2c device adaptions are (C) Kaj-Michael Lang <milang@tal.org>


Notes on testing for I2C devices. In the following scan /dev/i2c-0 is scanned for devices. In this case the LCD i2c device is a 0x3F
```
i2cdetect -r 0
WARNING! This program can confuse your I2C bus, cause data loss and worse!
I will probe file /dev/i2c-0 using read byte commands.
I will probe address range 0x03-0x77.
Continue? [Y/n] Y
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
00:          -- -- -- -- -- -- -- -- -- -- -- -- --
10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
20: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
30: -- -- -- -- -- -- -- -- -- -- UU -- -- -- -- 3f
40: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
50: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
60: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
70: -- -- -- -- -- -- -- --
```

## Also C code from onewire-over-uart
https://github.com/dword1511/onewire-over-uart

## Notes on WPx5xx
```
/dev/HS0 = UART1
/dev/HSL1 = UART2
```
at!mapuart has been use to configure UART2 as a linux console port and HSO for the customer app

### AT commands
```
microcom /dev/ttyAT
```

then 
```
  at!entercnd="A710"
  at!mapuart=17,1
```

## mqtt
The current app uses a cobbled together service based on eclipse paho but hacked by SW and me - this needs to be replaced !!
Note that only user / password authentication is used - the link is not secure.

mqtt.app is the model for the cobbled together service 
lcd.appComponent.mqtt -> mqttService.mqtt

mqtt.api is a model of the data generated by this app. It is used only at sierra wireless https://eu.airvantage.net develop->My Apps
UPload into your avms account
Add it to 
```
Inventory -> systems -> [your device] select
Edit
Applications
Select applications
````

Select the application and add the credential == the password set in 
mqttClient.c (not mqtt_client.c) currently set as 
```
static const char* password = "JT_PI_001";
```

Note that in the current (crude) mqtt service 
- The user is set by the device IMEI in mqttService
- The password is set as above 

## How this was built by me
```
latest was 16.10.3
source ~/legato/packages/legato.sdk.latest/resources/configlegatoenv
make wp85   ## or make wp75
```
