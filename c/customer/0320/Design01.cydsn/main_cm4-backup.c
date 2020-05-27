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
#include "project.h"
#include "ble_application.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "custom_uart.h"
#include "custom_i2c.h"
#include "custom_adc.h"
#include "custom_mcwdt.h"
#include "custom_rtc.h"
#include "custom_crypto.h"
#include "custom_flash.h"
#include "MAX30102.h"
#include "lowpower_clock.h"

uint32_t time_count=0;
uint8_t flag = 0;

uint16 result[2]={0};
char msg[100];



uint8_t *uartdata=NULL;
uint8_t len;


uint32 testdataadd=0;

cy_stc_rtc_config_t rtctime;


DS_data_t displacementdata;
uint8_t SPO2data;
uint8_t HRdata;



void getspo2data(void);

#if 1
uint8_t testbuf[192];
void testi2c(void)
{
    CI2C_masterread(0x08,0x00,testbuf,20);
    sprintf(msg,"test i2c:  \r\n");
    Cy_SCB_UART_PutString(UART_HW,msg);
    for(uint8_t i=0;i<20;i++)
    {
        sprintf(msg," %x",testbuf[i]);
        Cy_SCB_UART_PutString(UART_HW,msg);
    }
    memset(msg,0,sizeof(msg)); 
}
#endif

