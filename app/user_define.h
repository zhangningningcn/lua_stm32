#ifndef _USER_DEFINE_H
#define _USER_DEFINE_H

#define SPI_FLASH_SIZE         0x200000  //8M
#define SPI_FLASH_SECTOR_SIZE  4096  //4K


/* Definition for USARTx resources ********************************************/
#define USART_Lua_BaudRate              115200

#ifdef STM32F4XX

#define USART_Lua                           USART1
#define USART_Lua_CLK                       RCC_APB2Periph_USART1
#define USART_Lua_CLK_INIT                  RCC_APB2PeriphClockCmd
#define USART_Lua_IRQn                      USART1_IRQn
#define USART_Lua_IRQHandler                USART1_IRQHandler

#define USART_Lua_TX_PIN                    GPIO_Pin_9                
#define USART_Lua_TX_GPIO_PORT              GPIOA                       
#define USART_Lua_TX_GPIO_CLK               RCC_AHB1Periph_GPIOA
#define USART_Lua_TX_SOURCE                 GPIO_PinSource9
#define USART_Lua_TX_AF                     GPIO_AF_USART1

#define USART_Lua_RX_PIN                    GPIO_Pin_10                
#define USART_Lua_RX_GPIO_PORT              GPIOA                    
#define USART_Lua_RX_GPIO_CLK               RCC_AHB1Periph_GPIOA
#define USART_Lua_RX_SOURCE                 GPIO_PinSource10
#define USART_Lua_RX_AF                     GPIO_AF_USART1

#else

#define USART_Lua                   USART1
#define USART_Lua_GPIO              GPIOA
#define USART_Lua_CLK               RCC_APB2Periph_USART1
#define USART_Lua_GPIO_CLK          RCC_APB2Periph_GPIOA
#define USART_Lua_RxPin             GPIO_Pin_10
#define USART_Lua_TxPin             GPIO_Pin_9
#define USART_Lua_IRQn              USART1_IRQn
#define USART_Lua_IRQHandler        USART1_IRQHandler

#endif




#define USART_Lua_RxBufferSize        160
#define USART_Lua_TxBufferSize        160

#define DEFAULT_ADC_VREF_VALUE        3.3

#endif
