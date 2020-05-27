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
#include "custom_i2c.h"

static uint8_t I2C_WriteStatus = I2C_WRITECOMPELET;
static uint8_t I2C_ReadStatus = I2C_READCOMPELET;    


void CI2C_Init(void)
{
    /* Initialize SCB for I2C operation */
    (void) Cy_SCB_I2C_Init(I2C_HW, &I2C_config, &I2C_context);
    /* Configure desired data rate.
    * Note that internally configured clock is utilized.
    */
    (void) Cy_SCB_I2C_SetDataRate(I2C_HW, I2C_DATA_RATE_HZ, I2C_CLK_FREQ_HZ);
    /* Hook interrupt service routine and enable interrupt */
    Cy_SysInt_Init(&I2C_SCB_IRQ_cfg, &I2C_Isr);
    NVIC_EnableIRQ(I2C_SCB_IRQ_cfg.intrSrc);
    /* Enable I2C */
    Cy_SCB_I2C_Enable(I2C_HW);
    
    
    Cy_SCB_I2C_RegisterEventCallback(I2C_HW,I2C_callback,&I2C_context);
    
    /* SysPm callback params */
	static cy_stc_syspm_callback_params_t i2ccallbackParams = {
//        /*.mode       =*/CY_SYSPM_BEFORE_TRANSITION,
	    /*.base       =*/ I2C_HW,
	    /*.context    =*/ &I2C_context
	};
    
    static cy_stc_syspm_callback_t I2CDeepSleepCb = {Customer_SCB_I2C_DeepSleepCallback,  /* Callback function */
                                              CY_SYSPM_DEEPSLEEP,       /* Callback type */
                                              0,                        /* Skip mode */
											  &i2ccallbackParams,       	/* Callback params */
                                              NULL, NULL};				/* For internal usage */   
    if(Cy_SysPm_RegisterCallback(&I2CDeepSleepCb))
    {
    }
    else
    {
        Cy_SCB_UART_PutString(UART_HW,"I2C low power callback register failed!!!");
    }


}
#if 1
/*******************************************************************************
* Function Name: Cy_SCB_I2C_DeepSleepCallback
****************************************************************************//**
*
* This function handles the transition of the I2C SCB into and out of
* Deep Sleep mode. It prevents the device from entering Deep Sleep
* mode if the I2C slave or master is actively communicating.
* The  behavior of the I2C SCB in Deep Sleep depends on whether the SCB block is
* wakeup-capable or not:
* * <b>Wakeup-capable</b>: during Deep Sleep mode on incoming I2C slave address 
*   the slave receives address and stretches the clock until the device is 
*   awoken from Deep Sleep mode. If the slave address occurs before the device 
*   enters Deep Sleep mode, the device will not enter Deep Sleep mode.
*   Only the I2C slave can be configured to be a wakeup source from Deep Sleep
*   mode.
* * <b>Not wakeup-capable</b>: the SCB is disabled in Deep Sleep mode. 
*   It is re-enabled if the device fails to enter Deep Sleep mode or when the 
*   device is awoken from Deep Sleep mode. While the SCB is disabled it stops
*   driving the outputs and ignores the inputs. The slave NACKs all incoming
*   addresses.
*
* This function must be called during execution of \ref Cy_SysPm_CpuEnterDeepSleep.
* To do it, register this function as a callback before calling
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
* \note
* Only applicable for <b>rev-08 of the CY8CKIT-062-BLE</b>.
* For proper operation, when the I2C slave is configured to be a wakeup source
* from Deep Sleep mode, this function must be copied and modified by the user.
* The I2C clock disable code must be inserted in the \ref CY_SYSPM_BEFORE_TRANSITION
* and clock enable code in the \ref CY_SYSPM_AFTER_TRANSITION mode processing.
*
*******************************************************************************/
cy_en_syspm_status_t Customer_SCB_I2C_DeepSleepCallback(cy_stc_syspm_callback_params_t *callbackParams, cy_en_syspm_callback_mode_t mode)
{
    CySCB_Type *locBase = (CySCB_Type *) callbackParams->base;
    cy_stc_scb_i2c_context_t *locContext = (cy_stc_scb_i2c_context_t *) callbackParams->context;

    cy_en_syspm_status_t retStatus = CY_SYSPM_FAIL;

    switch(mode)
    {
        case CY_SYSPM_CHECK_READY:
        {
            /* Disable the slave interrupt sources to protect the state */
            Cy_SCB_SetSlaveInterruptMask(locBase, CY_SCB_CLEAR_ALL_INTR_SRC);

            /* If the I2C is in the IDLE state, it is ready for Deep Sleep mode
            * (either the master or the slave is not busy),
            * otherwise return fail and restore the slave interrupt sources.
            */
            if (CY_SCB_I2C_IDLE == locContext->state)
            {
                if (_FLD2BOOL(SCB_CTRL_EC_AM_MODE, SCB_CTRL(locBase)))
                {
                    /* The SCB is wakeup-capable: do not restore the address
                    * match and general call interrupt sources. The next
                    * transaction intended to the slave will be paused
                    * (SCL is stretched) before the address is ACKed because
                    * the corresponding interrupt source is disabled.
                    */
                    Cy_SCB_SetSlaveInterruptMask(locBase, CY_SCB_I2C_SLAVE_INTR_NO_ADDR);
                }
                else
                {
                    /* The SCB is NOT wakeup-capable: disable the I2C. The slave
                    * stops responding to the master and the master stops
                    * driving the bus until the I2C is enabled. This happens
                    * when the device failed to enter into Deep Sleep mode or it
                    * is awaken from Deep Sleep mode.
                    */
                    Cy_SCB_I2C_Disable(locBase, locContext);
                    Cy_SCB_SetSlaveInterruptMask(locBase, CY_SCB_I2C_SLAVE_INTR);
                }

                retStatus = CY_SYSPM_SUCCESS;
            }
            else
            {
                /* Restore the slave interrupt sources */
                Cy_SCB_SetSlaveInterruptMask(locBase, CY_SCB_I2C_SLAVE_INTR);
            }
        }
        break;

        case CY_SYSPM_CHECK_FAIL:
        {
            /* The other driver is not ready for Deep Sleep mode. Restore
            * Active mode configuration.
            */

            if (_FLD2BOOL(SCB_CTRL_EC_AM_MODE, SCB_CTRL(locBase)))
            {
                /* The SCB is wakeup-capable: restore the slave interrupt sources */
                Cy_SCB_SetSlaveInterruptMask(locBase, CY_SCB_I2C_SLAVE_INTR);
            }
            else
            {
                /* The SCB is NOT wakeup-capable: enable the I2C to operate */
                Cy_SCB_I2C_Enable(locBase);
            }

            retStatus = CY_SYSPM_SUCCESS;
        }
        break;

        case CY_SYSPM_BEFORE_TRANSITION:
        {
            /* This code executes inside the critical section. Enabling the
            * active interrupt source makes the interrupt pending in the NVIC.
            * However, the interrupt processing is delayed until the code exits
            * the critical section. The pending interrupt force WFI instruction
            * does nothing and the device remains in Active mode.
            */

            if (_FLD2BOOL(SCB_CTRL_EC_AM_MODE, SCB_CTRL(locBase)))
            {
                /* The SCB is wakeup-capable: enable the I2C wakeup interrupt
                * source. If any transaction was paused, the I2C interrupt
                * becomes pending and prevents entering Deep Sleep mode.
                * The transaction continues as soon as the global interrupts
                * are enabled.
                */
                Cy_SCB_SetI2CInterruptMask(locBase, CY_SCB_I2C_INTR_WAKEUP);

                /* Disable SCB clock */
                SCB_I2C_CFG(locBase) &= (uint32_t) ~CY_SCB_I2C_CFG_CLK_ENABLE_Msk;

                /* IMPORTANT (replace line above for the CY8CKIT-062 rev-08):
                * for proper entering Deep Sleep mode the I2C clock must be disabled.
                * This code must be inserted by the user because the driver
                * does not have access to the clock.
                */
            }
//            Cy_GPIO_SetDrivemode(I2C_sda_PORT,I2C_sda_NUM,CY_GPIO_DM_ANALOG);
//            Cy_GPIO_SetDrivemode(I2C_scl_PORT,I2C_scl_NUM,CY_GPIO_DM_ANALOG);
            
            retStatus = CY_SYSPM_SUCCESS;
        }
        break;

        case CY_SYSPM_AFTER_TRANSITION:
        {
            if (_FLD2BOOL(SCB_CTRL_EC_AM_MODE, SCB_CTRL(locBase)))
            {
                /* Enable SCB clock */
                SCB_I2C_CFG(locBase) |= CY_SCB_I2C_CFG_CLK_ENABLE_Msk;

                /* IMPORTANT (replace line above for the CY8CKIT-062 rev-08):
                * for proper exiting Deep Sleep, the I2C clock must be enabled.
                * This code must be inserted by the user because the driver
                * does not have access to the clock.
                */

                /* The SCB is wakeup-capable: disable the I2C wakeup interrupt
                * source and restore slave interrupt sources.
                */
                Cy_SCB_SetI2CInterruptMask  (locBase, CY_SCB_CLEAR_ALL_INTR_SRC);
                Cy_SCB_SetSlaveInterruptMask(locBase, CY_SCB_I2C_SLAVE_INTR);
            }
            else
            {
                /* The SCB is NOT wakeup-capable: enable the I2C to operate */
                Cy_SCB_I2C_Enable(locBase);
            }
//            Cy_GPIO_SetDrivemode(I2C_sda_PORT,I2C_sda_NUM,I2C_sda_DRIVEMODE);
//            Cy_GPIO_SetDrivemode(I2C_scl_PORT,I2C_scl_NUM,I2C_scl_DRIVEMODE);
            retStatus = CY_SYSPM_SUCCESS;
        }
        break;

        default:
            break;
    }

    return (retStatus);
}    
    
    
    
