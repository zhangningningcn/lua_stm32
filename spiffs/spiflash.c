
#ifdef STM32F4XX
#include "stm32f4xx.h"
#else
#include "stm32f10x.h"
#endif

#include "user_define.h"
#include "spiportdef.h"
#include "spiffs.h"
#include "spiffs_nucleus.h"

#define READ                0x03    // SPI Flash opcode: Read up up to 25MHz
#define READ_FAST           0x0B    // SPI Flash opcode: Read up to 50MHz with 1 dummy byte
#define ERASE_4K            0x20    // SPI Flash opcode: 4KByte sector erase
#define ERASE_32K           0x52    // SPI Flash opcode: 32KByte block erase
#define ERASE_SECTOR        0xD8    // SPI Flash opcode: 64KByte block erase
#define ERASE_ALL           0x60    // SPI Flash opcode: Entire chip erase
#define WRITE               0x02    // SPI Flash opcode: Write one byte (or a page of up to 256 bytes, depending on device)
#define WRITE_WORD_STREAM   0xAD    // SPI Flash opcode: Write continuous stream of 16-bit words (AAI mode); available on SST25VF016B (but not on SST25VF010A)
#define WRITE_BYTE_STREAM   0xAF    // SPI Flash opcode: Write continuous stream of bytes (AAI mode); available on SST25VF010A (but not on SST25VF016B)
#define RDSR                0x05    // SPI Flash opcode: Read Status Register
#define EWSR                0x50    // SPI Flash opcode: Enable Write Status Register
#define WRSR                0x01    // SPI Flash opcode: Write Status Register
#define WREN                0x06    // SPI Flash opcode: Write Enable
#define WRDI                0x04    // SPI Flash opcode: Write Disable / End AAI mode
#define RDID                0x90    // SPI Flash opcode: Read ID
#define JEDEC_ID            0x9F    // SPI Flash opcode: Read JEDEC ID
#define EBSY                0x70    // SPI Flash opcode: Enable write BUSY status on SO pin
#define DBSY                0x80    // SPI Flash opcode: Disable write BUSY status on SO pin
#define DUMMY_BYTE          0xA5


#define BUSY    0x01    // Mask for Status Register BUSY (Internal Write Operaiton in Progress status) bit
#define WEL     0x02    // Mask for Status Register WEL (Write Enable status) bit
#define BP0     0x04    // Mask for Status Register BP0 (Block Protect 0) bit
#define BP1     0x08    // Mask for Status Register BP1 (Block Protect 1) bit
#define BP2     0x10    // Mask for Status Register BP2 (Block Protect 2) bit
#define BP3     0x20    // Mask for Status Register BP3 (Block Protect 3) bit
#define AAI     0x40    // Mask for Status Register AAI (Auto Address Increment Programming status) bit
#define BPL     0x80    // Mask for Status Register BPL (BPx block protect bit read-only protect) bit

#define LOG_PAGE_SIZE   64
u8_t _work[LOG_PAGE_SIZE*2];
u8_t _fds[128];
#if SPIFFS_CACHE
#define SPIFFS_CACHE_SIZE  512
u8_t _cache[SPIFFS_CACHE_SIZE];
#else
#define SPIFFS_CACHE_SIZE  0
#define _cache NULL
#endif

// static s32_t fs_mount_specific(u32_t phys_addr, u32_t phys_size,
    // u32_t phys_sector_size,
    // u32_t log_block_size, u32_t log_page_size) ;
static void spiffs_check_cb_f(spiffs *fs, spiffs_check_type type, spiffs_check_report report,
    u32_t arg1, u32_t arg2);
static void sFLASH_LowLevel_Init(void);
static uint8_t sFLASH_SendByte(uint8_t byte);
static void sFLASH_WaitForWriteEnd(void);
//static s32_t _erase(spiffs *fs, u32_t addr, u32_t size) 
static s32_t spi_flasherase(spiffs *fs, u32_t addr, u32_t size);
static s32_t spi_flashread(spiffs *fs, u32_t addr, u32_t size, u8_t *dst);
static s32_t spi_flashwrite(spiffs *fs, u32_t addr, u32_t size, u8_t *src);

static void SFLASH_SPI_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;

  sFLASH_LowLevel_Init();
    
  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();

  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL)
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
#else
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
#endif

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(sFLASH_SPI, &SPI_InitStructure);

  /*!< Enable the sFLASH_SPI  */
  SPI_Cmd(sFLASH_SPI, ENABLE);
}

