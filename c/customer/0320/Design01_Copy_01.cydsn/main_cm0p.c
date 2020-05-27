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
#include "project.h"
#include "custom_ipc.h"
int main(void)
{
    __enable_irq(); /* Enable global interrupts. */
    uint32_t data;    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
//    Cy_BLE_Start(NULL);
    /* Enable CM4.  CY_CORTEX_M4_APPL_ADDR must be updated if CM4 memory layout is changed. */
    Cy_SysEnableCM4(CY_CORTEX_M4_APPL_ADDR); 

    Cy_IPC_Drv_SendMsgWord(Cy_IPC_Drv_GetIpcBaseAddress(IPC_CM4_CHANNEL_INDEX), IPC_CM4_INTR_MASK, data);
    for(;;)
    {
        /* Place your application code here. */
//        Cy_BLE_ProcessEvents();
//        Cy_SysLib_Delay(5000);
        Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);

        
    }
}













/* [] END OF FILE */
