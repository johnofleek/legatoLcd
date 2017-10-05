#ifndef _TEMPERATURE_H_
#define _TEMPERATURE_H_

#include "onewire.h"
#include "devices/ds18x20.h"
#include "devices/common.h"



      
#define GET_ID64_FROM_INT8(tab_int8) \
    (\
     ((int64_t)tab_int8[ 1]      ) + \
     ((int64_t)tab_int8[ 2] << 8 ) + \
     ((int64_t)tab_int8[ 3] << 16) + \
     ((int64_t)tab_int8[ 4] << 24) + \
     ((int64_t)tab_int8[ 5] << 32) + \
     ((int64_t)tab_int8[ 6] << 40)   \
    ) 
    
    
#define TEMPERATURE_MAX_SENSORS     (8)
#define TEMPERATURE_INDEX_START     (0)
#define TEMPERATURE_VALUE_INVALID   (-999)
#define TEMPERATURE_ID_INVALID      (0)
#define TEMPERATURE_READING_NOT_AVAILABLE   (-1)
#define TEMPERATURE_READING_OK      (0)

void temperature_read(void);

// reads values found by temperature_read()
// ID example 0416c19086ff as a uint64
// TEMPERATURE_MAX_SENSORS
int32_t temperature_get(uint8_t deviceIndex, uint64_t *id, float *temperature);


void temperature_init(void);

#endif // _TEMPERATURE_H_