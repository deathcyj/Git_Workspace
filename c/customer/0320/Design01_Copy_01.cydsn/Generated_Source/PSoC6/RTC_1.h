/*******************************************************************************
* \file RTC_1.h
* \version 2.0
*
* This file provides the constants and parameter values for the RTC Component.
*
********************************************************************************
* Copyright 2016-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(RTC_1_RTC_PDL_H)
#define RTC_1_RTC_PDL_H

#include "cyfitter.h"
#include "rtc/cy_rtc.h"

/* Initial interrupt state definition */
#define RTC_1_INITIAL_IRQ_STATUS        (1u)


/*******************************************************************************
*       Additional includes
*******************************************************************************/
#if (0u != RTC_1_INITIAL_IRQ_STATUS)
    #include "sysint/cy_sysint.h"
    #include "cyfitter_sysint.h"
    #include "cyfitter_sysint_cfg.h"
#endif /* (0u != RTC_1_INITIAL_IRQ_STATUS) */


/*******************************************************************************
*    Parameters definitions
*******************************************************************************/
/* Date Format setting constants */
#define RTC_1_MM_DD_YYYY                (0u)
#define RTC_1_DD_MM_YYYY                (1u)
#define RTC_1_YYYY_MM_DD                (2u)

/* Initial Date format definition */
#define RTC_1_INITIAL_DATA_FORMAT       (0u)

/* Time reset on start constant */
#define RTC_1_TIME_RESET_ON_START       (0u)

/* Initial Time and Date definitions */
#define RTC_1_INITIAL_DATE_SEC          (0u)
#define RTC_1_INITIAL_DATE_MIN          (0u)
#define RTC_1_INITIAL_DATE_HOUR         (11u)
#define RTC_1_INITIAL_DATE_HOUR_FORMAT  (CY_RTC_24_HOURS)
#define RTC_1_INITIAL_DATE_DOW          (4u)
#define RTC_1_INITIAL_DATE_DOM          (27u)
#define RTC_1_INITIAL_DATE_MONTH        (5u)
#define RTC_1_INITIAL_DATE_YEAR         (20u)

/* Initial DST definitions */
#define RTC_1_INITIAL_DST_STATUS        (0u)

#if(0u != RTC_1_INITIAL_DST_STATUS)
    
    #if(0u != RTC_1_TIME_RESET_ON_START)

        /* Definition of delay time which is required in Start function */
        #define RTC_1_DELAY_BEFORE_DST          (500u)
    #endif /* (0u != RTC_1_TIME_RESET_ON_START) */
    
    #define RTC_1_INITIAL_DST_DATE_TYPE     (0u)
    #define RTC_1_INITIAL_DST_START_MONTH   (3u)
    #define RTC_1_INITIAL_DST_START_WOM     (6u)
    #define RTC_1_INITIAL_DST_START_DOM     (22u)
    #define RTC_1_INITIAL_DST_START_DOW     (1u)
    #define RTC_1_INITIAL_DST_START_HRS     (0u)
    #define RTC_1_INITIAL_DST_STOP_MONTH    (10u)
    #define RTC_1_INITIAL_DST_STOP_DOM      (22u)
    #define RTC_1_INITIAL_DST_STOP_DOW      (1u)
    #define RTC_1_INITIAL_DST_STOP_WOM      (6u)
    #define RTC_1_INITIAL_DST_STOP_HRS      (0u)
#endif /* (0u != RTC_1_INITIAL_DST_STATUS) */

/*
* Definition of the date register fields. These macros are for creating a date
* value in a single word with the required date elements sequence.
*/
#if(RTC_1_INITIAL_DATA_FORMAT == RTC_1_MM_DD_YYYY)
    #define RTC_1_10_MONTH_OFFSET   (28u)
    #define RTC_1_MONTH_OFFSET      (24u)
    #define RTC_1_10_DAY_OFFSET     (20u)
    #define RTC_1_DAY_OFFSET        (16u)
    #define RTC_1_1000_YEAR_OFFSET  (12u)
    #define RTC_1_100_YEAR_OFFSET   (8u)
    #define RTC_1_10_YEAR_OFFSET    (4u)
    #define RTC_1_YEAR_OFFSET       (0u)
#elif(RTC_1_INITIAL_DATA_FORMAT == RTC_1_DD_MM_YYYY)
    #define RTC_1_10_MONTH_OFFSET   (20u)
    #define RTC_1_MONTH_OFFSET      (16u)
    #define RTC_1_10_DAY_OFFSET     (28u)
    #define RTC_1_DAY_OFFSET        (24u)
    #define RTC_1_1000_YEAR_OFFSET  (12u)
    #define RTC_1_100_YEAR_OFFSET   (8u)
    #define RTC_1_10_YEAR_OFFSET    (4u)
    #define RTC_1_YEAR_OFFSET       (0u)
