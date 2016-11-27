#ifndef _SPI_PORT_DEF_H
#define _SPI_PORT_DEF_H

#ifdef STM32F4XX
/* M25P FLASH SPI Interface pins  */  
#define sFLASH_SPI                           SPI1
#define sFLASH_SPI_CLK                       RCC_APB2Periph_SPI1
#define sFLASH_SPI_CLK_INIT                  RCC_APB2PeriphClockCmd

#define sFLASH_SPI_SCK_PIN                   GPIO_Pin_3
#define sFLASH_SPI_SCK_GPIO_PORT             GPIOB
#define sFLASH_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_SCK_SOURCE                GPIO_PinSource3
#define sFLASH_SPI_SCK_AF                    GPIO_AF_SPI1

#define sFLASH_SPI_MISO_PIN                  GPIO_Pin_4
#define sFLASH_SPI_MISO_GPIO_PORT            GPIOB
#define sFLASH_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_MISO_SOURCE               GPIO_PinSource4
#define sFLASH_SPI_MISO_AF                   GPIO_AF_SPI1

#define sFLASH_SPI_MOSI_PIN                  GPIO_Pin_5
#define sFLASH_SPI_MOSI_GPIO_PORT            GPIOB
#define sFLASH_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_MOSI_SOURCE               GPIO_PinSource5
#define sFLASH_SPI_MOSI_AF                   GPIO_AF_SPI1

#define sFLASH_CS_PIN                        GPIO_Pin_0
#define sFLASH_CS_GPIO_PORT                  GPIOB
#define sFLASH_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOB

#define  sFLASH_CS_HIGH() GPIO_SetBits(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN)
#define  sFLASH_CS_LOW()  GPIO_ResetBits(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN)
#else
	
#define sFLASH_SPI                       SPI1
#define sFLASH_SPI_CLK                   RCC_APB2Periph_SPI1
#define sFLASH_SPI_SCK_PIN               GPIO_Pin_5                  /* PA.05 */
#define sFLASH_SPI_SCK_GPIO_PORT         GPIOA                       /* GPIOA */
#define sFLASH_SPI_SCK_GPIO_CLK          RCC_APB2Periph_GPIOA
#define sFLASH_SPI_MISO_PIN              GPIO_Pin_6                  /* PA.06 */
#define sFLASH_SPI_MISO_GPIO_PORT        GPIOA                       /* GPIOA */
#define sFLASH_SPI_MISO_GPIO_CLK         RCC_APB2Periph_GPIOA
#define sFLASH_SPI_MOSI_PIN              GPIO_Pin_7                  /* PA.07 */
#define sFLASH_SPI_MOSI_GPIO_PORT        GPIOA                       /* GPIOA */
#define sFLASH_SPI_MOSI_GPIO_CLK         RCC_APB2Periph_GPIOA
#define sFLASH_CS_PIN                    GPIO_Pin_9                  /* PD.09 */
#define sFLASH_CS_GPIO_PORT              GPIOD                       /* GPIOD */
#define sFLASH_CS_GPIO_CLK               RCC_APB2Periph_GPIOD


#define  sFLASH_CS_HIGH() GPIO_ResetBits(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN)
#define  sFLASH_CS_LOW()  GPIO_SetBits(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN)

#endif //STM32F4XX
#endif //_SPI_PORT_DEF_H
