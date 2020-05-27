/******************************************************************************
* File Name: ble_application.c
*
* Version: 1.20
*
* Description: This file contains functions that handle custom BLE services
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
* This file contains functions that handle custom BLE services, which includes
* the CapSense Slider service, CapSense button service and the RGB LED service
*******************************************************************************/

/* Header file includes */
#include "ble_application.h"
#include "custom_flash.h"
//===================================================================
uint8_t opdata[20];
uint8_t returndata[20];
uint8_t testdata[20];
uint8_t BDaddr[6]={0};

typedef enum
{
    CHECK_DEVICEID  = 0x01,
    CHECK_RTC,
    CHECK_SENSORDATA,
    CHECK_BATTERYLEVEL,
    SET_RTC
}Op_code_e;
Op_code_e opcode;
//===================================================================
/* Indexes of a two-byte CCCD array */
#define CCCD_INDEX_0            (uint8_t) (0x00u)
#define CCCD_INDEX_1            (uint8_t) (0x01u)

/* Bit mask for the notification bit in CCCD (Client Characteristic Configuration 
   Descriptor), which is written by the client device */
#define CCCD_NOTIFY_ENABLED     (uint8_t) (0x01u)
#define CCCD_NOTIFY_DISABLED    (uint8_t) (0x00u)

#define CCCD_INDICATE_ENABLED   (uint8_t) (0x02u)
#define CCCD_INDICATE_DISABLED  (uint8_t) (0x00u)


/* 'connectionHandle' stores the BLE connection parameters */
cy_stc_ble_conn_handle_t static connectionHandle;

uint8_t static NotificationStatus       = CCCD_NOTIFY_DISABLED;
uint8_t static IndicateStatus           = CCCD_INDICATE_DISABLED;
uint8_t static Indicateresflag          = 1;

/* This flag is used to restart advertisements in the main control loop */
bool    static restartBleAdvertisement   = false;
extern uint8_t flag;
/*******************************************************************************
* Function Name: void UpdateCccdStatusInGattDb
*                (cy_ble_gatt_db_attr_handle_t cccdHandle, uint8_t value)                            
********************************************************************************
* Summary:
*  This function updates the notification status (lower byte of CCCD array) of
*  a characteristic in GATT DB with the provided parameters
*
* Parameters:
*  cccdHandle:	CCCD handle of the service
*  value     :  Notification status. Valid values are CCCD_NOTIFY_DISABLED and
*               CCCD_NOTIFY_ENABLED
*
* Return:
*  void
*
*******************************************************************************/
void UpdateCccdStatusInGattDb(cy_ble_gatt_db_attr_handle_t cccdHandle,
	uint8_t value)
{
    /* Local variable to store the current CCCD value */
	uint8_t cccdValue[CY_BLE_CCCD_LEN];
	
    /* Load the notification status to the CCCD array */
	cccdValue[CCCD_INDEX_0] = value;
	cccdValue[CCCD_INDEX_1] = CY_BLE_CCCD_DEFAULT;
	
    /* Local variable that stores notification data parameters */
	cy_stc_ble_gatt_handle_value_pair_t  cccdValuePair = 
	{
		.attrHandle = cccdHandle,
		.value.len = CY_BLE_CCCD_LEN,
		.value.val = cccdValue
	};
	
    /* Local variable that stores attribute value */
	cy_stc_ble_gatts_db_attr_val_info_t  cccdAttributeHandle=
	{
		.connHandle = connectionHandle,
		.handleValuePair = cccdValuePair,
		.offset = CY_BLE_CCCD_DEFAULT,
	};
	
    /* Extract flag value from the connection handle */
	if(connectionHandle.bdHandle == 0u)
	{
		cccdAttributeHandle.flags = CY_BLE_GATT_DB_LOCALLY_INITIATED;
	}
	else
	{
		cccdAttributeHandle.flags = CY_BLE_GATT_DB_PEER_INITIATED;
	}
	
    /* Update the CCCD attribute value per the input parameters */
	Cy_BLE_GATTS_WriteAttributeValueCCCD(&cccdAttributeHandle);
}


