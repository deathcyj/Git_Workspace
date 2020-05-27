/*******************************************************************************
* File Name: MCWDT_1.c
* Version 1.10
*
* Description:
*  This file provides the source code to the API for the MCWDT_1
*  component.
*
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "MCWDT_1_PDL.h"


/** MCWDT_1_initVar indicates whether the MCWDT_1  component
*  has been initialized. The variable is initialized to 0 and set to 1 the first
*  time MCWDT_1_Start() is called.
*  This allows the component to restart without reinitialization after the first 
*  call to the MCWDT_1_Start() routine.
*
*  If re-initialization of the component is required, then the 
*  MCWDT_1_Init() function can be called before the 
*  MCWDT_1_Start() or MCWDT_1_Enable() function.
*/
uint8 MCWDT_1_initVar = 0u;

/** The instance-specific configuration structure. This should be used in the 
*  associated MCWDT_1_Init() function.
*/ 
const cy_stc_mcwdt_config_t MCWDT_1_config =
{
    .c0Match     = MCWDT_1_C0_MATCH,
    .c1Match     = MCWDT_1_C1_MATCH,
    .c0Mode      = MCWDT_1_C0_MODE,
    .c1Mode      = MCWDT_1_C1_MODE,
    .c2ToggleBit = MCWDT_1_C2_PERIOD,
    .c2Mode      = MCWDT_1_C2_MODE,
    .c0ClearOnMatch = (bool)MCWDT_1_C0_CLEAR_ON_MATCH,
    .c1ClearOnMatch = (bool)MCWDT_1_C1_CLEAR_ON_MATCH,
    .c0c1Cascade = (bool)MCWDT_1_CASCADE_C0C1,
    .c1c2Cascade = (bool)MCWDT_1_CASCADE_C1C2
};


/*******************************************************************************
* Function Name: MCWDT_1_Start
****************************************************************************//**
*
*  Sets the initVar variable, calls the Init() function, unmasks the 
*  corresponding counter interrupts and then calls the Enable() function 
*  to enable the counters.
*
* \globalvars
*  \ref MCWDT_1_initVar
*
*  \note
*  When this API is called, the counter starts counting after two lf_clk cycles 
*  pass. It is the user's responsibility to check whether the selected counters
*  were enabled immediately after the function call. This can be done by the 
*  MCWDT_1_GetEnabledStatus() API.
*
*******************************************************************************/
void MCWDT_1_Start(void)
{
    if (0u == MCWDT_1_initVar)
    {
        (void)MCWDT_1_Init(&MCWDT_1_config);
        MCWDT_1_initVar = 1u; /* Component was initialized */
    }

	/* Set interrupt masks for the counters */
	MCWDT_1_SetInterruptMask(MCWDT_1_CTRS_INT_MASK);

	/* Enable the counters that are enabled in the customizer */
    MCWDT_1_Enable(MCWDT_1_ENABLED_CTRS_MASK, 0u);
}


/*******************************************************************************
* Function Name: MCWDT_1_Stop
****************************************************************************//**
*
*  Calls the Disable() function to disable all counters.
*
*******************************************************************************/
void MCWDT_1_Stop(void)
{
    MCWDT_1_Disable(CY_MCWDT_CTR_Msk, MCWDT_1_TWO_LF_CLK_CYCLES_DELAY);
    MCWDT_1_DeInit();
}


/* [] END OF FILE */
