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
#include "custom_rtc.h"
//typedef struct cy_stc_rtc_config
//    {
//        /* Time information */
//        uint32_t sec;                    /**< Seconds value, range [0-59] */
//        uint32_t min;                    /**< Minutes value, range [0-59] */
//        uint32_t hour;                   /**< Hour, range depends on hrFormat, if hrFormat = CY_RTC_24_HOURS, range [0-23];
//                                                  If hrFormat = CY_RTC_12_HOURS, range [1-12] and appropriate AM/PM day 
//                                                  period should be set (amPm) */
//        cy_en_rtc_am_pm_t amPm;              /**< AM/PM hour period, see \ref cy_en_rtc_am_pm_t.
//                                                  This element is actual when hrFormat = CY_RTC_12_HOURS. The firmware 
//                                                  ignores this element if hrFormat = CY_RTC_24_HOURS */
//        cy_en_rtc_hours_format_t hrFormat;   /**< Hours format, see \ref cy_en_rtc_hours_format_t */
//        uint32_t dayOfWeek;                  /**< Day of the week, range [1-7], see \ref group_rtc_day_of_the_week */
//        
//        /* Date information  */
//        uint32_t date;                        /**< Date of month, range [1-31] */
//        uint32_t month;                       /**< Month, range [1-12]. See \ref group_rtc_month */
//        uint32_t year;                        /**< Year, range [0-99] */
//    } cy_stc_rtc_config_t;
static cy_stc_rtc_config_t datetime;
void CRTC_Init(void)
{

    /* Set new time and time format in RTC */
    Cy_RTC_Init(&RTC_1_config);
    Cy_RTC_SetHoursFormat(CY_RTC_12_HOURS);
    /* Call these two lines if you enabled “Enable Interrupts” check box */
    Cy_SysInt_Init(&RTC_1_RTC_IRQ_cfg, &RTC_1_Interrupt);
    NVIC_EnableIRQ(RTC_1_RTC_IRQ_cfg.intrSrc);
    /* Setup the alarm – 12:00:00 20/4 */
//    Cy_RTC_SetAlarmDateAndTimeDirect(0u, 0u, 12u, 20u, 4u, CY_RTC_ALARM_1);
}

cy_stc_rtc_config_t Getrtctime(void)
{
    Cy_RTC_GetDateAndTime(&datetime); 
    
//            Cy_RTC_SetDateAndTime(&datetime);
//            sprintf(msg,"time: %d / %d / %d / %2d:%2d:%2d \r\n",(uint16_t)datetime.year,(uint16_t)datetime.month,(uint16_t)datetime.date,\
//                (uint16_t)datetime.hour,(uint16_t)datetime.min,(uint16_t)datetime.sec);
//            Cy_SCB_UART_PutString(UART_HW,msg);
//            memset(msg,0,sizeof(msg));
    return datetime;
}

void Setrtctime(uint8_t year,uint8_t month,uint8_t date,uint8_t hour,uint8_t min,uint8_t sec )
{
    cy_stc_rtc_config_t temptime;
    temptime.year = year;
    temptime.month = month;
    temptime.date = date;
    temptime.hour = hour;
    temptime.min = min;
    temptime.sec = sec;
    temptime.dayOfWeek = 1;
    Cy_RTC_SetDateAndTime(&temptime);
}
void Getrtctimearray(uint8_t *array)
{
    Cy_RTC_GetDateAndTime(&datetime); 
    array[0]=datetime.year;
    array[1]=datetime.month;
    array[2]=datetime.date;
    array[3]=datetime.hour;
    array[4]=datetime.min;
    array[5]=datetime.sec;
    
}
/* [] END OF FILE */
