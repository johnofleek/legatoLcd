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

#define MAIN_TIMER_NAME                 "MAIN_APP_TIMER"
#define MAIN_TEMPERATURE_UPDATE_RATE (5)        // seconds


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
    
void lcd_init (void) {

    lcd.begin(cols, rows);

    lcd.on();
    lcd.clear();
    lcd.noAutoscroll();

    lcd.setCursor(0,0);
    lcd.print("0 Stuff 1234567890");

    lcd.setCursor(0,1);
    lcd.print("1 signal");
    
    lcd.setCursor(0,2);
    lcd.print("2");
 
    
    lcd.setCursor(0,3);
    lcd.print("3 Radio info");

}

void main_displayTemperatures(void)
{  
    uint8_t deviceIndex;
    uint64_t id;
    float temperature;
    
    char displayData[32];
    
    temperature_read();

    deviceIndex = 0;
    temperature = temperature_get( deviceIndex, &id );
    LE_INFO("temp %f ID %" PRIx64 " IDX %d", temperature, id, deviceIndex);
    
    deviceIndex = 1;
    temperature = temperature_get( deviceIndex, &id );
    LE_INFO("temp %f ID %" PRIx64 " IDX %d", temperature, id, deviceIndex);

    lcd.setCursor(2,2);
    sprintf(displayData,"T%d %.1f T%d %.1f ",0, temperature_get( 0, &id ), 1, temperature_get( 1, &id ));  
    lcd.print(displayData);
}

static le_timer_Ref_t updateTimerRef;


static void updateTimer_cbh(le_timer_Ref_t timerRef)
{
    main_displayTemperatures();
}

static void main_timer_init(void)
{
    updateTimerRef = le_timer_Create (MAIN_TIMER_NAME);
    
    le_timer_SetHandler ( updateTimerRef, updateTimer_cbh);
    le_timer_SetRepeat( updateTimerRef, 0 );
    le_timer_SetMsInterval( updateTimerRef, MAIN_TEMPERATURE_UPDATE_RATE * 1000);        
    le_timer_Start( updateTimerRef); 
}

COMPONENT_INIT
{ 

    
    LE_INFO("started\n");
    
    lcd_init();
    
    temperature_init();
    
    main_displayTemperatures();

    main_timer_init();
}
