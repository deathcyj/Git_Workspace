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
#include "MAX30102.h"
#include "custom_i2c.h"
#include "algorithm.h"



void MAX30102_init(void)
{
    uint8_t tempdata=0;

    //reset 
    tempdata = 0x40;
	CI2C_masterwrite(MAX30102_address,REG_MODE_CONFIG,&tempdata,1);
    Cy_SysLib_Delay(1);
	CI2C_masterwrite(MAX30102_address,REG_MODE_CONFIG,&tempdata,1);
    Cy_SysLib_Delay(2);
    
    //init 
    tempdata = 0x0c;
    CI2C_masterwrite(MAX30102_address,REG_INTR_ENABLE_1,&tempdata,1);	// INTR setting
    tempdata = 0x00;
	CI2C_masterwrite(MAX30102_address,REG_INTR_ENABLE_2,&tempdata,1);
	CI2C_masterwrite(MAX30102_address,REG_FIFO_WR_PTR,&tempdata,1);  	//FIFO_WR_PTR[4:0]
	CI2C_masterwrite(MAX30102_address,REG_OVF_COUNTER,&tempdata,1);  	//OVF_COUNTER[4:0]
	CI2C_masterwrite(MAX30102_address,REG_FIFO_RD_PTR,&tempdata,1);  	//FIFO_RD_PTR[4:0]
    tempdata = 0x0f;
	CI2C_masterwrite(MAX30102_address,REG_FIFO_CONFIG,&tempdata,1);  	//sample avg = 1, fifo rollover=false, fifo almost full = 17
    tempdata = 0x03;
	CI2C_masterwrite(MAX30102_address,REG_MODE_CONFIG,&tempdata,1);  	//0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
	tempdata = 0x27;
    CI2C_masterwrite(MAX30102_address,REG_SPO2_CONFIG,&tempdata,1);  	// SPO2_ADC range = 4096nA, SPO2 sample rate (100 Hz), LED pulseWidth (400uS)  
	tempdata = 0x24;
    CI2C_masterwrite(MAX30102_address,REG_LED1_PA,&tempdata,1);   	//Choose value for ~ 7mA for LED1
	CI2C_masterwrite(MAX30102_address,REG_LED2_PA,&tempdata,1);   	//Choose value for ~ 7mA for LED2
	tempdata = 0x7f;
    CI2C_masterwrite(MAX30102_address,REG_PILOT_PA,&tempdata,1);   	//Choose value for ~ 25mA for Pilot LED
    
}
uint8_t wr_prt,rd_prt;
uint8_t avaliablesamplenum=0;
#if 0
void MAX30102readHR(uint8_t *data)
{
    CI2C_masterread(MAX30102_address,0xff,&avaliablesamplenum,1);
    CI2C_masterread(MAX30102_address,REG_FIFO_WR_PTR,&wr_prt,1);
    CI2C_masterread(MAX30102_address,REG_FIFO_RD_PTR,&rd_prt,1);
    avaliablesamplenum = wr_prt-rd_prt;
    CI2C_masterread(MAX30102_address,REG_FIFO_DATA,data,192);
    
}
#else
#define MAX_BRIGHTNESS 255    
    
uint32_t aun_red_buffer[500];
uint32_t aun_ir_buffer[500];
int32_t n_sp02; //SPO2 value
int8_t ch_spo2_valid;   //indicator to show if the SP02 calculation is valid
int32_t n_heart_rate;   //heart rate value
int8_t  ch_hr_valid;    //

uint16_t i=0;
uint8_t temp[6]={0};
uint32_t un_min=0x3fff,un_max=0,un_prev_data;

