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
#ifndef CUSTOM_IPC_H
#define CUSTOM_IPC_H
#include <project.h>    

    #define IPC_CHANNEL_INDEX          (15u)
    #define IPC_INTR_INDEX             (15u)
    
    #define IPC_NOTIFY_MASK             (1<<IPC_CHANNEL_INDEX)
    #define IPC_INTR_MASK               (1<<IPC_INTR_INDEX)
    
    #define IPC_CM4_CHANNEL_INDEX      (14u)
    #define IPC_CM4_INTR_INDEX         (14u)
    
    #define IPC_CM4_NOTIFY_MASK             (1<<IPC_CM4_CHANNEL_INDEX)
    #define IPC_CM4_INTR_MASK               (1<<IPC_CM4_INTR_INDEX)
    
    
    
    #define IPC_INTR_CM0P_MUX           7
    #define IPC_INTR_CM4_MUX            6
    
    #define DEEPSLEEP_MSG_MASK                0x000000FF
    #define CHANNEL_NUM_MASK                  0x0000FF00

    
    
    
    
    
    
#endif

/* [] END OF FILE */
