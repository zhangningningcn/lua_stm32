//#include "stm32f10x.h"
#include "app_usart.h"
#include "user_define.h"
#include "app_misc.h"
#include "io.h"
#include "xmodem.h"
#include <string.h>

#include "usbd_cdc_core_loopback.h"
#include "usbd_usr.h"
#include "usb_conf.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"
#include "module_comm.h"

#define USART_Lua_IRQHandler   USART1_IRQHandler

/* These are external variables imported from CDC core to be used for IN 
   transfer management. */
//extern uint8_t  APP_Rx_Buffer []; /* Write CDC received data in this buffer.
//                                     These data will be sent over USB IN endpoint
//                                     in the CDC core functions. */
//extern uint32_t APP_Rx_ptr_in;    /* Increment this pointer or roll it back to
//                                     start address when writing received data
 //                                    in the buffer APP_Rx_Buffer. */
                                     
__IO uint32_t receive_count;



static char RxBuffer[USART_Lua_RxBufferSize];
static char TxBuffer[USART_Lua_TxBufferSize];
static int MaxReceive = USART_Lua_RxBufferSize;//
static int RxState;//
//static int TxState;
static int RxCounter;
static int TxCounter;
//static int RxCTick;
static int State;
static int UsartRxDataTick;
//static int Terminal;
static TimeOutActionList *TimeOutActionRun=NULL;

static int RxIndex;
static int UsartDataState;

int USART_Lua_SendChars_from_isr(const char ch,int len);
int USART_Lua_SendS_from_isr(const char * buf,int len);
//uint16_t VCP_DataTx (uint8_t data);

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles USART_Lua global interrupt request.
  * @param  None
  * @retval None
  */
void USART_Lua_IRQHandler(void)
{
  if(USART_GetITStatus(USART_Lua, USART_IT_RXNE) != RESET)
  {
      uint8_t temp;
    /* Read one byte from the receive data register */
    temp = USART_ReceiveData(USART_Lua);
    //getTick
    if(RxState == 0 && RxCounter < USART_Lua_RxBufferSize) {
        if(State == USART_Lua_STATE_NORMAL) {
            switch(UsartDataState) {
                case 0:
                    if(temp == '\n' || temp == '\r') {
                        USART_Lua_SendS_from_isr("\r\n",2);
                        RxBuffer[RxCounter] = '\n';
                        RxState = 1;
                    }
                    else if(temp == '\b'){
                        if(RxIndex > 0) {
                            if(RxIndex == RxCounter) {
                                USART_Lua_SendS_from_isr("\b \b",3);
                                RxCounter--;
                                RxIndex--;
                            }
                            else {
                                RxIndex--;
                                for(int i=RxIndex;i<=RxCounter;i++) {
                                    RxBuffer[i] = RxBuffer[i+1];
                                }
                                RxBuffer[RxCounter-1] = ' ';
                                USART_Lua_SendS_from_isr("\b",1);
                                int sendlen = RxCounter-RxIndex;
                                USART_Lua_SendS_from_isr(&RxBuffer[RxIndex],sendlen);
                                USART_Lua_SendChars_from_isr('\b',sendlen);
                                RxCounter--;
                            }
                        }
                                    //return;
                    }
                    else if(temp == 0x1B) {
                        //RxIndex--;
                        //USART_SendData(USART_Lua, 0x1B);
                        UsartDataState = 1;
                    }
                    else {
                        if(RxIndex != RxCounter){
                            for(int i=RxCounter;i>=RxIndex;i--) {
                                RxBuffer[i+1] = RxBuffer[i];
                            }
                            RxBuffer[RxIndex] = temp;
                            int sendlen = RxCounter-RxIndex;
                            USART_Lua_SendS_from_isr(&RxBuffer[RxIndex],sendlen+1);
                            USART_Lua_SendChars_from_isr('\b',sendlen);
                        }
                        else {
                            RxBuffer[RxIndex] = temp;
                            USART_SendData(USART_Lua, temp);
                        }
                        RxCounter++;
                        RxIndex++;
                    }
                    break;
                case 1:
                    if(temp == 0x5B) {
                        UsartDataState = 2;
                    }
                    else{
                        UsartDataState = 0;
                    }
                    break;
                case 2:
                    if(temp == 0x44) { //left
                        RxIndex--;
                        USART_SendData(USART_Lua, '\b');
                    }
                    if(temp == 0x43) { //right
                        USART_SendData(USART_Lua, RxBuffer[RxIndex]);
                        RxIndex++;
                    }
                    UsartDataState = 0;
                    break;
            }
        }
        else if(State == USART_Lua_STATE_TRANSFILE) {
            RxBuffer[RxCounter] = temp;
            UsartRxDataTick = getTick();
            if(RxCounter == 0) {
                if(temp == XMODE_SOH) {
                    MaxReceive = 133;
                }
                else {
                    MaxReceive = 1;
                }
            }
            RxCounter++;
        }
        if(RxCounter >= MaxReceive){
            RxState = 1;
            //RxCounter = 0;
        }
        
    }
  }
  
  if(USART_GetITStatus(USART_Lua, USART_IT_TXE) != RESET)
  {   
    if(TxBuffer[TxCounter&0x7fff] == 0)
    {
      /* Disable the USART_Lua Transmit interrupt */
      USART_ITConfig(USART_Lua, USART_IT_TXE, DISABLE);
        TxBuffer[0] = '\0';
      TxCounter = 0;
    }
        else {
			/* Write one byte to the transmit data register */
			USART_SendData(USART_Lua, TxBuffer[TxCounter&0x7fff]);
      TxCounter++;
		}
  }
}


