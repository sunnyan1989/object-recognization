/****************************************Copyright (c)**************************************************                         
**
**                                 http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			SST25VF016B.h
** Descriptions:		SST25VF016B头函数 
**
**------------------------------------------------------------------------------------------------------
** Created by:			AVRman
** Created date:		2011-1-26
** Version:				1.0
** Descriptions:		The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:			
** Modified date:	
** Version:
** Descriptions:		
********************************************************************************************************/

#ifndef __SST25VF016B_H 
#define __SST25VF016B_H

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"

/* Private typedef -----------------------------------------------------------*/
/* 定义操作SST25VF016B所需的数据 */
typedef enum ERTYPE{Sec1,Sec8,Sec16,Chip} ErType;  
typedef enum IDTYPE{Manu_ID,Dev_ID,Jedec_ID} idtype;

/* Private define ------------------------------------------------------------*/
#define MAX_ADDR		0x1FFFFF	/* 定义芯片内部最大地址 */
#define	SEC_MAX     	511         /* 定义最大扇区号 */
#define SEC_SIZE		0x1000      /* 扇区大小	*/

/* 宏,用于定义SST25VF016B的片选引脚 */
#define SPI_FLASH_CS_LOW()       LPC_GPIO0->FIOPIN &= ~(1 << 16 )
#define SPI_FLASH_CS_HIGH()      LPC_GPIO0->FIOPIN |= (1 << 16 )

/* bit definitions for register SSPSR. */
#define SSPSR_RNE       2
#define SSPSR_BSY       4

/* bit-frequency = PCLK / CPSR, PCLK=72MHz, must be even	*/
#define SPI_SPEED_12MHz		6	/* => 12MHz */
#define SPI_SPEED_18MHz		4	/* => 18MHz */
#define SPI_SPEED_400kHz  180	/* => 400kHz */

#define ERROR				1
#define ENABLE      		0

/* Private function prototypes -----------------------------------------------*/
void  SPI_FLASH_Init(void);
uint8_t SSTF016B_RD(uint32_t Dst, uint8_t* RcvBufPt ,uint32_t NByte);
uint8_t SSTF016B_RdID(idtype IDType,uint32_t* RcvbufPt);
uint8_t SSTF016B_WR(uint32_t Dst, uint8_t* SndbufPt,uint32_t NByte);

uint8_t SSTF016B_Erase(uint32_t sec1, uint32_t sec2);
void SPI_FLASH_Test(void);

#endif
/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

