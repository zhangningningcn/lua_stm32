/**
  ******************************************************************************
  * @file    app.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    09-November-2015
  * @brief   This file provides all the Application firmware functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/ 

#include "usbd_cdc_core_loopback.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"
/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */


/** @defgroup APP_VCP 
  * @brief Mass storage application module
  * @{
  */ 

/** @defgroup APP_VCP_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup APP_VCP_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup APP_VCP_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup APP_VCP_Private_Variables
  * @{
  */ 
  
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment=4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
   
__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_dev __ALIGN_END ;
uint8_t Rxbuffer[64]; 
__IO uint32_t receive_count =1;
extern __IO uint32_t  data_sent;
/**
  * @}
  */ 


/** @defgroup APP_VCP_Private_FunctionPrototypes
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup APP_VCP_Private_Functions
  * @{
  */ 

/**
  * @brief  Program entry point
  * @param  None
  * @retval None
  */
int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
  this is done through SystemInit() function which is called from startup
  file (startup_stm32fxxx_xx.s) before to branch to application main.
  To reconfigure the default setting of SystemInit() function, refer to
  system_stm32fxxx.c file
  */  
  
  USBD_Init(&USB_OTG_dev,
#ifdef USE_USB_OTG_HS 
            USB_OTG_HS_CORE_ID,
#else            
            USB_OTG_FS_CORE_ID,
#endif  
            &USR_desc, 
            &USBD_CDC_cb, 
            &USR_cb);
    
  /* Main loop */
  while (1)
  {
    /* wait data reception */
    while (VCP_CheckDataReceived()==0);
    
    /* receive one character */
    VCP_ReceiveData(&USB_OTG_dev,Rxbuffer, receive_count);
    
    /*Check to see if we have data yet */
    if (receive_count  != 0)
    {  
      /* wait data sent */
      while (VCP_CheckDataSent()==1); 
      
      /* send myTxBuffer */
      VCP_SendData(&USB_OTG_dev, Rxbuffer, receive_count);
      
      /*INIT received byte count*/
      receive_count =0;
    }
  }
} 

#ifdef USE_FULL_ASSERT
/**
* @brief  assert_failed
*         Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  File: pointer to the source file name
* @param  Line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */ 


/**
  * @}
  */ 


/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