int32_t n_brightness;
float f_temp;
uint8_t temp_num=0;
//	uint8_t temp[6];
//	uint8_t str[100];
uint8_t dis_hr=0,dis_spo2=0;
void MAX30102readHR(uint8_t *data)
{
    
    for(i=0;i<500;i++)
    {
//        while(MAX30102_INT==1);   //wait until the interrupt pin asserts
        while(Cy_GPIO_Read(MAX30102_int_PORT,MAX30102_int_NUM)==1);
		CI2C_masterread(MAX30102_address,REG_FIFO_DATA,temp,6);
		aun_red_buffer[i] =  (long)((long)((long)temp[0]&0x03)<<16) | (long)temp[1]<<8 | (long)temp[2];    // Combine values to get the actual number
		aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03)<<16) |(long)temp[4]<<8 | (long)temp[5];   // Combine values to get the actual number
            
        if(un_min>aun_red_buffer[i])
            un_min=aun_red_buffer[i];    //update signal min
        if(un_max<aun_red_buffer[i])
            un_max=aun_red_buffer[i];    //update signal max
    }
    
    //calculate heart rate and SpO2 after first 500 samples (first 5 seconds of samples)
    maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, 500, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid); 
    
    
}


void MAX30102process(void)
{
    i=0;
    un_min=0x3FFFF;
    un_max=0;
	
	//dumping the first 100 sets of samples in the memory and shift the last 400 sets of samples to the top
    for(i=100;i<500;i++)
    {
        aun_red_buffer[i-100]=aun_red_buffer[i];
        aun_ir_buffer[i-100]=aun_ir_buffer[i];
        
        //update the signal min and max
        if(un_min>aun_red_buffer[i])
        un_min=aun_red_buffer[i];
        if(un_max<aun_red_buffer[i])
        un_max=aun_red_buffer[i];
    }
	//take 100 sets of samples before calculating the heart rate.
    for(i=400;i<500;i++)
    {
        un_prev_data=aun_red_buffer[i-1];
//        while(MAX30102_INT==1);
        while(Cy_GPIO_Read(MAX30102_int_PORT,MAX30102_int_NUM)==1);
//        max30102_FIFO_ReadBytes(REG_FIFO_DATA,temp);
        CI2C_masterread(MAX30102_address,REG_FIFO_DATA,temp,6);
		aun_red_buffer[i] =  (long)((long)((long)temp[0]&0x03)<<16) | (long)temp[1]<<8 | (long)temp[2];    // Combine values to get the actual number
		aun_ir_buffer[i] = (long)((long)((long)temp[3] & 0x03)<<16) |(long)temp[4]<<8 | (long)temp[5];   // Combine values to get the actual number
    
        if(aun_red_buffer[i]>un_prev_data)
        {
            f_temp=aun_red_buffer[i]-un_prev_data;
            f_temp/=(un_max-un_min);
            f_temp*=MAX_BRIGHTNESS;
            n_brightness-=(int)f_temp;
            if(n_brightness<0)
                n_brightness=0;
        }
        else
        {
            f_temp=un_prev_data-aun_red_buffer[i];
            f_temp/=(un_max-un_min);
            f_temp*=MAX_BRIGHTNESS;
            n_brightness+=(int)f_temp;
            if(n_brightness>MAX_BRIGHTNESS)
                n_brightness=MAX_BRIGHTNESS;
        }
		//send samples and calculation result to terminal program through UART
		if(ch_hr_valid == 1 && n_heart_rate<120)//**/ ch_hr_valid == 1 && ch_spo2_valid ==1 && n_heart_rate<120 && n_sp02<101
		{
			dis_hr = n_heart_rate;
			dis_spo2 = n_sp02;
		}
		else
		{
			dis_hr = 0;
			dis_spo2 = 0;
		}
//			printf("HR=%i, ", n_heart_rate); 
//			printf("HRvalid=%i, ", ch_hr_valid);
//			printf("SpO2=%i, ", n_sp02);
//			printf("SPO2Valid=%i\r\n", ch_spo2_valid);
	}
    maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, 500, aun_red_buffer, &n_sp02, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
		
}
#endif


/* [] END OF FILE */