/*******************************************************************************
* Function Name: void SendNotification(cy_stc_ble_conn_handle_t connectionhandle,
*   `                                  cy_ble_gatt_db_attr_handle_t charachandle,
*                                      uint8_t *Data)
********************************************************************************
* Summary:
*  Send RGB LED data as BLE Notifications. This function updates
*  the notification handle with data and triggers the BLE component to send
*  notification
*
* Parameters:
*  rgbData:	pointer to an array containing RGB color and Intensity values
*
* Return:
*  void
*
*******************************************************************************/
void SendNotification(cy_ble_gatt_db_attr_handle_t charachandle,uint8_t *Data,uint8 len)
{
       /* Local variable that stores data parameters */
    cy_stc_ble_gatt_handle_value_pair_t  dataHandle = 
    {
        .attrHandle = CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CHAR_HANDLE,
        .value.val  = Data,
        .value.len  = len 
    };
    
    /* Local variable that stores attribute value */
    cy_stc_ble_gatts_db_attr_val_info_t  AttributeValue = 
    {
        .handleValuePair = dataHandle,
        .offset = CY_BLE_CCCD_DEFAULT,
        .connHandle = connectionHandle,
        .flags = CY_BLE_GATT_DB_LOCALLY_INITIATED
    };

    /* Send updated handle as an attribute to the central device, 
       so that the central reads the new data */
    Cy_BLE_GATTS_WriteAttributeValue(&AttributeValue);
    if(GetNotificationStatus())
    {
        /* Make sure that stack is not busy, then send the notification. Note that 
           the number of buffers in the BLE stack that holds the application data 
           payload are limited, and there are chances that notification might drop 
           a packet if the BLE stack buffers are not available. This error condition
           is not handled in this example project */
    	if(Cy_BLE_GATT_GetBusyStatus(connectionHandle.attId) 
    		== CY_BLE_STACK_STATE_FREE)
//        while(Cy_BLE_GATT_GetBusyStatus(connectionHandle.attId) != CY_BLE_STACK_STATE_FREE);
    	{
            /* Local variable that stores RGB LED notification data parameters */
    		cy_stc_ble_gatts_handle_value_ntf_t NotificationHandle = 
    		{
                .connHandle = connectionHandle,             //connection handle
                .handleValPair.attrHandle = charachandle,   //character handle                     
                .handleValPair.value.val = Data,            //notification data
                .handleValPair.value.len = len               //character length
            };

            /* Send the updated handle as part of attribute for notifications */
            if(Cy_BLE_GATTS_Notification(&NotificationHandle)!= CY_BLE_SUCCESS) //CY_BLE_EVT_STACK_BUSY_STATUS 
            {
                //error handle
//                while(1);
            }
        }
//        else
//        {
////            while(1);
//        }
    }
}
/*******************************************************************************
* Function Name: void Updateateattributevalue(void)
********************************************************************************
* Summary:
*  Receive the new data and update the read characteristic handle so that the
*  next read from the BLE central device gives the current data.
*
* Parameters:
*  connectionhandle:type 'cy_stc_ble_conn_handle_t',it's the handle of current 
*  connection.
*  charachandle:type 'cy_ble_gatt_db_attr_handle_t',it is the handle of the 
*  characteritic that you want to write.
*  value: uint8_t pointer of the buffer you want to write into the charactristic
*  length: the length of the value.
* Return:
*  void
*
*******************************************************************************/
void Updateattributevalue(cy_stc_ble_conn_handle_t connectionhandle,cy_ble_gatt_db_attr_handle_t charachandle, uint8_t *value,uint16_t length)
{
    /* Local variable that stores data parameters */
	cy_stc_ble_gatt_handle_value_pair_t  Handle = 
	{
		.attrHandle = charachandle,
		.value.val  = value,
		.value.len  = length 
	};
	
    /* Local variable that stores attribute value */
	cy_stc_ble_gatts_db_attr_val_info_t  AttributeValue = 
	{
		.handleValuePair = Handle,
		.offset = CY_BLE_CCCD_DEFAULT,
		.connHandle = connectionhandle,
		.flags = CY_BLE_GATT_DB_LOCALLY_INITIATED
	};

    /* Send updated  charachandle as an attribute to the central device, 
       so that the central reads the new data */
	Cy_BLE_GATTS_WriteAttributeValue(&AttributeValue);
	

}