#else


cy_en_syspm_status_t Customer_SCB_I2C_DeepSleepCallback(cy_stc_syspm_callback_params_t *callbackParams)
{
    CySCB_Type *locBase = (CySCB_Type *) callbackParams->base;
    cy_stc_scb_i2c_context_t *locContext = (cy_stc_scb_i2c_context_t *) callbackParams->context;

    cy_en_syspm_status_t retStatus = CY_SYSPM_FAIL;

    switch(callbackParams->mode)
    {
        case CY_SYSPM_CHECK_READY:
        {
            /* Disable the slave interrupt sources to protect the state */
            Cy_SCB_SetSlaveInterruptMask(locBase, CY_SCB_CLEAR_ALL_INTR_SRC);

            /* If the I2C is in the IDLE state, it is ready for Deep Sleep mode
            * (either the master or the slave is not busy),
            * otherwise return fail and restore the slave interrupt sources.
            */
            if (CY_SCB_I2C_IDLE == locContext->state)
            {
                if (_FLD2BOOL(SCB_CTRL_EC_AM_MODE, locBase->CTRL))
                {
                    /* The SCB is wakeup-capable: do not restore the address
                    * match and general call interrupt sources. The next
                    * transaction intended to the slave will be paused
                    * (SCL is stretched) before the address is ACKed because
                    * the corresponding interrupt source is disabled.
                    */
                    Cy_SCB_SetSlaveInterruptMask(locBase, CY_SCB_I2C_SLAVE_INTR_NO_ADDR);
                }
                else
                {
                    /* The SCB is NOT wakeup-capable: disable the I2C. The slave
                    * stops responding to the master and the master stops
                    * driving the bus until the I2C is enabled. This happens
                    * when the device failed to enter into Deep Sleep mode or it
                    * is awaken from Deep Sleep mode.
                    */
                    Cy_SCB_I2C_Disable(locBase, locContext);
                    Cy_SCB_SetSlaveInterruptMask(locBase, CY_SCB_I2C_SLAVE_INTR);
                }

                retStatus = CY_SYSPM_SUCCESS;
            }
            else
            {
                /* Restore the slave interrupt sources */
                Cy_SCB_SetSlaveInterruptMask(locBase, CY_SCB_I2C_SLAVE_INTR);
            }
        }
        break;

        case CY_SYSPM_CHECK_FAIL:
        {
            /* The other driver is not ready for Deep Sleep mode. Restore
            * Active mode configuration.
            */

            if (_FLD2BOOL(SCB_CTRL_EC_AM_MODE, locBase->CTRL))
            {
                /* The SCB is wakeup-capable: restore the slave interrupt sources */
                Cy_SCB_SetSlaveInterruptMask(locBase, CY_SCB_I2C_SLAVE_INTR);
            }
            else
            {
                /* The SCB is NOT wakeup-capable: enable the I2C to operate */
                Cy_SCB_I2C_Enable(locBase);
            }

            retStatus = CY_SYSPM_SUCCESS;
        }
        break;

        case CY_SYSPM_BEFORE_TRANSITION:
        {
            /* This code executes inside the critical section. Enabling the
            * active interrupt source makes the interrupt pending in the NVIC.
            * However, the interrupt processing is delayed until the code exits
            * the critical section. The pending interrupt force WFI instruction
            * does nothing and the device remains in Active mode.
            */
//====================================================================================            
//            如果使用port 0口作为i2c，会有漏电，需要手动修改io状态。
//            Cy_GPIO_SetDrivemode(I2C_sda_PORT,I2C_sda_NUM,CY_GPIO_DM_ANALOG);
//            Cy_GPIO_SetDrivemode(I2C_scl_PORT,I2C_scl_NUM,CY_GPIO_DM_ANALOG);
//====================================================================================            
            if (_FLD2BOOL(SCB_CTRL_EC_AM_MODE, locBase->CTRL))
            {
                /* The SCB is wakeup-capable: enable the I2C wakeup interrupt
                * source. If any transaction was paused, the I2C interrupt
                * becomes pending and prevents entering Deep Sleep mode.
                * The transaction continues as soon as the global interrupts
                * are enabled.
                */
                Cy_SCB_SetI2CInterruptMask(locBase, CY_SCB_I2C_INTR_WAKEUP);

                /* Disable SCB clock */
                locBase->I2C_CFG &= (uint32_t) ~CY_SCB_I2C_CFG_CLK_ENABLE_Msk;

                /* IMPORTANT (replace line above for the CY8CKIT-062 rev-08):
                * for proper entering Deep Sleep mode the I2C clock must be disabled.
                * This code must be inserted by the user because the driver
                * does not have access to the clock.
                */
            }

            retStatus = CY_SYSPM_SUCCESS;
        }
        break;

        case CY_SYSPM_AFTER_TRANSITION:
        {
            if (_FLD2BOOL(SCB_CTRL_EC_AM_MODE, locBase->CTRL))
            {
                /* Enable SCB clock */
                locBase->I2C_CFG |= CY_SCB_I2C_CFG_CLK_ENABLE_Msk;

                /* IMPORTANT (replace line above for the CY8CKIT-062 rev-08):
                * for proper exiting Deep Sleep, the I2C clock must be enabled.
                * This code must be inserted by the user because the driver
                * does not have access to the clock.
                */

                /* The SCB is wakeup-capable: disable the I2C wakeup interrupt
                * source and restore slave interrupt sources.
                */
                Cy_SCB_SetI2CInterruptMask  (locBase, CY_SCB_CLEAR_ALL_INTR_SRC);
                Cy_SCB_SetSlaveInterruptMask(locBase, CY_SCB_I2C_SLAVE_INTR);
            }
            else
            {
                /* The SCB is NOT wakeup-capable: enable the I2C to operate */
                Cy_SCB_I2C_Enable(locBase);
            }
            
//====================================================================================            
//            如果使用port 0口作为i2c，会有漏电，需要手动修改io状态。            
//            Cy_GPIO_SetDrivemode(I2C_sda_PORT,I2C_sda_NUM,I2C_sda_DRIVEMODE);
//            Cy_GPIO_SetDrivemode(I2C_scl_PORT,I2C_scl_NUM,I2C_scl_DRIVEMODE);
//====================================================================================                
            retStatus = CY_SYSPM_SUCCESS;
        }
        break;

        default:
            break;
    }

    return (retStatus);
}
#endif
/* Implement ISR for I2C_1 */    
void I2C_Isr(void)
{
    Cy_SCB_I2C_Interrupt(I2C_HW, &I2C_context);
}


