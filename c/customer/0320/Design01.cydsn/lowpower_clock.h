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
#ifndef LOWPOWER_CLOCK_H
#define LOWPOWER_CLOCK_H
#include <project.h>
cy_en_syspm_status_t Clock_EnterUltraLowPowerCallback(cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode);
cy_en_syspm_status_t Clock_ExitUltraLowPowerCallback(cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode);
void custom_registerlowpowerclockcb(void);

/* Time out for changing the FLL (in cycles) */
#define FLL_CLOCK_TIMEOUT   200000u

/* Clock frequency constants (in Hz) */
#define FLL_CLOCK_50_MHZ	50000000u
#define FLL_CLOCK_100_MHZ	100000000u
#define IMO_CLOCK			8000000u    
    
#endif
/* [] END OF FILE */
