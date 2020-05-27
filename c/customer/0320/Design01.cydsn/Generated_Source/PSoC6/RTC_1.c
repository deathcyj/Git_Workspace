/*******************************************************************************
* \file RTC_1.c
* \version 2.0
*
* This file provides the source code to the API for the RTC Component.
*
********************************************************************************
* Copyright 2016-2017, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "RTC_1.h"

/** Indicates whether or not the RTC_1 has been initialized. 
*  The variable is initialized to 0 and set to 1 the first time 
*  RTC_1_Start() is called. This allows the Component to 
*  restart without reinitialization after the first call to 
*  the RTC_1_Start() routine.
*/
uint8_t RTC_1_initVar = 0u;

/** The instance-specific configuration structure. This should be used in the 
*  associated RTC_1_Init() function.
*/
cy_stc_rtc_config_t const RTC_1_config =
{
    /* Initiate time and date */
    .sec       = RTC_1_INITIAL_DATE_SEC,
    .min       = RTC_1_INITIAL_DATE_MIN,
    .hour      = RTC_1_INITIAL_DATE_HOUR,
    .hrFormat  = RTC_1_INITIAL_DATE_HOUR_FORMAT,
    .dayOfWeek = RTC_1_INITIAL_DATE_DOW,
    .date      = RTC_1_INITIAL_DATE_DOM,
    .month     = RTC_1_INITIAL_DATE_MONTH,
    .year      = RTC_1_INITIAL_DATE_YEAR,
};

    /** RTC_1_rtcDstStatus variable which is for DST feature and is 
    *  called in Cy_RTC_Interrupt() PDL driver function. This variable is 
    *  defined as true if DST is enabled and as false if DST is disabled
    */
    bool RTC_1_rtcDstStatus = false;

#if(0u != RTC_1_INITIAL_DST_STATUS)

    /** The instance-specific daylight saving time structure. This should be 
    * used in the associated DTS functions.
    */
    cy_stc_rtc_dst_t const RTC_1_dstConfig =
    {
        /* DST Start time */
        .startDst.format      = (cy_en_rtc_dst_format_t) RTC_1_INITIAL_DST_DATE_TYPE,
        .startDst.hour        = RTC_1_INITIAL_DST_START_HRS,
        .startDst.dayOfMonth  = RTC_1_INITIAL_DST_START_DOM,
        .startDst.weekOfMonth = RTC_1_INITIAL_DST_START_WOM,
        .startDst.dayOfWeek   = RTC_1_INITIAL_DST_START_DOW,
        .startDst.month       = RTC_1_INITIAL_DST_START_MONTH,

        /* DST Stop time */
        .stopDst.format      = (cy_en_rtc_dst_format_t) RTC_1_INITIAL_DST_DATE_TYPE,
        .stopDst.hour        = RTC_1_INITIAL_DST_STOP_HRS,
        .stopDst.dayOfMonth  = RTC_1_INITIAL_DST_STOP_DOM,
        .stopDst.weekOfMonth = RTC_1_INITIAL_DST_STOP_WOM,
        .stopDst.dayOfWeek   = RTC_1_INITIAL_DST_STOP_DOW,
        .stopDst.month       = RTC_1_INITIAL_DST_STOP_MONTH,
    };
#else

    /* Default DST structure in condition that DST is disabled */
    cy_stc_rtc_dst_t const RTC_1_dstConfig;
#endif /* (0u != RTC_1_INITIAL_DST_STATUS) */


/*******************************************************************************
* Function Name: RTC_1_Start
****************************************************************************//**
*
* Optionally calls the RTC_1_Init() when called the first time and 
* configure the RTC_1 as it is set in the customizer. For subsequent 
* calls the configuration is left unchanged and the component is just 
* configured. Invokes RTC_1_EnableDstTime() if the DST is enabled.
*
* \globalvars
* \ref RTC_1_initVar
*
*******************************************************************************/
void RTC_1_Start(void)
{
    #if(0u != RTC_1_INITIAL_DST_STATUS)
        cy_stc_rtc_config_t curTimeAndDate;
    #endif /* (0u != RTC_1_INITIAL_DST_STATUS) */
    
    if(0u == RTC_1_initVar)
    {
        #if(0u != RTC_1_TIME_RESET_ON_START)

            /* Configure the component if power cycle and the external 
            *  reset occurred
            */
            if(Cy_RTC_IsExternalResetOccurred())
            {
                (void) Cy_RTC_Init(&RTC_1_config);
            }
        #endif /* (0u != RTC_1_TIME_RESET_ON_START) */

        #if(0u != RTC_1_INITIAL_IRQ_STATUS)

            /* Hook the interrupt service routine */
            #if defined(RTC_1_RTC_IRQ__INTC_ASSIGNED)
                (void) Cy_SysInt_Init(&RTC_1_RTC_IRQ_cfg, &RTC_1_Interrupt);
            #endif /* defined(RTC_1_RTC_IRQ__INTC_ASSIGNED) */
        #endif /* (0u != RTC_1_INITIAL_IRQ_STATUS) */

        /* The component is configured */
        RTC_1_initVar = 1u;
    }

    #if(0u != RTC_1_INITIAL_IRQ_STATUS)
        #if defined(RTC_1_RTC_IRQ__INTC_ASSIGNED)
            NVIC_EnableIRQ(RTC_1_RTC_IRQ_cfg.intrSrc);
        #endif /* defined(RTC_1_RTC_IRQ__INTC_ASSIGNED) */

        #if(0u != RTC_1_INITIAL_DST_STATUS)
            #if(0u != RTC_1_TIME_RESET_ON_START)
                
                /* Wait for previous RTC update */
                Cy_SysLib_Delay(RTC_1_DELAY_BEFORE_DST);
            #endif /* (0u != RTC_1_TIME_RESET_ON_START) */
                
            Cy_RTC_GetDateAndTime(&curTimeAndDate);
            
            /* Set the DST configuration */
            if(Cy_RTC_EnableDstTime(&RTC_1_dstConfig, &curTimeAndDate) != CY_RTC_SUCCESS)
            {
                /* DST Setting failed */
                CY_ASSERT(0u != 0u);
            }
        #endif /* (0u != RTC_1_INITIAL_DST_STATUS) */
    #endif /* (0u != RTC_1_INITIAL_IRQ_STATUS) */
}


/* [] END OF FILE */
