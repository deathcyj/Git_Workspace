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
#include "custom_ipc.h"

uint32 ipcMessage;
uint32 ipcflag = 0;
/* Change the IPC channel and interrupt structures in common.h
    Can change the interrupt priority if required */
cy_stc_sysint_t intrConfig ={
//                .cm4Src = cpuss_interrupts_ipc_0_IRQn + IPC_INTR_INDEX,
                .intrSrc = cpuss_interrupts_ipc_14_IRQn,
                .intrPriority = 3
};

/* IPC interrupt service routine which can be used as a sync/notification point from CM4 */
void IPC_IntrISR()
{
    /* GPIO toggle to indicate working */
    Cy_IPC_Drv_ReadMsgWord(Cy_IPC_Drv_GetIpcBaseAddress(IPC_CM4_CHANNEL_INDEX), &ipcMessage);     
    Cy_IPC_Drv_LockRelease(Cy_IPC_Drv_GetIpcBaseAddress(IPC_CM4_CHANNEL_INDEX), 0);
    
    ipcflag = 1;
//    if(ipcMessage == 0)
//    {
//        Cy_GPIO_Clr(GPIO_PRT1, 5);
//    }
//    else
//    {
//        Cy_GPIO_Set(GPIO_PRT1, 5);
//    }
    
    /* Clear the IPC notify interrupt */
    Cy_IPC_Drv_ClearInterrupt(Cy_IPC_Drv_GetIntrBaseAddr(IPC_CM4_INTR_INDEX), 0, IPC_CM4_NOTIFY_MASK);
    NVIC_ClearPendingIRQ(intrConfig.intrSrc);    
}

void IPC_init(void)
{
        /* Init the IPC interrupt in CM0+ */
    Cy_SysInt_Init(&intrConfig, IPC_IntrISR);
    NVIC_EnableIRQ(intrConfig.intrSrc);
    
     /* Enable the notify interrupt corresponding to the IPC channel (IPC_CHANNEL_INDEX) 
        used for notification */
    Cy_IPC_Drv_SetInterruptMask(Cy_IPC_Drv_GetIntrBaseAddr(IPC_CM4_INTR_INDEX), 0, IPC_CM4_NOTIFY_MASK);


}

void IPC_dataprocess(void)
{
    if(ipcflag == 1)
    {
        ipcflag = 0;
        //dataprocess
    }
}

/* [] END OF FILE */
