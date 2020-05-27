/******************************************************************************
* File Name: ble_application.h
*
* Version: 1.20
*
* Description: This file contains macros and the declaration of functions provided 
*              by the ble_application.c file 
*
* Related Document: CE220167_BLE_UI.pdf
*
* Hardware Dependency: CY8CKIT-062-BLE PSoC 6 BLE Pioneer Kit
*                      CY8CKIT-028-EPD E-INK Display Shield
*
*******************************************************************************
* Copyright (2018), Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* (“Software”), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries (“Cypress”) and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software (“EULA”).
*
* If no EULA applies, Cypress hereby grants you a personal, nonexclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress’s integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, 
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress 
* reserves the right to make changes to the Software without notice. Cypress 
* does not assume any liability arising out of the application or use of the 
* Software or any product or circuit described in the Software. Cypress does 
* not authorize its products for use in any products where a malfunction or 
* failure of the Cypress product may reasonably be expected to result in 
* significant property damage, injury or death (“High Risk Product”). By 
* including Cypress’s product in a High Risk Product, the manufacturer of such 
* system or application assumes all risk of such use and in doing so agrees to 
* indemnify Cypress against all liability.
*******************************************************************************/
/******************************************************************************
* This file contains macros and the declaration of functions provided by the
* ble_application.c file 
********************************************************************************/

/* Include guard */
#ifndef BLE_APPLICATION_H
#define BLE_APPLICATION_H

/* Header file includes */
#include <project.h>
#include "ble\cy_ble_gap.h"
#include "ble\cy_ble_stack_gatt.h"
#include "custom_rtc.h"
#include "custom_crypto.h"
/* Function that initializes the BLE component  */    
void Ble_Init(void);  

/* Function that continuously process the BLE events and handles custom 
   BLE services */
void ProcessBleEvents(void);

/* Function to check if the BLE is ready to enter low power mode */
bool IsBleReadyForLowPowerMode(void);

/* Function that restarts BLE advertisement after a timeout or a disconnect
   event */
void RestartBleAdvertisement (void);

void SendNotification(cy_ble_gatt_db_attr_handle_t charachandle,uint8_t *Data,uint8 len);
        


bool GetNotificationStatus(void);
bool GetIndicateStatus(void);
uint8_t GetconnectionAttid(void);



void testsendnotification(void);
void testsendindicate(void);

#endif /* BLE_APPLICATION_H */
/* [] END OF FILE */