void I2C_callback(uint32 event)
{
    switch(event)
    {
        case CY_SCB_I2C_MASTER_WR_CMPLT_EVENT:
        I2C_WriteStatus = I2C_WRITECOMPELET;
        break;
        case CY_SCB_I2C_MASTER_RD_CMPLT_EVENT:
        I2C_ReadStatus = I2C_READCOMPELET;
        break;
        default:
        //other events
        break;
    }
}



void CI2C_masterread(uint8_t device_addr,uint8_t registeraddr,uint8_t *buf,uint8_t len)
{
    #if 0
    cy_stc_scb_i2c_master_xfer_config_t transaction;
    
    //write subaddr
    I2C_WriteStatus = I2C_WRITEBUSY;
    transaction.slaveAddress = device_addr;
    transaction.buffer = &subaddr;
    transaction.bufferSize = sizeof(subaddr);
    transaction.xferPending = false;
    Cy_SCB_I2C_MasterWrite(I2C_HW,&transaction,&I2C_context);
    //check master status or use callback to get notification about transaction completion.
    while(I2C_WRITECOMPELET != I2C_WriteStatus);
    //read data
    /* Configure transaction */
    I2C_ReadStatus = I2C_READBUSY;
    transaction.slaveAddress = device_addr;
    transaction.buffer = buf;
    transaction.bufferSize = len;
    transaction.xferPending = false;
    /* Master: request to execute transaction */
    (void) Cy_SCB_I2C_MasterRead(I2C_HW, &transaction, &I2C_context);
    /* Check master status or use callback to get notification about
    * transaction completion.
    */
    while(I2C_READCOMPELET != I2C_ReadStatus);
    #else
    uint8_t status = 0xff;
    cy_en_scb_i2c_status_t  errorStatus;
    
    /* Sends packets to slave using low level PDL library functions. */ 
    errorStatus = Cy_SCB_I2C_MasterSendStart(I2C_HW, device_addr, CY_SCB_I2C_WRITE_XFER, 10, &I2C_context);
    if(errorStatus == CY_SCB_I2C_SUCCESS)
    {
        //send subaddr
        errorStatus = Cy_SCB_I2C_MasterWriteByte(I2C_HW, registeraddr, 10, &I2C_context);
        //read data
        uint32_t cnt = 0UL;
        cy_en_scb_i2c_command_t cmd = CY_SCB_I2C_ACK;
        //send restart
        if(CY_SCB_I2C_SUCCESS == 
            Cy_SCB_I2C_MasterSendReStart(I2C_HW,device_addr,CY_SCB_I2C_READ_XFER,10,&I2C_context))
        {
            /* read data from the buffer */
            do
            {
                if (cnt == (len - 1UL))
                {
                    /* The last byte must be NACKed */
                    cmd = CY_SCB_I2C_NAK;
                }
                /* Write byte and receive ACK/NACK response */
                errorStatus = Cy_SCB_I2C_MasterReadByte(I2C_HW,cmd,&buf[cnt],10, &I2C_context);
                ++cnt;
            }
            while((errorStatus == CY_SCB_I2C_SUCCESS) && (cnt < len));   
        }
        
    }
   
    /* Check status of transaction */
    if ((errorStatus == CY_SCB_I2C_SUCCESS)           ||
        (errorStatus == CY_SCB_I2C_MASTER_MANUAL_NAK) ||
        (errorStatus == CY_SCB_I2C_MASTER_MANUAL_ADDR_NAK))
    {
        /* Send Stop condition on the bus */
        if (Cy_SCB_I2C_MasterSendStop(I2C_HW, 10, &I2C_context) == CY_SCB_I2C_SUCCESS)
        {
            status = 0;
        }
    }    
    #endif
}