void SPIFlashMount(spiffs *fs) {
    spiffs_config c;
// static s32_t fs_mount_specific(u32_t phys_addr, u32_t phys_size,
    // u32_t phys_sector_size,
    // u32_t log_block_size, u32_t log_page_size) ;
    c.hal_erase_f = spi_flasherase;
    c.hal_read_f = spi_flashread;
    c.hal_write_f = spi_flashwrite;
    c.log_block_size = 16*1024;
    c.log_page_size = 64;
    c.phys_addr = 0;
    c.phys_erase_block = SPI_FLASH_SECTOR_SIZE;
    c.phys_size = SPI_FLASH_SIZE;
#if SPIFFS_FILEHDL_OFFSET
    c.fh_ix_offset = SPIFFS_FILEHDL_OFFSET;
#endif
    memset(_work,0,sizeof(_work));
    
    SPIFFS_mount(fs, &c, _work, _fds, sizeof(_fds), _cache, sizeof(_cache), spiffs_check_cb_f);
	
}
void SPIFlashInit(spiffs *fs) {
    uint8_t flashid[2];
    SFLASH_SPI_Init();
    
    sFLASH_CS_LOW();
    sFLASH_SendByte(RDID);//DUMMY_BYTE
    for(int i=0;i<3;i++) {
        sFLASH_SendByte(DUMMY_BYTE);
    }
    for(int i=0;i<2;i++) {
        flashid[i] = sFLASH_SendByte(DUMMY_BYTE);
    }
    if(flashid[0] == 0xEF) { //winbond
        switch(flashid[1]) {
            case 0x12: // w25Q40BL(id7~id0:0x12;id15~id8:0x3013)
            break;
            case 0x13: // w25Q80BL(id7~id0:0x13;id15~id8:0x4041)
            break;
            case 0x15: // w25Q32BV(id7~id0:0x15;id15~id8:0x4016)
            break;
        }
    }
    /*!< Send "Write Enable" instruction */
    
    sFLASH_CS_HIGH();
    
    SPIFlashMount(fs);
    //fs_mount_specific(0, SPI_FLASH_SIZE,
    //SPI_FLASH_SECTOR_SIZE, 16*1024, 64);
}

static void sFLASH_WriteEnable(void) {
    
    sFLASH_CS_LOW();
    
    sFLASH_SendByte(WREN);
    
    sFLASH_CS_HIGH();
    
    
}

