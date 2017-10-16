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
#include "./avmsMqtt/mqttclient.h"
}

// LCD
#include "./lcd/LiquidCrystal_I2C.h"

#define MAIN_TIMER_NAME                 "MAIN_APP_TIMER"
#define MAIN_TEMPERATURE_UPDATE_RATE (2)        // seconds

#define ROW_TIME        0
#define ROW_RADIO       1
#define ROW_TEMPERATURE 2

// IP
static le_data_RequestObjRef_t ConnectionRef;

// LCD
// i2c address
uint8_t i2c=0x3F;   // 16x2 address is 3A  // older 20x4 is 0x27
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
    lcd.print("HELLO");

    lcd.setCursor(0,1);
    lcd.print("FROM");
    
    lcd.setCursor(0,2);
    lcd.print("LINKWAVE");
 
    
    lcd.setCursor(0,3);
    lcd.print("Linkwave ------ WASP");

}




// main app
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

    sprintf(displayData,"T%d %.1f T%d %.1f ",0, temperature_get( 0, &id ), 1, temperature_get( 1, &id ));  

    lcd.setCursor(0, ROW_TEMPERATURE);
    lcd.print(displayData);
}



void main_displayDateTime(void)
{
    char displayData[32];

    
    le_clk_GetLocalDateTimeString ( \
        "%d %b %y %T", \
        displayData, \
        sizeof(displayData), \
        NULL \
    );
        
    lcd.setCursor(0, ROW_TIME);
    lcd.print(displayData);
}

void main_displayRadioInfo(void)
{
    // warning this doesn't handle profiles - just uses the default set with cm
    le_mdc_ProfileRef_t profileRef = le_mdc_GetProfile(LE_MDC_DEFAULT_PROFILE);
    le_mdc_ConState_t state;
    char displayData[32] = "                    ";
    char ipv4[17];
    uint32_t signalQuality;
    le_result_t res;
    int strpad = 0;
    int strPrintPadctr;

    lcd.setCursor(0,ROW_RADIO);  
    
    if(le_mrc_GetSignalQual(&signalQuality)!= LE_OK)
    {
        signalQuality = 0;
    }
    res = le_mdc_GetSessionState(profileRef, &state);
          
    if (state == LE_MDC_CONNECTED)
    {
        le_mdc_GetIPv4Address(profileRef, ipv4 , sizeof(ipv4));

        sprintf(displayData, "%s  Q%d", ipv4,  signalQuality);
        strpad = cols - strlen(displayData);
        for (strPrintPadctr=0; strPrintPadctr<strpad;strPrintPadctr++ )
        {
            strcat(displayData," ");
        }
    }
    else if(res !=LE_OK)
    {
        sprintf(displayData,"RADIO SESSION : %d", res);
    }
    else
    {

        sprintf(displayData,"RADIO - NO IP       ");
    }     
    lcd.print(displayData);
}

static le_timer_Ref_t updateTimerRef;


static void updateTimer_cbh(le_timer_Ref_t timerRef)
{
    main_displayRadioInfo();
    main_displayTemperatures();
    main_displayDateTime();
}

static void main_timer_init(void)
{
    updateTimerRef = le_timer_Create (MAIN_TIMER_NAME);
    
    le_timer_SetHandler ( updateTimerRef, updateTimer_cbh);
    le_timer_SetRepeat( updateTimerRef, 0 );
    le_timer_SetMsInterval( updateTimerRef, MAIN_TEMPERATURE_UPDATE_RATE * 1000);        
    le_timer_Start( updateTimerRef); 
}


// IP data connection
// static le_mdc_ProfileRef_t profileRef;

static bool DataConnected = false;
//--------------------------------------------------------------------------------------------------
/**
 * Callback for the connection state
 */
//--------------------------------------------------------------------------------------------------
static void ConnectionStateHandler
(
    const char* intfName,
    bool isConnected,
    void* contextPtr
)
{
    if (isConnected)
    {
        DataConnected = isConnected;
        LE_INFO("Connected through interface '%s'\n", intfName);
    }
    else
    {
        LE_INFO("Disconnected\n");
        
        DataConnected = false;
    }
}


COMPONENT_INIT
{   
    LE_INFO("started");
    
    lcd_init();
    LE_INFO("LCD");
    
    temperature_init();
    LE_INFO("Temperature");
    
    main_timer_init();
    
    LE_INFO("timer");
    
    //le_data_SetTechnologyRank(1,LE_DATA_CELLULAR); // this breaks the data ip system ???
    
    // manually start the service
    le_data_ConnectService();
    LE_INFO("data service connected\n");
    
    le_data_AddConnectionStateHandler(&ConnectionStateHandler, NULL);

    ConnectionRef = le_data_Request();
    
    // this starts another timer and the timer uplinks the data - not the best way but 
    mqtt_init();
    LE_INFO("start done");
}
