/**
 ** i2c LCD test application
 **
 ** Author: Kaj-Michael Lang <milang@tal.org>
 ** Copyright 2014 - Under creative commons license 3.0 Attribution-ShareAlike CC BY-SA
 **/
#include "legato.h"
#include "interfaces.h"


// Temperature
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

extern "C" {
// 1 wire temperature probes
#include "./temperatureC/temperature.h"
}

// LCD
#include "./lcd/LiquidCrystal_I2C.h"

void originalMain (void) {
    // i2c address
    uint8_t i2c=0x27;   // 16x2 address is 3A
    // Control line PINs
    uint8_t en=2;
    uint8_t rw=1;
    uint8_t rs=0;

    // Data line PINs
    uint8_t d4=4;
    uint8_t d5=5;
    uint8_t d6=6;
    uint8_t d7=7;

    // Backlight PIN
    uint8_t bl=3;

    // LCD display size
    uint8_t rows=4;
    uint8_t cols=20;

    LiquidCrystal_I2C lcd("/dev/i2c-0", i2c, en, rw, rs, d4, d5, d6, d7, bl, POSITIVE);

    lcd.begin(cols, rows);

    lcd.on();
    lcd.clear();
    lcd.noAutoscroll();

    lcd.setCursor(0,0);
    lcd.print("Row 0 Stuff 1234567890");

    lcd.setCursor(0,1);
    lcd.print("Row 1 signal");
    
    lcd.setCursor(0,2);
    lcd.print("Row 2 Temperature");
    
    
    lcd.setCursor(0,3);
    lcd.print("Row 3 Radio info");
    
    sleep(2);
    
    lcd.scrollDisplayRight();
    
    sleep(2);
    
    lcd.scrollDisplayLeft();
    
    sleep(2);
    
    lcd.setCursor(6,0);
    lcd.print("new message   ");
    sleep(2);
    lcd.setCursor(6,0);
    lcd.print("from mars     ");
    
    

}

COMPONENT_INIT
{ 
    uint8_t deviceIndex;
    uint64_t id;
    float temperature;
    
    LE_INFO("started\n");
    
    originalMain();
    
    temperature_init();
    
    deviceIndex = 0;
    temperature_read();
    temperature_get( deviceIndex, &id, &temperature);
    LE_INFO("temp %f ID %" PRIx64 " IDX %d", temperature, id, deviceIndex);
    
    deviceIndex = 1;
    temperature_read();
    temperature_get( deviceIndex, &id, &temperature);
    LE_INFO("temp %f ID %" PRIx64 " IDX %d", temperature, id, deviceIndex);
    
}
