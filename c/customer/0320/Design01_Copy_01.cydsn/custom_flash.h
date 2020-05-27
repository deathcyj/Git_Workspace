/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#ifndef CUSTOM_FLASH_H
#define CUSTOM_FLASH_H
#include <project.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "custom_adc.h"

#include "custom_crypto.h"
#include "ble_application.h"

typedef struct
{
    uint16 datacount;
    uint16 datapage;
}Data_index_t;



void storedata(cy_stc_rtc_config_t rtctime,DS_data_t displacementdata,      \
                uint8_t SPO2data,uint8_t HRdata);
void testreaddata(void);
void SetnowDataindex(void);
uint8_t isflashfull(void);
void cleartheflash(void);
#endif

/* [] END OF FILE */