/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
static void RCC_Configuration(void)
{   

    #ifdef STM32F4XX
  /* Enable GPIO clock */
  RCC_AHB2PeriphClockCmd(USART_Lua_TX_GPIO_CLK | USART_Lua_RX_GPIO_CLK, ENABLE);
  
  /* Enable USART clock */
  USART_Lua_CLK_INIT(USART_Lua_CLK, ENABLE);

    #else
  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(USART_Lua_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);

  /* Enable USART_Lua Clock */
  RCC_APB2PeriphClockCmd(USART_Lua_CLK, ENABLE);  
  
  #endif
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
static void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  #ifdef STM32F4XX
  
  /* Connect USART pins to AF7 */
  //GPIO_PinAFConfig(USARTx_TX_GPIO_PORT, USARTx_TX_SOURCE, USARTx_TX_AF);
  //GPIO_PinAFConfig(USARTx_RX_GPIO_PORT, USARTx_RX_SOURCE, USARTx_RX_AF);
  
  /* Configure USART Tx and Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = USART_Lua_TX_PIN;
  GPIO_Init(USART_Lua_TX_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = USART_Lua_RX_PIN;
  GPIO_Init(USART_Lua_RX_GPIO_PORT, &GPIO_InitStructure);

  /* Enable the USART OverSampling by 8 */
  USART_OverSampling8Cmd(USART_Lua, ENABLE);  
  
  #else
  
  /* Configure USART_Lua Rx as input floating */
  GPIO_InitStructure.GPIO_Pin = USART_Lua_RxPin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(USART_Lua_GPIO, &GPIO_InitStructure);
  
  
  /* Configure USART_Lua Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = USART_Lua_TxPin;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(USART_Lua_GPIO, &GPIO_InitStructure);
  #endif
  PinUse(SYS_PIN,MODULE_GPIOA,USART_Lua_TX_SOURCE,PIN_STATE_UNUSE);
  PinUse(SYS_PIN,MODULE_GPIOA,USART_Lua_RX_SOURCE,PIN_STATE_UNUSE);
}


/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure the NVIC Preemption Priority Bits */  
  //NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART_Lua_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void USART_Lua_Init(void) {
    USART_InitTypeDef USART_InitStructure;
  /* System Clocks Configuration */
  RCC_Configuration();
       
  /* NVIC configuration */
  NVIC_Configuration();

  /* Configure the GPIO ports */
  GPIO_Configuration();
  
  
  USART_InitStructure.USART_BaudRate = USART_Lua_BaudRate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  /* Configure USART_Lua */
  USART_Init(USART_Lua, &USART_InitStructure);
  
  
  /* Enable USART_Lua Receive and Transmit interrupts */
  USART_ITConfig(USART_Lua, USART_IT_RXNE, ENABLE);
  //USART_ITConfig(USART_Lua, USART_IT_TXE, ENABLE);


  /* Enable the USART_Lua */
  USART_Cmd(USART_Lua, ENABLE);
  
    RxCounter = 0;
    TxCounter = 0;
    RxState = 0;
    State = USART_Lua_STATE_NORMAL;
  //TxState = 0;
}