#else
    #define RTC_1_10_MONTH_OFFSET   (12u)
    #define RTC_1_MONTH_OFFSET      (8u)
    #define RTC_1_10_DAY_OFFSET     (4u)
    #define RTC_1_DAY_OFFSET        (0u)
    #define RTC_1_1000_YEAR_OFFSET  (28u)
    #define RTC_1_100_YEAR_OFFSET   (24u)
    #define RTC_1_10_YEAR_OFFSET    (20u)
    #define RTC_1_YEAR_OFFSET       (16u)
#endif /* (RTC_1_INITIAL_DATA_FORMAT == RTC_1_MM_DD_YYYY) */


/*******************************************************************************
*    Global Variables
*******************************************************************************/
extern uint8_t RTC_1_initVar;
/**
* \addtogroup group_globals
* \{
*/
extern bool RTC_1_rtcDstStatus;
extern cy_stc_rtc_dst_t const RTC_1_dstConfig;
extern cy_stc_rtc_config_t const RTC_1_config;
/** \} group_globals */


/*******************************************************************************
*    Function Prototypes
*******************************************************************************/
void RTC_1_Start(void);

__STATIC_INLINE cy_en_rtc_status_t RTC_1_Init(cy_stc_rtc_config_t const *config);
__STATIC_INLINE cy_en_rtc_status_t RTC_1_SetDateAndTime(cy_stc_rtc_config_t const *dateTime);
__STATIC_INLINE cy_en_rtc_status_t RTC_1_SetDateAndTimeDirect(uint32_t sec, uint32_t min, uint32_t hour, 
                                                                  uint32_t date, uint32_t month, uint32_t year);
__STATIC_INLINE void RTC_1_GetDateAndTime(cy_stc_rtc_config_t *dateTime);

__STATIC_INLINE cy_en_rtc_status_t RTC_1_SetAlarmDateAndTime(cy_stc_rtc_alarm_t const *alarmDateTime,
                                                                        cy_en_rtc_alarm_t alarmIndex);
__STATIC_INLINE void RTC_1_GetAlarmDateAndTime(cy_stc_rtc_alarm_t *alarmDateTime,
                                                               cy_en_rtc_alarm_t alarmIndex);
__STATIC_INLINE cy_en_rtc_status_t RTC_1_SetAlarmDateAndTimeDirect(uint32_t sec, uint32_t min, 
                                                                              uint32_t hour, uint32_t date, 
                                                                              uint32_t month, cy_en_rtc_alarm_t alarmIndex);

__STATIC_INLINE cy_en_rtc_hours_format_t RTC_1_GetHoursFormat(void);
__STATIC_INLINE cy_en_rtc_status_t RTC_1_SetHoursFormat(cy_en_rtc_hours_format_t hourMode);
__STATIC_INLINE void RTC_1_SelectFrequencyPrescaler(cy_en_rtc_clock_freq_t clkSel);

__STATIC_INLINE uint32_t RTC_1_GetInterruptStatus(void);
__STATIC_INLINE void RTC_1_ClearInterrupt(uint32_t interruptMask);
__STATIC_INLINE void RTC_1_SetInterrupt(uint32_t interruptMask);
__STATIC_INLINE uint32_t RTC_1_GetInterruptMask(void);
__STATIC_INLINE void RTC_1_SetInterruptMask(uint32_t interruptMask);

#if(0u != RTC_1_INITIAL_IRQ_STATUS)

    __STATIC_INLINE void RTC_1_Interrupt(void);
    __STATIC_INLINE void RTC_1_DstInterrupt(cy_stc_rtc_dst_t const *dstTime);

    #if(0u != RTC_1_INITIAL_DST_STATUS)

        __STATIC_INLINE cy_en_rtc_status_t RTC_1_EnableDstTime(cy_stc_rtc_dst_t const *dstTime, 
                                                                          cy_stc_rtc_config_t const *timeDate);
        __STATIC_INLINE cy_en_rtc_status_t RTC_1_SetNextDstTime(cy_stc_rtc_dst_format_t const *nextDst);
        __STATIC_INLINE bool RTC_1_GetDstStatus(cy_stc_rtc_dst_t const *dstTime, 
                                                           cy_stc_rtc_config_t const *timeDate);
    #endif /* (0u != RTC_1_INITIAL_DST_STATUS) */