int main(void)
{
    __enable_irq(); /* Enable global interrupts. */

    
    //init adc
    CADC_Init();
    
    
    Ble_Init();
    
    //init mcwdt
    CMCWDT_Init();
   
    //init uart
//    CUART_Init();
    
    
    
    //init i2c
    CI2C_Init();
    
    //init rtc
    CRTC_Init();
    
    //init crypto for CRC
    CCRYPTO_init();
    
    //init max30102
    MAX30102_init();
    MAX30102readHR(testbuf);
    
    uint8_t work_state=0;
    
    if(1)
    {
        work_state = 0;
    }
    else
    {
        work_state = 1;
    }
    

    #if 1
//    uint8_t flashWriteStatus = 0xff;
//    uint8_t a;
    #endif
//    testi2c();
    
	for(;;)
	{
        /* Place your application code here. */
        /* Continuously process BLE events and handle custom BLE services */
        ProcessBleEvents();

        #if 1 
        if(GetNotificationStatus() && flag )
        {
            //Cy_BLE_GATT_GetMtuSize  ( cy_stc_ble_gatt_xchg_mtu_param_t *  param ) ;
            if(Cy_BLE_GATT_GetBusyStatus(GetconnectionAttid())==CY_BLE_STACK_STATE_FREE )
            {
                testreaddata(); 
                time_count++;
                if(time_count == 3600)
                {
                    time_count = 0;
                    flag = 0;
                }
            }
        }
        #endif
        
        if(Cy_BLE_GetNumOfActiveConn()==0)
        {
            Cy_GPIO_Write(Work_led_PORT,Work_led_NUM,0);
            Cy_GPIO_Write(MAX30102_enable_PORT,MAX30102_enable_NUM,1);
//            MAX30102_init();
//            MAX30102readHR(testbuf);
            MAX30102process();
        }
        
        #if 1
        if(GetwdtInterruptOccured())
        {
            ClearwdtInterruptOccured();
//            if(Cy_BLE_GetNumOfActiveConn()==0)
//            {
//                Cy_GPIO_Write(Work_led_PORT,Work_led_NUM,0);
//                Cy_GPIO_Write(MAX30102_enable_PORT,MAX30102_enable_NUM,1);
//                MAX30102_init();
//                MAX30102readHR(testbuf);
//                MAX30102process();
//            }
            // adc measure
            ADC_StartConvert();
            if(ADC_IsEndConversion(CY_SAR_WAIT_FOR_RESULT))
            {
                result[0] = ADC_GetResult16(0);
                result[1] = ADC_GetResult16(1); 
      
                sprintf(msg,"adc result: channel0 %d channel1 %d \r\n",result[0],result[1]);
                Cy_SCB_UART_PutString(UART_HW,msg);
                memset(msg,0,sizeof(msg));     
            }        
            
            //SPO2
            getspo2data();
            
            //heart rate
            //getheartrate();
            
            //RTC time
            rtctime = Getrtctime();
            
            //store data
//            for(uint8_t j=0;j<30;j++)
            {
                displacementdata.integer = result[0];
                displacementdata.decimal = result[0] >> 8;
                storedata(rtctime,displacementdata,SPO2data,HRdata);       
            }
//            Cy_GPIO_Write(Work_led_PORT,Work_led_NUM,1);
        }
        #endif
        #if 0
        // adc measure
        ADC_StartConvert();
        while(!ADC_IsEndConversion(CY_SAR_WAIT_FOR_RESULT));
        {
            result[0] = ADC_GetResult16(0);
        }
        #endif
        #if 0
        if(GetUartinterrupte()&& GetUartErrorstate() == UART_RX_SUCCESS)
        {
            ClearUartinterrupte();
            
            len = GetUartdata(&uartdata);
            if(len)
            {
                Cy_SCB_UART_PutArray(UART_HW,uartdata,len);
                Cy_SCB_UART_PutString(UART_HW,"\r\n");\
            }
            if(0 == memcmp(uartdata,"testflash",9))
            {
                sprintf(msg,"flash address: 0x%lx",(uint32)flashData[0]);
                Cy_SCB_UART_PutString(UART_HW,msg);
                Cy_SCB_UART_PutString(UART_HW,"\r\n");
                memset(msg,0,sizeof(msg));
            }
            if(0 == memcmp(uartdata,"storedata",9))
            {
                flashWriteStatus = Cy_Flash_WriteRow((uint32_t)flashData[0], (const uint32_t *)rawdata);
                if(flashWriteStatus == CY_FLASH_DRV_SUCCESS)
                {
                    
                }
                else
                {
                    //error handle
                    sprintf(msg,"store data failed!!!!!!!!\r\n");
                    Cy_SCB_UART_PutString(UART_HW,msg);
                    Cy_SCB_UART_PutString(UART_HW,"\r\n");
                    memset(msg,0,sizeof(msg));
                    while(1);
                }
                sprintf(msg,"store data finish!\r\n");
                Cy_SCB_UART_PutString(UART_HW,msg);
                Cy_SCB_UART_PutString(UART_HW,"\r\n");
                memset(msg,0,sizeof(msg));
            }
            if(0 == memcmp(uartdata,"showdata",8))
            {
                for(a=0;a<72;a++)
                {
                    uint8_t j=0;
                    j += sprintf(msg+j,"dataline %d:",a);
                    for(uint8_t i=0;i<50;i++)
                    {
                        j+=sprintf(msg+j," 0x%x",flashData[a][i]);
                    }
                    Cy_SCB_UART_PutString(UART_HW,msg);
                    Cy_SCB_UART_PutString(UART_HW,"\r\n");
                    memset(msg,0,sizeof(msg));
                }
            }
            if(0 == memcmp(uartdata,"godeepsleep",11))
            {
                Cy_SCB_UART_PutString(UART_HW,"OK!\r\n");
            }
            if(0 == memcmp(uartdata,"testdata",8))
            {
                testdataadd = 0x10086200;
                sprintf(msg,"testdata : %x",*((uint8_t *)testdataadd));
                Cy_SCB_UART_PutString(UART_HW,msg);
                Cy_SCB_UART_PutString(UART_HW,"\r\n");
                memset(msg,0,sizeof(msg));
//                *(uint8_t*)testdataadd = 0x12;
            }
            ClearUartdata();
        }
        else if(GetUartErrorstate() != UART_RX_SUCCESS)
        {
            UartErrorhandle();
        }
        #endif
        
        if(Cy_BLE_GetNumOfActiveConn()==0 && Cy_SCB_UART_IsTxComplete(UART_HW))
        {
            
//            Cy_GPIO_SetDrivemode(MAX30102_enable_PORT,MAX30102_enable_NUM,CY_GPIO_DM_ANALOG);
//            Cy_GPIO_SetDrivemode(MAX30102_int_PORT,MAX30102_int_NUM,CY_GPIO_DM_ANALOG);
//            Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
//            
//            Cy_GPIO_SetDrivemode(MAX30102_enable_PORT,MAX30102_enable_NUM,CY_GPIO_DM_STRONG);
//            Cy_GPIO_SetDrivemode(MAX30102_int_PORT,MAX30102_int_NUM,CY_GPIO_DM_STRONG);
            
        }
        
	}
}
#if 0
//typedef struct
//{
//    uint8_t pos;
//    uint8_t len;
//}Data_location_t;
//    static Data_location_t rtc_pos = 
//    {
//        .pos = 0,
//        .len = 6
//    };
//    static Data_location_t adc_pos =
//    {
//        .pos = 6,
//        .len = 2
//    };
//    static Data_location_t spo2_pos =
//    {
//        .pos = 8,
//        .len = 1
//    };
//    static Data_location_t hr_pos =
//    {
//        .pos = 9,
//        .len = 1
//    };
#endif
extern uint8_t dis_hr,dis_spo2;
void getspo2data(void)
{
    SPO2data = dis_spo2;
    HRdata = dis_hr;
    
}

/* [] END OF FILE */