int USART_Lua_Recive(char * buf,int len) {
//  char *p_buffer = buf;
    char *p_data;
//  int lentemp;
    int bufferdatalen;
    if(RxState == 0) {
        return -1;
    }
    p_data = &RxBuffer[RxCounter];
    bufferdatalen = strlen(p_data);
    if(len < bufferdatalen) {
        bufferdatalen = len;
    }
    memcpy(buf,RxBuffer,bufferdatalen);
    RxCounter += bufferdatalen;
    if(RxBuffer[RxCounter] == '\0') {
        RxCounter = 0;
        RxState = 0;
    }
    
    return bufferdatalen;
}
int USART_Lua_Send(const char * buf,int len) {
    if(len == 0) {
        return 0;
    }
    else if(len >= USART_Lua_TxBufferSize) {
        len = USART_Lua_TxBufferSize - 1;
    }
    if(State < USB_Lua_STATE) {
        while(TxCounter != 0);
        memcpy(TxBuffer,buf,len);
        USART_ITConfig(USART_Lua, USART_IT_TXE, ENABLE);
        USART_SendData(USART_Lua, TxBuffer[TxCounter++]);
    }
    else {
        VCP_SendData(&USB_OTG_dev, buf, len);
    }
    return len;
}
int USART_Lua_ReciveS(char * buf,int len) {
    char *p_data;
    int bufferdatalen;
    if(RxState == 0) {
        return -1;
    }
    p_data = &RxBuffer[RxCounter];
    bufferdatalen = strlen(p_data);
    len--;
    if(len < bufferdatalen) {
        bufferdatalen = len;
    }
    memcpy(buf,RxBuffer,bufferdatalen);
    *buf = '\0';
    RxCounter += bufferdatalen;
    if(RxBuffer[RxCounter] == '\0') {
        RxCounter = 0;
        RxState = 0;
    }
    
    return bufferdatalen;
}
int USART_Lua_SendS(const char * buf,char endchar) {
    uint32_t len = strlen(buf);
    if(len == 0) {
        return 0;
    }
    else if(len >= USART_Lua_TxBufferSize-2) {
        len = USART_Lua_TxBufferSize - 3;
    }
    if(State < USB_Lua_STATE) {
        while(TxCounter != 0);
        memcpy(TxBuffer,buf,len);
        TxBuffer[len++] = endchar;
        TxBuffer[len] = 0;
        USART_ITConfig(USART_Lua, USART_IT_TXE, ENABLE);
        USART_SendData(USART_Lua, TxBuffer[TxCounter++]);
    }
    else {
        VCP_SendData(&USB_OTG_dev, buf, len);
    }
    return len;
}
int USART_Lua_SendS_from_isr(const char * buf,int len) {
	int len2;
	len2 = strlen(TxBuffer);
	
    if(len == 0) {
        return 0;
    }
    else if(len+len2 >= USART_Lua_TxBufferSize) {
        len = USART_Lua_TxBufferSize - len2 - 1;
    }
    if(State < USB_Lua_STATE) {
        memcpy(TxBuffer+len2,buf,len);
        TxBuffer[len+len2] = 0;
        if(TxCounter == 0) {
            USART_ITConfig(USART_Lua, USART_IT_TXE, ENABLE);
        }
    }
    return len;
}
int USART_Lua_SendChars_from_isr(const char ch,int len) {
	int len2;
	len2 = strlen(TxBuffer);
	
    if(len == 0) {
        return 0;
    }
    else if(len+len2 >= USART_Lua_TxBufferSize) {
        len = USART_Lua_TxBufferSize - len2 - 1;
    }
    if(State < USB_Lua_STATE) {
        memset(TxBuffer+len2,ch,len);
        TxBuffer[len+len2] = 0;
        if(TxCounter == 0) {
            USART_ITConfig(USART_Lua, USART_IT_TXE, ENABLE);
        }
    }
    return len;
}
int USART_Lua_Getchar(void) {
    int ch;
    if(RxState == 0) {
        return EOF;
    }
    ch = RxBuffer[RxCounter];
    if(ch == '\0') {
        RxState = 0;
        return EOF;
    }
    else{
        RxCounter++;
    }
    return ch;
}
int USART_Lua_UnGetchar(char c) {
    //int ch;
    if(RxState == 0) {
        return -1;
    }
    if(RxCounter > 0) {
        RxCounter--;
    }
    else {
        uint32_t len = strlen(&RxBuffer[RxCounter]);
        if(len >= USART_Lua_TxBufferSize) {
            return -2;
        }
        else {
            len++;
            while(len > 0) {
                RxBuffer[len + 1] = RxBuffer[len];
                len--;
            }
        }
    }
    RxBuffer[RxCounter] = c;
    return 0;
}
int USART_Lua_RecBufferEOF(void) {
    //int ch;
    if(RxState == 0) {
        return 1;
    }
    return 0;
}
int USART_Lua_RecTell(void) {
    
    if(RxState == 0) {
        return 0;
    }
    return RxCounter;
}
void USART_Lua_SetState(e_USART_Lua_STATE state) {
    if(state <= USB_Lua_STATE_TRANSFILE) {
        switch(state) {
            case USART_Lua_STATE:
                if(state > USB_Lua_STATE) {
                    state -= (USB_Lua_STATE - USART_Lua_STATE);
                }
            break;
            case USB_Lua_STATE:
                if(state < USB_Lua_STATE) {
                    state += (USB_Lua_STATE - USART_Lua_STATE);
                }
            break;
            default:
                State = state;
                RxCounter = 0;
            break;
        }
    }
}

