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

/* [] END OF FILE */
#ifndef CUSTOM_UART_H
#define CUSTOM_UART_H
#include <project.h>

#define UART_RXBUFFLEN                  64    

#define UART_RX_SUCCESS                 0x00
#define ERR_UART_RXDATA_OVERFLOW        0x01
#define ERR_UART_OTHERISR               0x02
    
    
void CUART_Init(void);
uint8_t GetUartinterrupte(void);
void ClearUartinterrupte(void);

uint8_t GetUartdata(uint8_t **data);
void ClearUartdata(void);
uint8_t GetUartErrorstate(void);
void ClearUartErrorstate(void);
void UartErrorhandle(void);
#if 1
cy_en_syspm_status_t Custom_SCB_UART_DeepSleepCallback(cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode);    
#else
cy_en_syspm_status_t Custom_SCB_UART_DeepSleepCallback(cy_stc_syspm_callback_params_t *callbackParams);        
#endif
    

    
#endif