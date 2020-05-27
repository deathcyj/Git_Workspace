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
#include "custom_uart.h"



uint8_t uart_error;

uint8_t uart_interrupt = UART_RX_SUCCESS;
uint8 len = 0;
uint8 Uart_Rxbuf[UART_RXBUFFLEN];

/***************************************************************************//**
* Function Name: ISR_UART
********************************************************************************
*
* Summary:
*  This function is registered to be called when UART interrupt occurs.
*  (Note that only RX fifo not empty interrupt is unmasked)
*  Whenever there is a data in UART RX fifo, Get that data and Put it into 
*  UART TX fifo which will be transmitted to terminal
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
void ISR_UART(void)
{
    uint8_t temp=0;
    /* Check for "RX fifo not empty interrupt" */
    if((UART_HW->INTR_RX_MASKED & SCB_INTR_RX_MASKED_NOT_EMPTY_Msk ) != 0)
	{
        /* Clear UART "RX fifo not empty interrupt" */
		UART_HW->INTR_RX = UART_HW->INTR_RX & SCB_INTR_RX_NOT_EMPTY_Msk;        
            
        /* Get the character from terminal */
        temp = Cy_SCB_UART_Get(UART_HW);
        if(len < UART_RXBUFFLEN)
        {
            if(temp == 0x0d)
            {
                len |= (uint16)(1<<7);
                Uart_Rxbuf[(len++)&0x7f] = temp;
            }
            else
            {
                if((len & (uint16)(1 << 7)) && (temp == 0x0a))
                {
                    uart_interrupt |= 1;
                    len--;
                }
                else
                {
                    if((len&0x7f) < ERR_UART_RXDATA_OVERFLOW)
                    {
                        Uart_Rxbuf[(len++)&0x7f] = temp;
                        len &=~(uint16)(1<<7);
                    }
                }
            }
        }
        else
        {
            //Error if the data length is overflow
            uart_error = ERR_UART_RXDATA_OVERFLOW;
        }
                
	}   
    else
    {
        /* Error if any other interrupt occurs */
        uart_error = ERR_UART_OTHERISR;
    }
}

void CUART_Init(void)
{
    UART_Start();
    
     /* Unmasking only the RX fifo not empty interrupt bit */
    UART_HW->INTR_RX_MASK = SCB_INTR_RX_MASK_NOT_EMPTY_Msk;
    Cy_SCB_SetRxInterruptMask(UART_HW,SCB_INTR_RX_NOT_EMPTY_Msk);       //使能中断
    
    /* Interrupt Settings for UART */    
    Cy_SysInt_Init(&UART_SCB_IRQ_cfg, ISR_UART);
    
    /* Enable the interrupt */
    NVIC_EnableIRQ(UART_SCB_IRQ_cfg.intrSrc);
    

    
    /* SysPm callback params */
	static cy_stc_syspm_callback_params_t uartcallbackParams = {
	    /*.base       =*/ UART_HW,
	    /*.context    =*/ &UART_context
	};
    static cy_stc_syspm_callback_t UARTDeepSleepCb = {Custom_SCB_UART_DeepSleepCallback,  /* Callback function */
                                              CY_SYSPM_DEEPSLEEP,       /* Callback type */
                                              0,                        /* Skip mode */
											  &uartcallbackParams,       	/* Callback params */
                                              NULL, NULL};				/* For internal usage */    
    if(Cy_SysPm_RegisterCallback(&UARTDeepSleepCb))
    {
        //register success
    }
    else
    {
        //error
        Cy_SCB_UART_PutString(UART_HW,"Uart low power callback register failed!!!\r\n");
        while(1);
    }

    Cy_SCB_UART_PutString(UART_HW,"test test\r\n");
}

#if 1
    
uint8_t GetUartinterrupte(void)
{
    return uart_interrupt;
}

void ClearUartinterrupte(void)
{
    uart_interrupt = 0;
}

uint8_t GetUartdata(uint8_t **data)
{
    *data = Uart_Rxbuf;
    return (len&0x7f);
}
void ClearUartdata(void)
{
    memset(Uart_Rxbuf,0,sizeof(Uart_Rxbuf));
    len = 0;
}
uint8_t GetUartErrorstate(void)
{
    return uart_error;
}
void ClearUartErrorstate(void)
{
    uart_error = 0;
}