void Handlecustomdata(cy_stc_ble_gatts_write_cmd_req_param_t 
	*writeRequest)
{
    uint16 CRCresult=1;
    uint8_t datalen=0;
    if (writeRequest->handleValPair.attrHandle == 
		CY_BLE_SIMON_SERVICE_SIMON_INDICATE_CHARAC_CHAR_HANDLE)
	{
        /* Extract the write value sent by the Client for RGB LED Color 
           characteristic */
        datalen = writeRequest->handleValPair.value.len;
		for(uint8_t i=0;i<datalen;i++)
		{
			opdata[i] = writeRequest->handleValPair.value.val[i];
		}
        
        if(opdata[0] == 0xfe)
        {
            switch(opdata[1])
            {
                case 01:
                
                if(opdata[2] == CHECK_DEVICEID)
                {
                    //checkCRC
                    CRCresult = GetCRC16(opdata,datalen);
                    if(CRCresult == (((uint16)opdata[4]<<8)&opdata[3]))
                    {
                        returndata[0] = 0xFE;
                        returndata[1] = 01;
                        returndata[2] = CHECK_DEVICEID;
                        memcpy(&returndata[3],BDaddr,6);
                        //calculate CRC 
                        CRCresult = GetCRC16(returndata,8);
                        returndata[9] = (uint8_t)CRCresult;
                        returndata[10] = (uint8_t)(CRCresult>>8);
                        SendNotification(CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CHAR_HANDLE,     \
                                returndata,20);
                    }
                }
                else if(opdata[2] == CHECK_RTC)
                {
                    //CRC
                    CRCresult = GetCRC16(opdata,datalen);
                    if(CRCresult == (((uint16)opdata[4]<<8)&opdata[3]))
                    {
                        returndata[0] = 0xFE;\
                        returndata[1] = 0x01;
                        returndata[2] = CHECK_RTC;
                        Getrtctimearray(&returndata[3]);
                        //calculate CRC 
                        CRCresult = GetCRC16(returndata,9);
                        returndata[9] = CRCresult;
                        returndata[10] = CRCresult>>8;
                        SendNotification(CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CHAR_HANDLE,     \
                                returndata,20);
                    }
                }
                else if(opdata[2] == CHECK_BATTERYLEVEL)
                {
                    //CRC
                    CRCresult = GetCRC16(opdata,datalen);
                    if(CRCresult == (((uint16)opdata[4]<<8)&opdata[3]))
                    {
                        returndata[0] = 0xFE;
                        returndata[1] = 0x01;
                        returndata[2] = CHECK_BATTERYLEVEL;
                        returndata[3] = 0x64;
                        //calculate CRC 
                        CRCresult = GetCRC16(returndata,4);
                        returndata[4] = CRCresult;
                        returndata[5] = CRCresult>>8;
                        SendNotification(CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CHAR_HANDLE,     \
                                returndata,20);
                    }
                }
                else if(opdata[2] == CHECK_SENSORDATA)
                {
                    
                    //CRC
                    CRCresult = GetCRC16(opdata,datalen);
                    if(CRCresult == (((uint16)opdata[6]<<8)&opdata[5]))
                    {
                        flag = 1;
                    }
//                    {
////                        flag = 1；
//                        returndata[0] = 0xFE;
//                        returndata[1] = 0x01;
//                        returndata[2] = CHECK_SENSORDATA;
//                        returndata[3] = 0x64;
//                        //calculate CRC 
//                        returndata[4] = CRCresult;
//                        returndata[5] = CRCresult>>8;
//                        SendNotification(CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CHAR_HANDLE,     \
//                                returndata,20);
//                        
//                    }
                }
                
                break;
                
                case 05:
                if(opdata[2] == CHECK_RTC)
                {
                    CRCresult = GetCRC16(opdata,datalen);
                    if(CRCresult == (((uint16)opdata[10]<<8)&opdata[9]))
                    {
                    Setrtctime(opdata[3],opdata[4],opdata[5],opdata[6],opdata[7],opdata[8]);
                    returndata[0]=0xfe;
                    returndata[1]=0x05;
                    returndata[2]=0x02;
                    CRCresult = GetCRC16(returndata,3);
                    returndata[3] = CRCresult;
                    returndata[4] = CRCresult>>8;
                    SendNotification(CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CHAR_HANDLE,     \
                                returndata,20);
                    }
                }
                break;
                
                default:
                break;
            }
        }

        Updateattributevalue(connectionHandle,CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CHAR_HANDLE,opdata,sizeof(testdata));

	}
    
}

