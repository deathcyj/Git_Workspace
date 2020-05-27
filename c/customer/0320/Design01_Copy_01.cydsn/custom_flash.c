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

#include "custom_flash.h"

extern char msg[100];
uint8_t storeflag = 1;
Data_index_t Dataindex=
{
    .datacount = 0,
    .datapage = 0
};

CY_ALIGN(CY_FLASH_SIZEOF_ROW) 
const uint8_t flashData[73][CY_FLASH_SIZEOF_ROW] = {0}; /* The array will be placed in Flash */
uint8_t rawdata[CY_FLASH_SIZEOF_ROW]={0};  
uint8_t data[CY_FLASH_SIZEOF_ROW] = {0};


void SetnowDataindex(void)
{
    for(Dataindex.datapage = 0;Dataindex.datapage < 72;Dataindex.datapage++)
    {
        if(flashData[Dataindex.datapage][511] < 49 )
        {
            memcpy(rawdata,flashData[Dataindex.datapage],CY_FLASH_SIZEOF_ROW);
            Dataindex.datacount = flashData[Dataindex.datapage][511];
            return;
        }
        else
        {
            if(flashData[Dataindex.datapage][511] > 49)
            {
                //error
                while(1);
            }
        }
    }
    Dataindex.datapage = 0;
    return;
}

uint8_t isflashfull(void)
{
    if(flashData[71][511] == 49)
    {
        cleartheflash();
        return 1;
    }
    return 0;
}

void cleartheflash(void)
{
    //clear all data in the flash region
    
}

void storedata(cy_stc_rtc_config_t rtctime,DS_data_t displacementdata,uint8_t SPO2data,uint8_t HRdata)
{
    uint8_t *index;

    if(Dataindex.datacount < 50)
    {
        index = &rawdata[(Dataindex.datacount*10)];
        index[0] = rtctime.year;
        index[1] = rtctime.month;
        index[2] = rtctime.date;
        index[3] = rtctime.hour;
        index[4] = rtctime.min;
        index[5] = rtctime.sec;
        index[6] = displacementdata.integer;
        index[7] = displacementdata.decimal;
        index[8] = SPO2data;
        index[9] = HRdata;
        Dataindex.datacount++; 
        rawdata[511] = Dataindex.datacount;
        if(Cy_BLE_GetNumOfActiveConn() == 0)
        {
            if(Cy_Flash_WriteRow((uint32_t)flashData[Dataindex.datapage], (const uint32_t *)rawdata) == CY_FLASH_DRV_SUCCESS)
            {   
//                sprintf(msg,"store data success,store location: page %d 0x%lx\r\n",      \
//                        Dataindex.datapage,(uint32_t)&flashData[Dataindex.datapage][Dataindex.datacount]);
//                Cy_SCB_UART_PutString(UART_HW,msg);
//                memset(msg,0,sizeof(msg));
            }
            else
            {
                //error handle
//                sprintf(msg,"store data failed!!!!!!!!\r\n");
//                Cy_SCB_UART_PutString(UART_HW,msg);
//                Cy_SCB_UART_PutString(UART_HW,"\r\n");
//                memset(msg,0,sizeof(msg));
                while(1);
            }
            #if 0
            //store dataindex
            memcpy(data,(uint8_t*)&Dataindex,sizeof(Dataindex));
            if(Cy_Flash_WriteRow((uint32_t)flashData[73],(const uint32_t*)data) == CY_FLASH_DRV_SUCCESS)
            {
                
            }
            else
            {
                while(1);
            }
            #endif
            
            
        }
        else
        {
            
        }
    }
    else
    {
        Dataindex.datacount = 0;
        if(Dataindex.datapage < 72)
        {
            if(Cy_BLE_GetNumOfActiveConn() == 0)
            {
                Dataindex.datapage++;
//                if(Cy_Flash_WriteRow((uint32_t)flashData[Dataindex.datapage], (const uint32_t *)rawdata) == CY_FLASH_DRV_SUCCESS)
                {   
                    sprintf(msg,"store in a new line,store location: page %d 0x%lx\r\n",Dataindex.datapage,(uint32_t)flashData[Dataindex.datapage]);
                    Cy_SCB_UART_PutString(UART_HW,msg);
                    memset(msg,0,sizeof(msg));
                }
//                else
//                {
//                    //error handle
//                    sprintf(msg,"store data failed!!!!!!!!\r\n");
//                    Cy_SCB_UART_PutString(UART_HW,msg);
//                    Cy_SCB_UART_PutString(UART_HW,"\r\n");
//                    memset(msg,0,sizeof(msg));
//                    while(1);
//                }
                
            }
            else
            {
                //complete the 
                
            }
        
        }
        else
        {
            Dataindex.datapage = 0;
        }
    }
}

void testreaddata(void)
{
    uint8_t temp[18]={0xfe,0x01,0x03,0x00,0x00,0x09};
    static uint16_t datacount = 0;
    uint16_t crc;
    static Data_index_t index;
    
    if(Cy_BLE_GATT_GetBusyStatus(GetconnectionAttid())==CY_BLE_STACK_STATE_FREE )
    {
        datacount++;
        
        temp[3] = (uint8_t)(datacount >> 8);
        temp[4] = (uint8_t)datacount;
        memcpy(&temp[5],&flashData[index.datapage][index.datacount*10],10);
        crc = GetCRC16(temp,16);
        temp[16] = (uint8_t)(crc >> 8);
        temp[17] = (uint8_t)crc;

	    SendNotification(CY_BLE_SIMON_SERVICE_SIMON_TEST_CHARACTERISTIC_CHAR_HANDLE,temp,18);
//        sprintf(msg,"send number:  %d %d %d %x %x\r\n",index.datacount,index.datapage ,datacount,temp[3],temp[4]);
//        Cy_SCB_UART_PutString(UART_HW,msg);
//        memset(msg,0,sizeof(msg));

        if(index.datacount < 49)
        {
            index.datacount++;
        }
        else
        {
            index.datacount = 0;
            if(index.datapage < 71)
            {
                index.datapage++;
            }
            else
            {
                datacount = 0;
                index.datapage = 0;
            }
        }
    }
    
}
/* [] END OF FILE */
