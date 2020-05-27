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
#include "custom_adc.h"



void SAR_Interrupt(void)
{
    /************************************************************************
    *  Custom Code
    *  - add user ISR code between the following #START and #END tags
    *************************************************************************/
    /* `#START MAIN_ADC_ISR`  */

    uint32_t intrStatus = 0u;
    /* An array to store the channel results, two channels for Config0 and 1 channel for Config1 */
    static int16_t chanResults[2];
    
    /* Read interrupt status register. */
    intrStatus = Cy_SAR_GetInterruptStatus(ADC_SAR__HW);
    
    /* Check for the EOS interrupt. */
    if ((intrStatus & CY_SAR_INTR_EOS_MASK) == CY_SAR_INTR_EOS_MASK)
    {
        /* Clear handled interrupt. */
        Cy_SAR_ClearInterrupt(ADC_SAR__HW, intrStatus);    

        /* Get the ADC results for both channels in Config0. */
        chanResults[0] = Cy_SAR_CountsTo_mVolts(ADC_SAR__HW, 0, Cy_SAR_GetResult16(SAR, 0));
        chanResults[1] = Cy_SAR_CountsTo_mVolts(ADC_SAR__HW, 1, Cy_SAR_GetResult16(SAR, 1));
    }
    
    /* `#END`  */
    
    /* Clear handled interrupt */
    Cy_SAR_ClearInterrupt(ADC_SAR__HW, intrStatus);
    /* Read interrupt status register to ensure write completed due to buffered writes */
    (void)Cy_SAR_GetInterruptStatus(ADC_SAR__HW);
}


void CADC_Init(void)
{
//    ADC_Start();
    ADC_StartEx(SAR_Interrupt);
    
    static cy_stc_syspm_callback_params_t SARDeepSleepCallbackParams =
    {
//        /* .mode    */ CY_SYSPM_CHECK_READY,
        /* .base    */ SAR,
        /* .context */ NULL
    };
    
    /* Attach the Cy_SAR_DeepSleepCallback function and set the callback parameters. */
    static cy_stc_syspm_callback_t SARDeepSleepCallbackStruct =
    {
        /* .callback        */ &Custom_SAR_DeepSleepCallback,
        /* .type            */ CY_SYSPM_DEEPSLEEP,
        /* .skipMode        */ 0UL,
        /* .callbackParams  */ &SARDeepSleepCallbackParams,
        /* .prevItm         */ NULL,
        /* .nextItm         */ NULL
    };
    
    /* Register the callback before entering Deep Sleep mode. */
    if(Cy_SysPm_RegisterCallback(&SARDeepSleepCallbackStruct))
    {
        
    }
    else
    {
        Cy_SCB_UART_PutString(UART_HW,"ADC low power callback register failed!!!");
    }

}




#if 1
/*******************************************************************************
* Function Name: Cy_SAR_DeepSleepCallback
****************************************************************************//**
*
* Callback to prepare the SAR before entering Deep Sleep
* and to re-enable the SAR after exiting Deep Sleep.
*
* \param callbackParams
* Pointer to structure of type \ref cy_stc_syspm_callback_params_t
*
* \param mode
* Callback mode, see \ref cy_en_syspm_callback_mode_t
*
* \return
* See \ref cy_en_syspm_status_t
*
* \funcusage
*
* \snippet sar_sut_01.cydsn/main_cm0p.c SNIPPET_SAR_DEEPSLEEP_CALLBACK
*
*******************************************************************************/
cy_en_syspm_status_t Custom_SAR_DeepSleepCallback(cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode)
{
    cy_en_syspm_status_t returnValue = CY_SYSPM_SUCCESS;

    if (CY_SYSPM_BEFORE_TRANSITION == mode)
    { /* Actions that should be done before entering the Deep Sleep mode */
        Cy_SAR_DeepSleep((SAR_V1_Type *)callbackParams->base);
    }
    else if (CY_SYSPM_AFTER_TRANSITION == mode)
    { /* Actions that should be done after exiting the Deep Sleep mode */
        Cy_SAR_Wakeup((SAR_V1_Type *)callbackParams->base);
    }
    else
    { /* Does nothing in other modes */
    }

    return returnValue;
}    
#else
/*******************************************************************************
* Function Name: Cy_SAR_DeepSleepCallback
****************************************************************************//**
*
* Callback to prepare the SAR before entering Deep Sleep
* and to re-enable the SAR after exiting Deep Sleep.
*
* \param callbackParams
* Pointer to structure of type \ref cy_stc_syspm_callback_params_t
*
* \return
* See \ref cy_en_syspm_status_t
*
* \funcusage
*
* \snippet sar_sut_01.cydsn/main_cm0p.c SNIPPET_SAR_DEEPSLEEP_CALLBACK
*
*******************************************************************************/
cy_en_syspm_status_t Custom_SAR_DeepSleepCallback(cy_stc_syspm_callback_params_t *callbackParams)
{
    cy_en_syspm_status_t returnValue = CY_SYSPM_SUCCESS;

    if (CY_SYSPM_BEFORE_TRANSITION == callbackParams->mode)
    { /* Actions that should be done before entering the Deep Sleep mode */
        Cy_SAR_Sleep((SAR_Type *)callbackParams->base);
    }
    else if (CY_SYSPM_AFTER_TRANSITION == callbackParams->mode)
    { /* Actions that should be done after exiting the Deep Sleep mode */
        Cy_SAR_Wakeup((SAR_Type *)callbackParams->base);
    }
    else
    { /* Does nothing in other modes */
    }

    return returnValue;
}
#endif

/* [] END OF FILE */