static s32_t spi_flashread(struct spiffs_t *fs, u32_t addr, u32_t size, u8_t *dst) {
//    int t_size = size;
    /*!< Select the FLASH: Chip Select low */
    sFLASH_CS_LOW();

    /*!< Send "Read from Memory " instruction */
    sFLASH_SendByte(READ);

    /*!< Send the 24-bit address of the address to read from -------------------*/
    /*!< Send ReadAddr high nibble address byte */
    sFLASH_SendByte((addr & 0xFF0000) >> 16);
    /*!< Send ReadAddr medium nibble address byte */
    sFLASH_SendByte((addr& 0xFF00) >> 8);
    /*!< Send ReadAddr low nibble address byte */
    sFLASH_SendByte(addr & 0xFF);

    while(size > 0) {
        *dst = sFLASH_SendByte(DUMMY_BYTE);
        dst++;
        size--;
    }
    sFLASH_CS_HIGH();
    
	return 0;
}
static s32_t spi_flashwrite(struct spiffs_t *fs, u32_t addr, u32_t size, u8_t *src) {
  /*!< Enable the write access to the FLASH */
  sFLASH_WriteEnable();
  //int t_size = size;
  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();
  /*!< Send "Write to Memory " instruction */
  sFLASH_SendByte(WRITE);
  /*!< Send WriteAddr high nibble address byte to write to */
  sFLASH_SendByte((addr & 0xFF0000) >> 16);
  /*!< Send WriteAddr medium nibble address byte to write to */
  sFLASH_SendByte((addr & 0xFF00) >> 8);
  /*!< Send WriteAddr low nibble address byte to write to */
  sFLASH_SendByte(addr & 0xFF);

  /*!< while there is data to be written on the FLASH */
  while (size > 0)
  {
    /*!< Send the current byte */
    sFLASH_SendByte(*src);
    /*!< Point on the next byte to be written */
    src++;
    size--;
  }

  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();

  /*!< Wait the end of Flash writing */
  sFLASH_WaitForWriteEnd();
	return 0;
}
static s32_t spi_flasherase(spiffs *fs, u32_t addr, u32_t size) {
    /*!< Send write enable instruction */
    sFLASH_WriteEnable();
    
    /*!< Sector Erase */
    /*!< Select the FLASH: Chip Select low */
    sFLASH_CS_LOW();
    /*!< Send Sector Erase instruction */
    switch(size) {
        case 32768:
            sFLASH_SendByte(ERASE_32K);
            size = 32768;
            break;
        case 65536:
            sFLASH_SendByte(ERASE_SECTOR);
            size = 65536;
            break;
        case 4096:
            sFLASH_SendByte(ERASE_4K);
            size = 4096;
            break;
        default:
            return -1;
    }
    /*!< Send SectorAddr high nibble address byte */
    sFLASH_SendByte((addr & 0xFF0000) >> 16);
    /*!< Send SectorAddr medium nibble address byte */
    sFLASH_SendByte((addr & 0xFF00) >> 8);
    /*!< Send SectorAddr low nibble address byte */
    sFLASH_SendByte(addr & 0xFF);
    /*!< Deselect the FLASH: Chip Select high */
    sFLASH_CS_HIGH();
    
    /*!< Wait the end of Flash writing */
    sFLASH_WaitForWriteEnd();
	return 0;
}
static uint8_t sFLASH_SendByte(uint8_t byte) {
    
    /*!< Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_TXE) == RESET);

    /*!< Send byte through the SPI1 peripheral */
    SPI_I2S_SendData(sFLASH_SPI, byte);

    /*!< Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET);

    /*!< Return the byte read from the SPI bus */
    return SPI_I2S_ReceiveData(sFLASH_SPI);
}
/**
  * @brief  Polls the status of the Write In Progress (WIP) flag in the FLASH's
  *         status register and loop until write opertaion has completed.
  * @param  None
  * @retval None
  */
static void sFLASH_WaitForWriteEnd(void)
{
    uint8_t flashstatus = 0;
   
    /*!< Select the FLASH: Chip Select low */
    sFLASH_CS_LOW();
   
    /*!< Send "Read Status Register" instruction */
    sFLASH_SendByte(RDSR);
   
    /*!< Loop as long as the memory is busy with a write cycle */
    do
    {
        /*!< Send a dummy byte to generate the clock needed by the FLASH
        and put the value of the status register in FLASH_Status variable */
        flashstatus = sFLASH_SendByte(DUMMY_BYTE);
       
    }
    while (flashstatus & BUSY); /* Write in progress */
   
    /*!< Deselect the FLASH: Chip Select high */
    sFLASH_CS_HIGH();
}