/*******************************************************************************
* Function Name: void HandleDisconnectEvent(void)
********************************************************************************
* Summary:
*  This functions handles the 'disconnect' event for the service
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void HandleDisconnectEvent(void)
{
    //clear the connectionhandle
    connectionHandle.attId =0;
    connectionHandle.bdHandle = 0;
    
    restartBleAdvertisement = true;
    /* Reset RGB notification flag to prevent further notifications
       being sent to Central device after next connection. */
	NotificationStatus = CCCD_NOTIFY_DISABLED;
	Cy_GPIO_Write(Connect_led_PORT,Connect_led_NUM,1);
    /* Update the corresponding CCCD value in GATT DB */
	UpdateCccdStatusInGattDb(CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE, NotificationStatus);

	
}


/*******************************************************************************
* Function Name: void HandleWriteRequest
*                     (cy_stc_ble_gatts_write_cmd_req_param_t *writeRequest)
********************************************************************************
* Summary:
*  This functions handles the 'write request' event for the RGB LED service
*
* Parameters:
*  writeRequest : pointer to the write request parameters from the central        
*
* Return:
*  void
*
*******************************************************************************/
void HandleWriteRequest(cy_stc_ble_gatts_write_cmd_req_param_t 
	*writeRequest)
{
    /* Check the validity and then extract the write value sent by the Client 
       for CCCD */
	if (writeRequest->handleValPair.attrHandle == CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE && 
		((writeRequest->handleValPair.value.val[CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] 
			== CCCD_NOTIFY_DISABLED)||
		(writeRequest->handleValPair.value.val[CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX]
			== CCCD_NOTIFY_ENABLED)))
	{
		NotificationStatus = writeRequest->
		handleValPair.value.val[0] ;
		
         /* Update the corresponding CCCD value in GATT DB */
		UpdateCccdStatusInGattDb(CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE, NotificationStatus);
	}
	
    
//    if( (writeRequest->handleValPair.attrHandle == CY_BLE_SIMON_SERVICE_SIMON_INDICATE_CHARAC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE) &&      \
//        ((writeRequest->handleValPair.value.val[CY_BLE_SIMON_SERVICE_SIMON_INDICATE_CHARAC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] == CCCD_INDICATE_ENABLED) ||        \
//        (writeRequest->handleValPair.value.val[CY_BLE_SIMON_SERVICE_SIMON_INDICATE_CHARAC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] == CCCD_INDICATE_DISABLED))
//    )
//    {
//        IndicateStatus = writeRequest->handleValPair.value.val[0];
//        UpdateCccdStatusInGattDb(CY_BLE_SIMON_SERVICE_SIMON_INDICATE_CHARAC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE, IndicateStatus);
//    }
    
    /* Check if the notification is enabled for RGB LED service */
	if (NotificationStatus == CCCD_NOTIFY_ENABLED)
	{
        /* Update the Notification attribute */
		SendNotification(CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CHAR_HANDLE,testdata,20);
	}
	
    /* Check if the returned handle is matching to Write
       Attribute and extract the data*/
	if (writeRequest->handleValPair.attrHandle == 
		CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CHAR_HANDLE)
	{
        /* Extract the write value sent by the Client for characteristic */
		for(uint8_t i=0;i<writeRequest->handleValPair.value.len;i++)
		{
			testdata[i] = writeRequest->handleValPair.value.val[i];
		}
        
        cy_stc_ble_gatt_handle_value_pair_t data;
        
        data.attrHandle = CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CHAR_HANDLE;
        data.value.val = testdata;
        data.value.len = writeRequest->handleValPair.value.len;
        
        
        Cy_BLE_GATTS_WriteAttributeValueLocal(&data);

	}
}

