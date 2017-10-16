/*
 * this should be moved to another app so that the led app behaviour is available to all apps
*/
#include "legato.h"

#include "interfaces.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include "temperature.h"


uint64_t temperature_ids[TEMPERATURE_MAX_SENSORS];
float temperature_values[TEMPERATURE_MAX_SENSORS];




char *get_type_by_id(uint8_t idInt) {
  switch (idInt) {
    case DS18S20_FAMILY_CODE: return "DS18S20";
    case DS18B20_FAMILY_CODE: return "DS18B20";
    case DS1822_FAMILY_CODE : return "DS1822";
    default: return "WHAT?!";
  }
}

// a ram based KVS would be a better way but I don't have one yet
float temperature_get(uint8_t deviceIndex, uint64_t *idVal)
{
    float temperature ;
    
    if(deviceIndex < TEMPERATURE_MAX_SENSORS )
    {
        temperature = temperature_values[deviceIndex];
        *idVal = temperature_ids[deviceIndex];
    }
    else 
    {
        temperature = TEMPERATURE_VALUE_INVALID;
        *idVal = TEMPERATURE_ID_INVALID;
    }
    
    return (temperature);
}

static void temperature_save(uint8_t deviceIndex, uint64_t idVal, float temperature)
{
    temperature_values[deviceIndex] = temperature;
    temperature_ids[deviceIndex] =  idVal;
}
// the following needs sorting out - JT

#define SERIAL_PORT ("/dev/ttyHS0")        // note this is HW UART1 WP8548
//#define SERIAL_PORT ("/dev/ttyGS0")        // note this is HW UART1 WP7502
   

void temperature_read(void)
{
    int8_t idx = 0;
    uint8_t diff = OW_SEARCH_FIRST;
    int16_t temp_dc;
    uint8_t id[OW_ROMCODE_SIZE];
    uint32_t convertCount = 0;
    
    if( ow_init(SERIAL_PORT))
    {
        LE_INFO("Serial port INIT failed. Check serial port.\n");
        return ;
    }
    
    while (diff != OW_LAST_DEVICE) 
    {
        DS18X20_find_sensor(&diff, id);
        if (diff == OW_ERR_PRESENCE) 
        {
            LE_INFO("All sensors are offline now.\n");
            ow_finit();
            return ;
        }
        if (diff == OW_ERR_DATA) 
        {
            LE_INFO("Bus error.\n");
            ow_finit();
            return ;
        }
        /* LE_INFO( "Device %03u Type 0x%02hx (%s) ID %02hx%02hx%02hx%02hx%02hx%02hx CRC 0x%02hx ", \
            idx, id[0], get_type_by_id(id[0]), id[6], id[5], id[4], id[3], id[2], id[1], id[7]);
        */
        idx++;

        if (DS18X20_start_meas(DS18X20_POWER_EXTERN, NULL) == DS18X20_OK) 
        {
            while (DS18X20_conversion_in_progress() == DS18X20_CONVERTING) 
            {
                delay_ms(100); /* It will take a while */
                if(convertCount++ > 20)
                {
                    return;
                }
            }
            if (DS18X20_read_decicelsius(id, &temp_dc) == DS18X20_OK) 
            {
                /* Copied from my MCU code, so no float point */
                // LE_INFO( "TEMP %3d.%01d C\n", temp_dc / 10, temp_dc > 0 ? temp_dc % 10 : -temp_dc % 10);
                
                // added save measurement <JT> //
                if((idx - 1) < TEMPERATURE_MAX_SENSORS)
                {
                    temperature_save((idx -1), GET_ID64_FROM_INT8(id) , ((float )temp_dc) / 10);
                }
                continue;
            }
        }

        LE_INFO("MEASURE FAILED!\n");

    }
    LE_INFO("Sensors listed.\n");

    ow_finit();
    return ;   
}


// sets data to invalid
void temperature_init(void)
{
    uint32_t idx;
     
    for (idx=0;idx<TEMPERATURE_MAX_SENSORS;idx++)
    {
        temperature_save(idx,TEMPERATURE_ID_INVALID,TEMPERATURE_VALUE_INVALID);
    }
}