void CI2C_masterwrite(uint8_t device_addr,uint8_t registeraddr,uint8_t*buf,uint8_t len)
{
    #if 0
    cy_stc_scb_i2c_master_xfer_config_t transaction;
    
    //write
    transaction.slaveAddress = device_addr;
    transaction.buffer = &subaddr;
    transaction.bufferSize = sizeof(subaddr);
    transaction.xferPending = false;
    Cy_SCB_I2C_MasterWrite(I2C_HW,&transaction,&I2C_context);
    //check master status or use callback to get notification about transaction completion.
    while(I2C_WRITECOMPELET != I2C_WriteStatus);
        
    //write
    transaction.slaveAddress = device_addr;
    transaction.buffer = buf;
    transaction.bufferSize = len;
    transaction.xferPending = false;
    Cy_SCB_I2C_MasterWrite(I2C_HW,&transaction,&I2C_context);
    //check master status or use callback to get notification about transaction completion.
    while(I2C_WRITECOMPELET != I2C_WriteStatus);
    #endif
    #if 1
    uint8_t status = 0xff;
    cy_en_scb_i2c_status_t  errorStatus;
    
    /* Sends packets to slave using low level PDL library functions. */ 
    errorStatus = Cy_SCB_I2C_MasterSendStart(I2C_HW, device_addr, CY_SCB_I2C_WRITE_XFER, 10, &I2C_context);
    if(errorStatus == CY_SCB_I2C_SUCCESS)
    {
        //send subaddr
        errorStatus = Cy_SCB_I2C_MasterWriteByte(I2C_HW, registeraddr, 10, &I2C_context);
        //send data
        uint32_t cnt = 0UL;
        /* write data from the buffer */
        do
        {
            /* Write byte and receive ACK/NACK response */
            errorStatus = Cy_SCB_I2C_MasterWriteByte(I2C_HW, buf[cnt], 10, &I2C_context);
            ++cnt;
        }
        while((errorStatus == CY_SCB_I2C_SUCCESS) && (cnt < len));   
    }
   
    /* Check status of transaction */
    if ((errorStatus == CY_SCB_I2C_SUCCESS)           ||
        (errorStatus == CY_SCB_I2C_MASTER_MANUAL_NAK) ||
        (errorStatus == CY_SCB_I2C_MASTER_MANUAL_ADDR_NAK))
    {
        /* Send Stop condition on the bus */
        if (Cy_SCB_I2C_MasterSendStop(I2C_HW, 10, &I2C_context) == CY_SCB_I2C_SUCCESS)
        {
            status = 0;
        }
    }
//    return status;
    #endif
} 

/* [] END OF FILE */