/*******************************************************************************
* Function Name: void StackEventHandler(uint32_t event, void *eventParameter)
********************************************************************************
* Summary:
*  Call back event function to handle various events from the BLE stack
*
* Parameters:
*  event            :	event returned
*  eventParameter   :	link to value of the events returned
*
* Return:
*  void
*
*******************************************************************************/
void StackEventHandler(uint32_t event, void *eventParameter)
{
    /* Local variable to store the data received as part of the write request
       events */
	cy_stc_ble_gatts_write_cmd_req_param_t   *writeReqParameter;
    
	
    /* Take an action based on the current event */
	switch (event)
	{
        /* This event is received when the BLE stack is Started */
		case CY_BLE_EVT_STACK_ON:
		
            /* Set restartBleAdvertisement flag to allow calling advertisement
               API */
        //Cy_BLE_GAP_SetBdAddress();
        Cy_BLE_GAP_GetBdAddress();
        memcpy(BDaddr,cy_ble_config.deviceAddress->bdAddr,6);
		
		restartBleAdvertisement = true;
		break;
		
        /*~~~~~~~~~~~~~~~~~~~~~~ GATT EVENTS ~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		
        /* This event is received when device is connected over GATT level */    
		case CY_BLE_EVT_GATT_CONNECT_IND:
		
            /* Update attribute handle on GATT Connection */
		connectionHandle = *(cy_stc_ble_conn_handle_t *) eventParameter;
        Cy_GPIO_Write(Connect_led_PORT,Connect_led_NUM,0);
		break;
		
        /* This event is received when device is disconnected */
		case CY_BLE_EVT_GATT_DISCONNECT_IND:

		HandleDisconnectEvent();
		break;
		
        /* This event is received when Central device sends a Write command
           on an Attribute */
        case CY_BLE_EVT_GATTS_WRITE_REQ:                        //write with response
        Cy_BLE_GATTS_WriteRsp(connectionHandle);
        writeReqParameter = (cy_stc_ble_gatts_write_cmd_req_param_t *) 
		eventParameter;
//        if(CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CHAR_HANDLE
//			== writeReqParameter->handleValPair.attrHandle )
        {
            Handlecustomdata(writeReqParameter);
        }
        /* Send the response to the write request received. */
        #if 1
        {
//        cy_stc_ble_gatt_err_info_t errcode;
//        Cy_BLE_GATTS_SendErrorRsp(&connectionHandle,&errcode);
        }
        
        
        
        if (CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE
			== writeReqParameter->handleValPair.attrHandle ||
			CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CHAR_HANDLE
			== writeReqParameter->handleValPair.attrHandle )//||
            //writeReqParameter->handleValPair.attrHandle == CY_BLE_SIMON_SERVICE_SIMON_INDICATE_CHARAC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE
        
		{
			HandleWriteRequest(writeReqParameter);

		}
        
        
         #endif
        
        break;
		case CY_BLE_EVT_GATTS_WRITE_CMD_REQ:                    //write without response
        {
        /* Read the write request parameter */
		writeReqParameter = (cy_stc_ble_gatts_write_cmd_req_param_t *) 
		eventParameter;

		if (CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE
			== writeReqParameter->handleValPair.attrHandle ||
			CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CHAR_HANDLE
			== writeReqParameter->handleValPair.attrHandle )//||
            //writeReqParameter->handleValPair.attrHandle == CY_BLE_SIMON_SERVICE_SIMON_INDICATE_CHARAC_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE
        
		{
			HandleWriteRequest(writeReqParameter);

		}
		}
            
		break;
           
        /*~~~~~~~~~~~~~~~~~~~~~~ OTHER EVENTS ~~~~~~~~~~~~~~~~~~~~~~~~~~*/ 
        /*  client response the indicate*/
		case CY_BLE_EVT_GATTS_HANDLE_VALUE_CNF:                     
//                cy_stc_ble_conn_handle_t *temp = (cy_stc_ble_conn_handle_t*)eventParameter;
//            Cy_GPIO_Inv(Pin_3_PORT, Pin_3_NUM);
//            Indicateresflag = 1;
            
        break;
            
        case CY_BLE_EVT_GET_DEVICE_ADDR_COMPLETE:
        {
            
            memcpy(BDaddr,(*(cy_stc_ble_bd_addrs_t *)(*(cy_stc_ble_events_param_generic_t*)eventParameter).eventParams).publicBdAddr,6);
        }
            
            
        break;
           
        case CY_BLE_EVT_STACK_BUSY_STATUS:
            
        break;
        /* Do nothing for all other events */
		default:
		break;
	}
}