#endif /* (0u != RTC_1_INITIAL_IRQ_STATUS) */


/*******************************************************************************
* Function Name: RTC_1_Init
****************************************************************************//**
*
* Invokes the Cy_RTC_Init() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_rtc_status_t RTC_1_Init(cy_stc_rtc_config_t const *config)
{
    return(Cy_RTC_Init(config));
}


/*******************************************************************************
* Function Name: RTC_1_GetHoursFormat
****************************************************************************//**
*
* Invokes the Cy_RTC_GetHoursFormat() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_rtc_hours_format_t RTC_1_GetHoursFormat(void)
{
    return(Cy_RTC_GetHoursFormat());
}


/*******************************************************************************
* Function Name: RTC_1_SetHoursFormat
****************************************************************************//**
*
* Invokes the Cy_RTC_SetHoursFormat() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_rtc_status_t RTC_1_SetHoursFormat(cy_en_rtc_hours_format_t hourMode)
{
    return(Cy_RTC_SetHoursFormat(hourMode));
}


/*******************************************************************************
* Function Name: RTC_1_SelectFrequencyPrescaler()
****************************************************************************//**
*
* Invokes the Cy_RTC_SelectFrequencyPrescaler() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void RTC_1_SelectFrequencyPrescaler(cy_en_rtc_clock_freq_t clkSel)
{
    Cy_RTC_SelectFrequencyPrescaler(clkSel);
}


/*******************************************************************************
* Function Name: RTC_1_SetDateAndTime
****************************************************************************//**
*
* Invokes the Cy_RTC_SetDateAndTime() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_rtc_status_t RTC_1_SetDateAndTime(cy_stc_rtc_config_t const *dateTime)
{
    return(Cy_RTC_SetDateAndTime(dateTime));
}


/*******************************************************************************
* Function Name: RTC_1_GetDateAndTime
****************************************************************************//**
*
* Invokes the Cy_RTC_GetDateAndTime() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void RTC_1_GetDateAndTime(cy_stc_rtc_config_t *dateTime)
{
    Cy_RTC_GetDateAndTime(dateTime);
}

/*******************************************************************************
* Function Name: RTC_1_SetAlarmDateAndTime
****************************************************************************//**
*
* Invokes the Cy_RTC_SetAlarmDateAndTime() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_rtc_status_t RTC_1_SetAlarmDateAndTime(cy_stc_rtc_alarm_t const *alarmDateTime,
                                                                        cy_en_rtc_alarm_t alarmIndex)
{
    return(Cy_RTC_SetAlarmDateAndTime(alarmDateTime, alarmIndex));
}


/*******************************************************************************
* Function Name: RTC_1_GetAlarmDateAndTime
****************************************************************************//**
*
* Invokes the Cy_RTC_GetAlarmDateAndTime() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void RTC_1_GetAlarmDateAndTime(cy_stc_rtc_alarm_t *alarmDateTime, 
                                                          cy_en_rtc_alarm_t alarmIndex)
{
    Cy_RTC_GetAlarmDateAndTime(alarmDateTime, alarmIndex);
}


/*******************************************************************************
* Function Name: RTC_1_SetDateAndTimeDirect
****************************************************************************//**
*
* Invokes the Cy_RTC_SetDateAndTimeDirect() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE cy_en_rtc_status_t RTC_1_SetDateAndTimeDirect(uint32_t sec, uint32_t min, uint32_t hour, 
                                                                         uint32_t date, uint32_t month, uint32_t year)
{
    return(Cy_RTC_SetDateAndTimeDirect(sec, min, hour, date, month, year));
}


/*******************************************************************************
* Function Name: RTC_1_SetAlarmDateAndTimeDirect
****************************************************************************//**
*
* Invokes the Cy_RTC_SetAlarmDateAndTimeDirect() PDL driver function.
*
*******************************************************************************/

__STATIC_INLINE cy_en_rtc_status_t RTC_1_SetAlarmDateAndTimeDirect(uint32_t sec, uint32_t min, 
                                                                              uint32_t hour, uint32_t date,
                                                                              uint32_t month, 
                                                                              cy_en_rtc_alarm_t alarmIndex)
{
    return(Cy_RTC_SetAlarmDateAndTimeDirect(sec, min, hour, date, month, alarmIndex));
}

