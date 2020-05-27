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


#define     BATTERY_CHANNEL         0
#define     SENSOR_CHANNEL          1


uint32_t time_count=0;
uint8_t flag = 0;

uint16 result[2]={0};
char msg[100];


uint16 battery_voltage=0;
uint16 displacement_sensor = 0;
uint8_t work_state=0;
uint8_t battery_state = 0;


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
    
    //init Dataindex
    SetnowDataindex();
    
    //init adc
    CADC_Init();
    
    // adc measure
    ADC_StartConvert();
    while(!ADC_IsEndConversion(CY_SAR_WAIT_FOR_RESULT)){
    };
    battery_voltage = ADC_GetResult16(BATTERY_CHANNEL);
    
    
    if(battery_voltage < 124)           //124-200mV Vref=3.3 signle-end ADC 11bit
    {
        battery_state = 0;
    }
    else
    {
        battery_state = 1;
        Ble_Init();
    
        //init mcwdt
        CMCWDT_Init();
       
        //init uart
//        CUART_Init();
        
        //init i2c
        CI2C_Init();
        
        //init rtc
        CRTC_Init();
        
        //init crypto for CRC
        CCRYPTO_init();
        
        //init max30102
        MAX30102_init();
        MAX30102readHR(testbuf);
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
        if(battery_state)
        {
            ProcessBleEvents();
            
            if(Cy_BLE_GetNumOfActiveConn()!=0)      //判断当前状态是否是连接状态
            {
                //连接状态
                if(1)                               //判断是否是初次连接 
                {
                    //初次连接
                    
                    //启用ADC采样，进行sensor零位校准，并存储
                    ADC_StartConvert();
                    while(!ADC_IsEndConversion(CY_SAR_WAIT_FOR_RESULT)){
                    };
                    displacement_sensor = ADC_GetResult16(SENSOR_CHANNEL);
                    
                    //storedisplacement_offset
                    
                    //device goto the work mode
                    work_state=1;
                    
                }
                else
                {
                    //非初次连接
                    //nothing
                }
                
            }
            
            //
            if(isflashfull())
            {
                //clear the data
                
            }
            
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
            
            if(work_state)
            {
                //位移传感器位置调零
                
                //位移传感器采样
                
                //判断位移是否达标
                if(1)           
                {   //位移达标
                    
                    //判断当前为非连接状态，进行heart rate、SPO2测量
                    if(Cy_BLE_GetNumOfActiveConn()==0)
                    {
                        Cy_GPIO_Write(Work_led_PORT,Work_led_NUM,0);
                        Cy_GPIO_Write(MAX30102_enable_PORT,MAX30102_enable_NUM,1);
            //            MAX30102_init();
            //            MAX30102readHR(testbuf);
                        MAX30102process();
                    }
                }
                else
                {   //位移不达标
                    
                }
                
            }
            
            
            
            #if 1
            if(GetwdtInterruptOccured())
            {
                ClearwdtInterruptOccured();
//                if(Cy_BLE_GetNumOfActiveConn()==0)
//                {
//                    Cy_GPIO_Write(Work_led_PORT,Work_led_NUM,0);
//                    Cy_GPIO_Write(MAX30102_enable_PORT,MAX30102_enable_NUM,1);
////                    MAX30102_init();
//                    MAX30102readHR(testbuf);
//                    MAX30102process();
//                }
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
        }
        if(Cy_BLE_GetNumOfActiveConn()==0 && Cy_SCB_UART_IsTxComplete(UART_HW))
        {
            
//            Cy_GPIO_SetDrivemode(MAX30102_enable_PORT,MAX30102_enable_NUM,CY_GPIO_DM_ANALOG);
//            Cy_GPIO_SetDrivemode(MAX30102_int_PORT,MAX30102_int_NUM,CY_GPIO_DM_ANALOG);
            Cy_SysPm_DeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
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