int readline(char *indata,int maxlen,const char *msg) {
    int len;
    MaxReceive = maxlen;
    USART_Lua_SendS(msg,'\0');
    while(1) {
        switch(State) {
            case USB_Lua_STATE_NORMAL:
            case USART_Lua_STATE_NORMAL:
                if(RxState == 1) {
                    if(RxCounter > maxlen) {
                        len = RxCounter - maxlen;
                        RxCounter = maxlen;
                    }
                    else {
                        len = 0;
                    }
                    memcpy(indata,RxBuffer,RxCounter);
                    
                    if(len) {
                        for(int i=0;i<len;i++) {
                            RxBuffer[i] = RxBuffer[i+RxCounter];
                        }
                    }
                    RxState = 0;
                    RxCounter = len;
                    return 1;
                }
            break;
            case USB_Lua_STATE_TRANSFILE:
            case USART_Lua_STATE_TRANSFILE:
                if(RxCounter > 0) {
                    if(getTick() - UsartRxDataTick > 3000) {
                        RxState = 2;
                    }
                    if(RxState) {
                        if(RxState == 1) {
                            XmodemRecData(RxBuffer,RxCounter,USART_Lua_No_Error);
                        }
                        else if(RxState == 2) {
                            XmodemRecData(RxBuffer,RxCounter,USART_Lua_TimeOut_Error);
                        }
                        RxCounter = 0;
                    }
                }
            break;
        }
        if(TimeOutActionRun != NULL) {
            TimeOutActionList *curent = TimeOutActionRun;
            TimeOutActionRun=TimeOutActionRun->next;
            curent->fun(curent->parameter);
            free(curent);
        }
    }
    return 1;
}


/**
  * @brief  VCP_DataRx
  *         Data received over USB OUT endpoint are sent over CDC interface 
  *         through this function.
  *           
  *         @note
  *         This function will block any OUT packet reception on USB endpoint 
  *         until exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result 
  *         in receiving more data while previous ones are still not sent.
  *                 
  * @param  Buf: Buffer of data received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else VCP_FAIL
  */
uint16_t VCP_DataRx (uint32_t Len)
{
    uint32_t i;
    uint8_t temp;
    receive_count = Len;
    for (i = 0; i < Len; i++)
    {
        VCP_ReceiveData(&USB_OTG_dev, &temp, 1);
        if(RxCounter >= USART_Lua_RxBufferSize) {
            break;
        }
        switch(State) {
            case USART_Lua_STATE_NORMAL:
                State = USB_Lua_STATE_NORMAL;
                // no break;
            case USB_Lua_STATE_NORMAL:
                while (VCP_CheckDataSent()==1); 
                VCP_SendData(&USB_OTG_dev, &temp, 1);
                if(temp == '\n') {
                    RxState = 1;
                }
                RxCounter++;
                break;
            case USART_Lua_STATE_TRANSFILE:
                State = USB_Lua_STATE_TRANSFILE;
                // no break;
            case USB_Lua_STATE_TRANSFILE:
                RxBuffer[RxCounter] = temp;
                if(RxCounter == 0) {
                    if(temp == XMODE_SOH) {
                        MaxReceive = 133;
                    }
                    else {
                        MaxReceive = 1;
                    }
                }
                RxCounter++;
                break;
        }
        //Buf++;
    } 
    UsartRxDataTick = getTick();
 
    //if(USB_Lua_STATE_NORMAL)
    return USBD_OK;
}
int getTerminalType(int st) {
    if(st == 0) {
        if(State < USB_Lua_STATE) {//USART_Lua_STATE
            return USART_Lua_STATE;
        }
        else {
            return USB_Lua_STATE;
        }
    }
    return State;
}

void addRunList(TimeOutActionList *action) {
    action->next = NULL;
    if(TimeOutActionRun == NULL) {
        TimeOutActionRun = action;
    }
    else {
        TimeOutActionList *action1;
        action1 = TimeOutActionRun;
        while(action1->next != NULL) {
            action1 = action1->next;
        }
        action1->next = action;
    }
}