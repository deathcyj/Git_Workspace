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
#ifndef CUSTOM_ADC_H
#define CUSTOM_ADC_H
#include <project.h>

typedef struct
{
    uint8_t integer;
    uint8_t decimal;
}DS_data_t;    
    
void CADC_Init(void);

#if 1
cy_en_syspm_status_t Custom_SAR_DeepSleepCallback(cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode);    
#else
cy_en_syspm_status_t Custom_SAR_DeepSleepCallback(cy_stc_syspm_callback_params_t *callbackParams);    
#endif


#endif
/* [] END OF FILE */