#if(0u != RTC_1_INITIAL_IRQ_STATUS)
    /*******************************************************************************
    * Function Name: RTC_1_Interrupt
    ****************************************************************************//**
    *
    * RTC interrupt handler function. 
    * Invokes the Cy_RTC_Interrupt() PDL driver function.
    *
    *******************************************************************************/
    __STATIC_INLINE void RTC_1_Interrupt(void)
    {
        Cy_RTC_Interrupt(&RTC_1_dstConfig, RTC_1_rtcDstStatus);
    }


    /*******************************************************************************
    * Function Name: RTC_1_DstInterrupt
    ****************************************************************************//**
    * 
    * Invokes the RTC_DstInterrupt() PDL driver function.
    *
    *******************************************************************************/
    __STATIC_INLINE void RTC_1_DstInterrupt(cy_stc_rtc_dst_t const *dstTime)
    {
        Cy_RTC_DstInterrupt(dstTime);
    }


    #if(0u != RTC_1_INITIAL_DST_STATUS)
        /*******************************************************************************
        * Function Name: RTC_1_EnableDstTime
        ****************************************************************************//**
        * 
        * Invokes the Cy_RTC_EnableDstTime() PDL driver function.
        *
        *******************************************************************************/
        __STATIC_INLINE cy_en_rtc_status_t RTC_1_EnableDstTime(cy_stc_rtc_dst_t const *dstTime, 
                                                                          cy_stc_rtc_config_t const *timeDate)
        {
            return(Cy_RTC_EnableDstTime(dstTime, timeDate));
        }


        /*******************************************************************************
        * Function Name: Cy_RTC_SetNextDstTime
        ****************************************************************************//**
        *
        * Invokes the Cy_RTC_SetNextDstTime() PDL driver function.
        *
        *******************************************************************************/
        __STATIC_INLINE cy_en_rtc_status_t RTC_1_SetNextDstTime(cy_stc_rtc_dst_format_t const *nextDst)
        {
            return(Cy_RTC_SetNextDstTime(nextDst));
        }

        /*******************************************************************************
        * Function Name: RTC_1_GetDstStatus
        ****************************************************************************//**
        *
        * Invokes the Cy_RTC_GetDstStatus() PDL driver function.
        *
        *******************************************************************************/
        __STATIC_INLINE bool RTC_1_GetDstStatus(cy_stc_rtc_dst_t const *dstTime, 
                                                           cy_stc_rtc_config_t const *timeDate)
        {
            return(Cy_RTC_GetDstStatus(dstTime, timeDate));
        }
    #endif /* (0u != RTC_1_INITIAL_DST_STATUS) */

#endif /* (0u != RTC_1_INITIAL_IRQ_STATUS) */



/*******************************************************************************
* Function Name: RTC_1_GetInterruptStatus
****************************************************************************//**
*
* Invokes the Cy_RTC_GetInterruptStatus() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t RTC_1_GetInterruptStatus(void)
{
    return(Cy_RTC_GetInterruptStatus());
}


/*******************************************************************************
* Function Name: RTC_1_ClearInterrupt
****************************************************************************//**
*
* Invokes the Cy_RTC_ClearInterrupt() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void RTC_1_ClearInterrupt(uint32_t interruptMask)
{
    Cy_RTC_ClearInterrupt(interruptMask);
}


/*******************************************************************************
* Function Name: RTC_1_SetInterrupt
****************************************************************************//**
*
* Invokes the Cy_RTC_SetInterrupt() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void RTC_1_SetInterrupt(uint32_t interruptMask)
{
    Cy_RTC_SetInterrupt(interruptMask);
}


/*******************************************************************************
* Function Name: RTC_1_GetInterruptMask
****************************************************************************//**
*
* Invokes the Cy_RTC_GetInterruptMask() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE uint32_t RTC_1_GetInterruptMask(void)
{
    return(Cy_RTC_GetInterruptMask());
}


/*******************************************************************************
* Function Name: RTC_1_SetInterruptMask
****************************************************************************//**
*
* Invokes the Cy_RTC_SetInterruptMask() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE void RTC_1_SetInterruptMask(uint32_t interruptMask)
{
    Cy_RTC_SetInterruptMask(interruptMask);
}


/*******************************************************************************
* Function Name: RTC_1_IsExternalResetOccurred
****************************************************************************//**
*
* Invokes the Cy_RTC_IsExternalResetOccurred() PDL driver function.
*
*******************************************************************************/
__STATIC_INLINE bool RTC_1_IsExternalResetOccurred(void)
{
    return(Cy_RTC_IsExternalResetOccurred());
}

#endif /* RTC_1_RTC_PDL_H */


/* [] END OF FILE */
