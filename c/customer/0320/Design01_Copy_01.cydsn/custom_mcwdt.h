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
#ifndef CUSTOM_MCWDT_H
#define CUSTOM_MCWDT_H

#include <project.h>

void CMCWDT_Init(void);
void MCWDT_Interrupt(void);

uint8_t GetwdtInterruptOccured(void);
void ClearwdtInterruptOccured(void);
    
#endif

/* [] END OF FILE */
