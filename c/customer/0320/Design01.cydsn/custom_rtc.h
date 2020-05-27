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
#ifndef CUSTOM_RTC_H
#define CUSTOM_RTC_H
#include <project.h>
    

void CRTC_Init(void);
cy_stc_rtc_config_t Getrtctime(void);
void Getrtctimearray(uint8_t *array);
void Setrtctime(uint8_t year,uint8_t month,uint8_t date,uint8_t hour,uint8_t min,uint8_t sec );
#endif
/* [] END OF FILE */