/*******************************************************************************
* Function Name: void ProcessBleEvents(void)
********************************************************************************
* Summary:
*  Function that continuously process the BLE events and handles custom BLE 
*  services
*
* Parameters:
*  None
*
* Return:
*  Void
*
*******************************************************************************/
void ProcessBleEvents(void)
{  
    /* Process event callback to handle BLE events. The events generated 
	   and used for this application are inside the 'StackEventHandler' 
       routine */
	Cy_BLE_ProcessEvents();

    /* BLE is connected */
	if(Cy_BLE_GetConnectionState(connectionHandle) == 
		CY_BLE_CONN_STATE_CONNECTED)
	{
        //BLE is connected
		
	}
    /* BLE is advertising */
	else if (Cy_BLE_GetAdvertisementState() == CY_BLE_ADV_STATE_ADVERTISING)
	{
        //ble is advertising
	}
	else
	{
        //disconnected
		
         /* Check if the advertisement needs to be restarted */
		if(restartBleAdvertisement)
		{
    		/* Reset 'restartBleAdvertisement' flag */
			restartBleAdvertisement = false;
			
    		/* Start Advertisement and enter discoverable mode */
			Cy_BLE_GAPP_StartAdvertisement
			(CY_BLE_ADVERTISING_FAST,CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);	
		}
	}
}

/*******************************************************************************
* Function Name: void InitBle(void)
********************************************************************************
*
* Summary:
*  Function that initializes the BLE component with a custom event handler
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  None
*
*******************************************************************************/
void Ble_Init(void)
{
    /* Start the BLE component and register the stack event handler */
	Cy_BLE_Start(StackEventHandler);
	
}

/*******************************************************************************
* Function Name: void RestartBleAdvertisement (void)
********************************************************************************
*
* Summary:
*  Function that restarts BLE advertisement after a timeout or a disconnect 
*  event
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  None
*
*******************************************************************************/
void RestartBleAdvertisement (void)
{
    /* Set the restart advertisement flag */
	restartBleAdvertisement = true;
}

