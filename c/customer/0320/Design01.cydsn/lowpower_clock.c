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
#include "lowpower_clock.h"

cy_stc_syspm_callback_params_t callbackParams = {
	    /*.base       =*/ NULL,
	    /*.context    =*/ NULL
	};


cy_stc_syspm_callback_t ClkEnterUlpCb = {Clock_EnterUltraLowPowerCallback, /* Callback function */
    										 CY_SYSPM_ULP,                     /* Callback type */
                                             CY_SYSPM_SKIP_CHECK_READY |
                                             CY_SYSPM_SKIP_CHECK_FAIL |
                                             CY_SYSPM_SKIP_AFTER_TRANSITION,   /* Skip mode */
											 &callbackParams,             	   /* Callback params */
                                             NULL, NULL};
cy_stc_syspm_callback_t ClkExitUlpCb = {Clock_ExitUltraLowPowerCallback, /* Callback function */
  										CY_SYSPM_LP,           		     /* Callback type */
                                        CY_SYSPM_SKIP_CHECK_READY |
                                        CY_SYSPM_SKIP_CHECK_FAIL |
                                        CY_SYSPM_SKIP_BEFORE_TRANSITION, /* Skip mode */
										&callbackParams,                 /* Callback params */
                                        NULL, NULL};    
											
void custom_registerlowpowerclockcb(void)
{
    Cy_SysPm_RegisterCallback(&ClkEnterUlpCb);
    Cy_SysPm_RegisterCallback(&ClkExitUlpCb);
}

/*******************************************************************************
* Function Name: Clock_EnterUltraLowPowerCallback
****************************************************************************//**
*
* Enter Low Power Mode callback implementation. It reduces the FLL frequency by
* half.
*
*******************************************************************************/
cy_en_syspm_status_t Clock_EnterUltraLowPowerCallback(
		cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode)
{
    cy_en_syspm_status_t retVal = CY_SYSPM_FAIL;

	switch (mode)
	{
		case CY_SYSPM_BEFORE_TRANSITION:
			/* Disable the FLL */
			Cy_SysClk_FllDisable();

			/* Reconfigure the FLL to be half of the original frequency */
			Cy_SysClk_FllConfigure(IMO_CLOCK, FLL_CLOCK_50_MHZ, CY_SYSCLK_FLLPLL_OUTPUT_AUTO);

			/* Re-enable the FLL */
			Cy_SysClk_FllEnable(FLL_CLOCK_TIMEOUT);

			/* Set Peri Clock Divider to 0u, to keep at 50 MHz */
			Cy_SysClk_ClkPeriSetDivider(0u);
            
            Cy_GPIO_SetDrivemode(MAX30102_enable_PORT,MAX30102_enable_NUM,CY_GPIO_DM_ANALOG);
            Cy_GPIO_SetDrivemode(MAX30102_int_PORT,MAX30102_int_NUM,CY_GPIO_DM_ANALOG);
            
			retVal = CY_SYSPM_SUCCESS;
			break;

		default:
			/* Don't do anything in the other modes */
			retVal = CY_SYSPM_SUCCESS;
			break;
	}

    return retVal;
}

/*******************************************************************************
* Function Name: Clock_ExitUltraLowPowerCallback
****************************************************************************//**
*
* Exit Low Power Mode callback implementation. It sets the original FLL
* frequency for the device.
*
*******************************************************************************/
cy_en_syspm_status_t Clock_ExitUltraLowPowerCallback(
		cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode)
{
    cy_en_syspm_status_t retVal = CY_SYSPM_FAIL;

	switch (mode)
	{
		case CY_SYSPM_AFTER_TRANSITION:
			/* Set Peri Clock Divider to 1u, to keep at 50 MHz */
			Cy_SysClk_ClkPeriSetDivider(1u);

			/* Disable the FLL */
			Cy_SysClk_FllDisable();

			/* Reconfigure the FLL to be the original frequency */
			Cy_SysClk_FllConfigure(IMO_CLOCK, FLL_CLOCK_50_MHZ, CY_SYSCLK_FLLPLL_OUTPUT_AUTO);

			/* Re-enable the FLL */
			Cy_SysClk_FllEnable(FLL_CLOCK_TIMEOUT);
            
            Cy_GPIO_SetDrivemode(MAX30102_enable_PORT,MAX30102_enable_NUM,CY_GPIO_DM_STRONG);
            Cy_GPIO_SetDrivemode(MAX30102_int_PORT,MAX30102_int_NUM,CY_GPIO_DM_STRONG);
            
			retVal = CY_SYSPM_SUCCESS;
			break;

		default:
			/* Don't do anything in the other modes */
			retVal = CY_SYSPM_SUCCESS;
			break;
	}

    return retVal;
}									
/* [] END OF FILE */