static void spiffs_check_cb_f(spiffs *fs, spiffs_check_type type, spiffs_check_report report,
    u32_t arg1, u32_t arg2) {
/*  if (report == SPIFFS_CHECK_PROGRESS && old_perc != arg1) {
    old_perc = arg1;
    printf("CHECK REPORT: ");
    switch(type) {
    case SPIFFS_CHECK_LOOKUP:
      printf("LU "); break;
    case SPIFFS_CHECK_INDEX:
      printf("IX "); break;
    case SPIFFS_CHECK_PAGE:
      printf("PA "); break;
    }
    printf("%i%%\n", arg1 * 100 / 256);
  }*/
  //if (report != SPIFFS_CHECK_PROGRESS) {
  //  if (report != SPIFFS_CHECK_ERROR) fs_check_fixes++;
  //  printf("   check: ");
  //  switch (type) {
  //  case SPIFFS_CHECK_INDEX:
  //    printf("INDEX  "); break;
  //  case SPIFFS_CHECK_LOOKUP:
  //    printf("LOOKUP "); break;
  //  case SPIFFS_CHECK_PAGE:
  //    printf("PAGE   "); break;
  //  default:
  //    printf("????   "); break;
  //  }
  //  if (report == SPIFFS_CHECK_ERROR) {
  //    printf("ERROR %i", arg1);
  //  } else if (report == SPIFFS_CHECK_DELETE_BAD_FILE) {
  //    printf("DELETE BAD FILE %04x", arg1);
  //  } else if (report == SPIFFS_CHECK_DELETE_ORPHANED_INDEX) {
  //    printf("DELETE ORPHANED INDEX %04x", arg1);
  //  } else if (report == SPIFFS_CHECK_DELETE_PAGE) {
  //    printf("DELETE PAGE %04x", arg1);
  //  } else if (report == SPIFFS_CHECK_FIX_INDEX) {
  //    printf("FIX INDEX %04x:%04x", arg1, arg2);
  //  } else if (report == SPIFFS_CHECK_FIX_LOOKUP) {
  //    printf("FIX INDEX %04x:%04x", arg1, arg2);
  //  } else {
  //    printf("??");
  //  }
  //  printf("\n");
  //}
}
#if 0
static s32_t fs_mount_specific(u32_t phys_addr, u32_t phys_size,
    u32_t phys_sector_size,
    u32_t log_block_size, u32_t log_page_size) {
    spiffs_config c;
    c.hal_erase_f = spi_flasherase;
    c.hal_read_f = spi_flashread;
    c.hal_write_f = spi_flashwrite;
    c.log_block_size = log_block_size;
    c.log_page_size = log_page_size;
    c.phys_addr = phys_addr;
    c.phys_erase_block = phys_sector_size;
    c.phys_size = phys_size;
#if SPIFFS_FILEHDL_OFFSET
    c.fh_ix_offset = SPIFFS_FILEHDL_OFFSET;
#endif
    memset(_work,0,sizeof(_work));
    return SPIFFS_mount(&__fs, &c, _work, _fds, sizeof(_fds), _cache, sizeof(_cache), spiffs_check_cb_f);
}
#endif
static void sFLASH_LowLevel_Init(void) {
	
  GPIO_InitTypeDef GPIO_InitStructure;
#ifdef STM32F4XX

  /*!< Enable the SPI clock */
  sFLASH_SPI_CLK_INIT(sFLASH_SPI_CLK, ENABLE);

  /*!< Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(sFLASH_SPI_SCK_GPIO_CLK | sFLASH_SPI_MISO_GPIO_CLK | 
                         sFLASH_SPI_MOSI_GPIO_CLK | sFLASH_CS_GPIO_CLK, ENABLE);
  
  /*!< SPI pins configuration *************************************************/

  /*!< Connect SPI pins to AF5 */  
  GPIO_PinAFConfig(sFLASH_SPI_SCK_GPIO_PORT, sFLASH_SPI_SCK_SOURCE, sFLASH_SPI_SCK_AF);
  GPIO_PinAFConfig(sFLASH_SPI_MISO_GPIO_PORT, sFLASH_SPI_MISO_SOURCE, sFLASH_SPI_MISO_AF);
  GPIO_PinAFConfig(sFLASH_SPI_MOSI_GPIO_PORT, sFLASH_SPI_MOSI_SOURCE, sFLASH_SPI_MOSI_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
        
  /*!< SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_SCK_PIN;
  GPIO_Init(sFLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< SPI MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  sFLASH_SPI_MOSI_PIN;
  GPIO_Init(sFLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< SPI MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin =  sFLASH_SPI_MISO_PIN;
  GPIO_Init(sFLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure sFLASH Card CS pin in output pushpull mode ********************/
  GPIO_InitStructure.GPIO_Pin = sFLASH_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(sFLASH_CS_GPIO_PORT, &GPIO_InitStructure);
#else
  /*!< sFLASH_SPI_CS_GPIO, sFLASH_SPI_MOSI_GPIO, sFLASH_SPI_MISO_GPIO 
       and sFLASH_SPI_SCK_GPIO Periph clock enable */
  RCC_APB2PeriphClockCmd(sFLASH_CS_GPIO_CLK | sFLASH_SPI_MOSI_GPIO_CLK | sFLASH_SPI_MISO_GPIO_CLK |
                         sFLASH_SPI_SCK_GPIO_CLK, ENABLE);

  /*!< sFLASH_SPI Periph clock enable */
  RCC_APB2PeriphClockCmd(sFLASH_SPI_CLK, ENABLE);
  
  /*!< Configure sFLASH_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(sFLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure sFLASH_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_MOSI_PIN;
  GPIO_Init(sFLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure sFLASH_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
  GPIO_Init(sFLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
  
  /*!< Configure sFLASH_CS_PIN pin: sFLASH Card CS pin */
  GPIO_InitStructure.GPIO_Pin = sFLASH_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(sFLASH_CS_GPIO_PORT, &GPIO_InitStructure);
  #endif
}
