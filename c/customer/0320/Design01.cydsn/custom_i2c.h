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

#ifndef CUSTOM_I2C_H
#define CUSTOM_I2C_H
    
#include <project.h>
    
/* Allocate buffer */
#define BUFFER_SIZE (128UL)    
    
#define I2C_WRITECOMPELET       1
#define I2C_READCOMPELET        1
#define I2C_WRITEBUSY           0
#define I2C_READBUSY            0    
    
    
    
void CI2C_Init(void);
void CI2C_masterread(uint8_t device_addr,uint8_t registeraddr,uint8_t *buf,uint8_t len);
void CI2C_masterwrite(uint8_t device_addr,uint8_t registeraddr,uint8_t*buf,uint8_t len);
void I2C_Isr(void);
void I2C_callback(uint32_t event);

#if 1
cy_en_syspm_status_t Customer_SCB_I2C_DeepSleepCallback(cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode);    
#else
cy_en_syspm_status_t Customer_SCB_I2C_DeepSleepCallback(cy_stc_syspm_callback_params_t *callbackParams); 
#endif
   
    
    
#endif

/* [] END OF FILE */