/*******************************************************************************
* Function Name: bool isBLEreadyForLPM(void)
********************************************************************************
*
* Summary:
*  Function to check if the BLE is ready to enter low power mode
*
* Parameters:
*  None
*
* Return:
*  bool     : true if ready to enter low power mode, false otherwise
*
* Side Effects:
*  None
*
*******************************************************************************/
bool  IsBleReadyForLowPowerMode(void)
{
    /* Variable that stores the return flag */
	bool lowPowerModeReady = false;
	
    /* Don't enter low power mode if the stack is not on */
	if(Cy_BLE_GetState() == CY_BLE_STATE_ON)
	{
        /* Don't enter the low power mode if BLE is connected */
		if(Cy_BLE_GetNumOfActiveConn() != 0u)
		{
			lowPowerModeReady=false;
		}
		else
		{
			lowPowerModeReady=true;
		}
	}
	
    /* Return  the low power mode entry readiness */
	return lowPowerModeReady;
}

#if 1

    
void testsendnotification(void)
{
    static uint32 randdata=0;
    randdata++;
 

    {
        memcpy(testdata,&randdata,4);
    }
    #if 1
     
    #endif
    
	if (NotificationStatus == CCCD_NOTIFY_ENABLED)
	{
        /* Update the RGB LED Notification attribute with new color
           coordinates */
		SendNotification(CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CHAR_HANDLE,testdata,20);
//        Cy_BLE_ProcessEvents();
	}
}


void testsendindicate(void)
{
    static uint32 randdata=0;
    randdata++;
 

    {
        memcpy(testdata,&randdata,4);
    }
    #if 1
        /* Local variable that stores  data parameters */
    cy_stc_ble_gatt_handle_value_pair_t  dataHandle = 
    {
        .attrHandle = CY_BLE_SIMON_SERVICE_SIMON_INDICATE_CHARAC_CHAR_HANDLE,
        .value.val  = testdata,
        .value.len  = 64 
    };
    
    /* Local variable that stores RGB attribute value */
    cy_stc_ble_gatts_db_attr_val_info_t  indicateAttributeValue = 
    {
        .handleValuePair = dataHandle,
        .offset = CY_BLE_CCCD_DEFAULT,
        .connHandle = connectionHandle,
        .flags = CY_BLE_GATT_DB_LOCALLY_INITIATED
    };

    /* Send updated RGB control handle as an attribute to the central device, 
       so that the central reads the new RGB color data */
    Cy_BLE_GATTS_WriteAttributeValue(&indicateAttributeValue);
    #endif
    
	if (IndicateStatus == CCCD_INDICATE_ENABLED)
	{
        /* Update the RGB LED Notification attribute with new color
           coordinates */
        /* Make sure that stack is not busy, then send the notification. Note that 
       the number of buffers in the BLE stack that holds the application data 
       payload are limited, and there are chances that notification might drop 
       a packet if the BLE stack buffers are not available. This error condition
       is not handled in this example project */
        if (Cy_BLE_GATT_GetBusyStatus(connectionHandle.attId) 
            == CY_BLE_STACK_STATE_FREE)
        {
            /* Local variable that stores RGB LED notification data parameters */
            cy_stc_ble_gatts_handle_value_ind_t IndicateHandle = 
            {
                .connHandle = connectionHandle,             //connection handle
                .handleValPair.attrHandle = CY_BLE_SIMON_SERVICE_SIMON_INDICATE_CHARAC_CHAR_HANDLE,   //character handle                     
                .handleValPair.value.val = testdata,            //notification data
                .handleValPair.value.len = 20              //character length
            };

            /* Send the updated handle as part of attribute for notifications */
            Indicateresflag = 0;
            Cy_BLE_GATTS_Indication(&IndicateHandle);
            
//                while(1);
            Cy_BLE_ProcessEvents();
        }
	}
}
#endif



bool GetNotificationStatus(void)
{
    return NotificationStatus;
}

bool GetIndicateStatus(void)
{
    return IndicateStatus && Indicateresflag;
}

uint8_t GetconnectionAttid(void)
{
    return connectionHandle.attId;
    
}
/* [] END OF FILE */