void UartErrorhandle(void)
{
    switch(uart_error)
    {
        case ERR_UART_OTHERISR:
        Cy_SCB_GetInterruptCause(UART_HW);
        
        Cy_SCB_GetRxInterruptMask(UART_HW);                         //获取使能中断
        Cy_SCB_GetRxInterruptStatusMasked(UART_HW); 
        
        Cy_SCB_SetRxInterrupt(UART_HW,SCB_INTR_RX_NOT_EMPTY_Msk);           //产生中断
        Cy_SCB_SetRxInterruptMask(UART_HW,SCB_INTR_RX_NOT_EMPTY_Msk);       //使能中断
        Cy_SCB_GetRxInterruptStatus(UART_HW);                               //获取当前RX中断状态
        Cy_SCB_ClearRxInterrupt(UART_HW,SCB_INTR_RX_NOT_EMPTY_Msk);         //清除中断
        
        break;
        
        case ERR_UART_RXDATA_OVERFLOW:
        break;
        
        default:
        break;
        
    }
    ClearUartErrorstate();
}

#endif

#if 1
/*******************************************************************************
* Function Name: Cy_SCB_UART_DeepSleepCallback
****************************************************************************//**
*
* This function handles the transition of the SCB UART into and out of
* Deep Sleep mode. It prevents the device from entering Deep Sleep 
* mode if the UART is transmitting data or has any data in the RX FIFO. If the
* UART is ready to enter Deep Sleep mode, it is disabled. The UART is enabled
* when the device fails to enter Deep Sleep mode or it is awakened from
* Deep Sleep mode. While the UART is disabled, it stops driving the outputs
* and ignores the inputs. Any incoming data is ignored. Refer to section 
* \ref group_scb_uart_lp for more information about UART pins when SCB disabled.
*
* This function must be called during execution of \ref Cy_SysPm_CpuEnterDeepSleep,
* to do it, register this function as a callback before calling
* \ref Cy_SysPm_CpuEnterDeepSleep : specify \ref CY_SYSPM_DEEPSLEEP as the callback
* type and call \ref Cy_SysPm_RegisterCallback.
*
* \param callbackParams
* The pointer to the callback parameters structure
* \ref cy_stc_syspm_callback_params_t.
*
* \param mode
* Callback mode, see \ref cy_en_syspm_callback_mode_t
*
* \return
* \ref cy_en_syspm_status_t
*
*******************************************************************************/
cy_en_syspm_status_t Custom_SCB_UART_DeepSleepCallback(cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode)
{
    cy_en_syspm_status_t retStatus = CY_SYSPM_FAIL;

    CySCB_Type *locBase = (CySCB_Type *) callbackParams->base;
    cy_stc_scb_uart_context_t *locContext = (cy_stc_scb_uart_context_t *) callbackParams->context;

    switch(mode)
    {
        case CY_SYSPM_CHECK_READY:
        {
            /* Check whether the High-level API is not busy executing the transmit
            * or receive operation.
            */
            if ((0UL == (CY_SCB_UART_TRANSMIT_ACTIVE & Cy_SCB_UART_GetTransmitStatus(locBase, locContext))) &&
                (0UL == (CY_SCB_UART_RECEIVE_ACTIVE  & Cy_SCB_UART_GetReceiveStatus (locBase, locContext))))
            {
                /* If all data elements are transmitted from the TX FIFO and
                * shifter and the RX FIFO is empty: the UART is ready to enter
                * Deep Sleep mode.
                */
                if (Cy_SCB_UART_IsTxComplete(locBase))
                {
                    if (0UL == Cy_SCB_UART_GetNumInRxFifo(locBase))
                    {
                        /* Disable the UART. The transmitter stops driving the
                        * lines and the receiver stops receiving data until
                        * the UART is enabled.
                        * This happens when the device failed to enter Deep
                        * Sleep or it is awaken from Deep Sleep mode.
                        */
                        Cy_SCB_UART_Disable(locBase, locContext);

                        retStatus = CY_SYSPM_SUCCESS;
                    }
                }
            }
        }
        break;

        case CY_SYSPM_CHECK_FAIL:
        {
            /* The other driver is not ready for Deep Sleep mode. Restore the
            * Active mode configuration.
            */

            /* Enable the UART to operate */
            Cy_SCB_UART_Enable(locBase);

            retStatus = CY_SYSPM_SUCCESS;
        }
        break;

        case CY_SYSPM_BEFORE_TRANSITION:
            /* Do noting: the UART is not capable of waking up from
            * Deep Sleep mode.
            */
        Cy_GPIO_SetDrivemode(Uart_RX_PORT,Uart_RX_NUM,CY_GPIO_DM_ANALOG);
        break;

        case CY_SYSPM_AFTER_TRANSITION:
        {
            /* Enable the UART to operate */
            Cy_SCB_UART_Enable(locBase);

            Cy_GPIO_SetDrivemode(Uart_RX_PORT,Uart_RX_NUM,CY_GPIO_DM_HIGHZ);
            retStatus = CY_SYSPM_SUCCESS;
        }
        break;

        default:
            break;
    }

    return (retStatus);
}    
#else
/*******************************************************************************
* Function Name: Cy_SCB_UART_DeepSleepCallback
****************************************************************************//**
*
* This function handles the transition of the SCB UART into and out of
* Deep Sleep mode. It prevents the device from entering Deep Sleep 
* mode if the UART is transmitting data or has any data in the RX FIFO. If the
* UART is ready to enter Deep Sleep mode, it is disabled. The UART is enabled
* when the device fails to enter Deep Sleep mode or it is awakened from
* Deep Sleep mode. While the UART is disabled, it stops driving the outputs
* and ignores the inputs. Any incoming data is ignored.
*
* This function must be called during execution of \ref Cy_SysPm_DeepSleep,
* to do it, register this function as a callback before calling
* \ref Cy_SysPm_DeepSleep : specify \ref CY_SYSPM_DEEPSLEEP as the callback
* type and call \ref Cy_SysPm_RegisterCallback.
*
* \param callbackParams
* The pointer to the callback parameters structure
* \ref cy_stc_syspm_callback_params_t.
*
* \return
* \ref cy_en_syspm_status_t
*
*******************************************************************************/
cy_en_syspm_status_t Custom_SCB_UART_DeepSleepCallback(cy_stc_syspm_callback_params_t *callbackParams)
{
    cy_en_syspm_status_t retStatus = CY_SYSPM_FAIL;

    CySCB_Type *locBase = (CySCB_Type *) callbackParams->base;
    cy_stc_scb_uart_context_t *locContext = (cy_stc_scb_uart_context_t *) callbackParams->context;

    switch(callbackParams->mode)
    {
        case CY_SYSPM_CHECK_READY:
        {
            /* Check whether the High-level API is not busy executing the transmit
            * or receive operation.
            */
            if ((0UL == (CY_SCB_UART_TRANSMIT_ACTIVE & Cy_SCB_UART_GetTransmitStatus(locBase, locContext))) &&
                (0UL == (CY_SCB_UART_RECEIVE_ACTIVE  & Cy_SCB_UART_GetReceiveStatus (locBase, locContext))))
            {
                /* If all data elements are transmitted from the TX FIFO and
                * shifter and the RX FIFO is empty: the UART is ready to enter
                * Deep Sleep mode.
                */
                if (Cy_SCB_UART_IsTxComplete(locBase))
                {
                    if (0UL == Cy_SCB_UART_GetNumInRxFifo(locBase))
                    {
                        /* Disable the UART. The transmitter stops driving the
                        * lines and the receiver stops receiving data until
                        * the UART is enabled.
                        * This happens when the device failed to enter Deep
                        * Sleep or it is awaken from Deep Sleep mode.
                        */
                        Cy_SCB_UART_Disable(locBase, locContext);

                        retStatus = CY_SYSPM_SUCCESS;
                    }
                }
            }
        }
        break;

        case CY_SYSPM_CHECK_FAIL:
        {
            /* The other driver is not ready for Deep Sleep mode. Restore the
            * Active mode configuration.
            */

            /* Enable the UART to operate */
            Cy_SCB_UART_Enable(locBase);

            retStatus = CY_SYSPM_SUCCESS;
        }
        break;

        case CY_SYSPM_BEFORE_TRANSITION:
            /* Do noting: the UART is not capable of waking up from
            * Deep Sleep mode.
            */
//            Cy_GPIO_SetDrivemode(Uart_TX_PORT,Uart_TX_NUM,CY_GPIO_DM_ANALOG);
            Cy_GPIO_SetDrivemode(Uart_RX_PORT,Uart_RX_NUM,CY_GPIO_DM_ANALOG);
        break;

        case CY_SYSPM_AFTER_TRANSITION:
        {
//            Cy_GPIO_SetDrivemode(Uart_TX_PORT,Uart_TX_NUM,CY_GPIO_DM_STRONG_IN_OFF);
            Cy_GPIO_SetDrivemode(Uart_RX_PORT,Uart_RX_NUM,CY_GPIO_DM_HIGHZ);
            /* Enable the UART to operate */
            Cy_SCB_UART_Enable(locBase);
        
            retStatus = CY_SYSPM_SUCCESS;
        }
        break;

        default:
            break;
    }

    return (retStatus);
}
#endif
/* [] END OF FILE */
