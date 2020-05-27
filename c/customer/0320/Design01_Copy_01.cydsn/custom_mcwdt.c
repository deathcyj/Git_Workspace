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
#include "custom_mcwdt.h"

static uint8_t wdtInterruptOccured=0;
void MCWDT_Interrupt(void)
{
    uint32 mcwdtIsrMask;

    mcwdtIsrMask = Cy_MCWDT_GetInterruptStatus(MCWDT_HW);

//    /* Toggle the Output Counter00_Out if interrupt cause is MCWDT_0 Counter0*/
//    if (0u != (CY_MCWDT_CTR0 & mcwdtIsrMask))
//    {
//        Cy_MCWDT_ClearInterrupt(MCWDT_HW, CY_MCWDT_CTR0);
////        wdtInterruptOccured = 1;
//
//    }
    if(0u != (CY_MCWDT_CTR1 & mcwdtIsrMask))
    {
        Cy_MCWDT_ClearInterrupt(MCWDT_HW, CY_MCWDT_CTR1);
        wdtInterruptOccured = 1;
    }

   
}
//count0与count1级联产生10s定时中断
void CMCWDT_Init(void)
{
    /* Initialize MCWDT_0 interrupt */
    Cy_SysInt_Init(&MCWDT_isr_cfg, MCWDT_Interrupt);
    NVIC_EnableIRQ(MCWDT_isr_cfg.intrSrc);
    
    /* Set interrupt mask for MCWDT_1 */
	Cy_MCWDT_SetInterruptMask(MCWDT_HW, (CY_MCWDT_CTR1));
    /* Start MCWDT_0 */
    Cy_MCWDT_Init(MCWDT_HW, &MCWDT_config);
    Cy_MCWDT_Enable(MCWDT_HW, CY_MCWDT_CTR0|CY_MCWDT_CTR1, \
	                MCWDT_TWO_LF_CLK_CYCLES_DELAY);
    
    Cy_MCWDT_SetMatch(MCWDT_HW,CY_MCWDT_COUNTER0,32000,0);      //count0 设置match值，计时1s
    Cy_MCWDT_SetMatch(MCWDT_HW,CY_MCWDT_COUNTER1,9,0);          //count1 设置match值，count0产生9次中断，计时10s
}

uint8_t GetwdtInterruptOccured(void)
{
    return wdtInterruptOccured;
}

void ClearwdtInterruptOccured(void)
{
    wdtInterruptOccured = 0;
}
/* [] END OF FILE */
