/*******************************************************************************
* File Name: cyfitter_sysint_cfg.c
* 
* PSoC Creator  4.3 Early Access
*
* Description:
* 
* This file is automatically generated by PSoC Creator.
*
********************************************************************************
* Copyright (c) 2007-2019 Cypress Semiconductor.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#include "cyfitter_sysint.h"
#include "cyfitter_sysint_cfg.h"

/* ARM CM0+ */
#if (((__CORTEX_M == 0) && (CY_CORE_ID == 0)))

    /* BLE_bless_isr */
    const cy_stc_sysint_t BLE_bless_isr_cfg = {
        .intrSrc = (IRQn_Type)BLE_bless_isr__INTC_CORTEXM0P_MUX,
        .cm0pSrc = (cy_en_intr_t)BLE_bless_isr__INTC_NUMBER,
        .intrPriority = BLE_bless_isr__INTC_CORTEXM0P_PRIORITY
    };

#endif /* ((__CORTEX_M == 0) && (CY_CORE_ID == 0)) */

