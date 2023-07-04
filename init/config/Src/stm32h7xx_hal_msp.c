/*******************************************************************************
*                                                                              *
* FILE:                                                                        *
*       stm32h7xx_hal_msp.c                                                    *
*                                                                              *
* DESCRIPTION:                                                                 *
*       Contains implementation of MSP initialization and de-initialization    *
*                routines                                                      *
*                                                                              *
*******************************************************************************/


/*------------------------------------------------------------------------------
 Includes                                                              
------------------------------------------------------------------------------*/
#include "main.h"
#include "zav_pin_defines_A0002.h"
#include "zav_error.h"


/*------------------------------------------------------------------------------
 Procedures 
------------------------------------------------------------------------------*/


/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   *
*       HAL_MspInit                                                            *
*                                                                              *
* DESCRIPTION:                                                                 *
*       Initializes the Global MSP                                             *
*                                                                              *
*******************************************************************************/
void HAL_MspInit
    (
    void
    )
{
__HAL_RCC_SYSCFG_CLK_ENABLE();
} /* HAL_MspInit */


/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   *
*       HAL_SPI_MspInit                                                        *
*                                                                              *
* DESCRIPTION:                                                                 *
*       Initializes the SPI MSP                                                *
*                                                                              *
*******************************************************************************/
void HAL_SPI_MspInit
    (
    SPI_HandleTypeDef* hspi
    )
{
GPIO_InitTypeDef         GPIO_InitStruct     = {0};
RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

/* Flash SPI */
if( hspi->Instance == FLASH_SPI )
    {
    /* Initializes the peripherals clock */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI2;
    PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;
    if ( HAL_RCCEx_PeriphCLKConfig( &PeriphClkInitStruct ) != HAL_OK )
        {
        Error_Handler( ERROR_SPI_HAL_MSP_ERROR );
        }

    /* Peripheral clock enable */
    __HAL_RCC_SPI2_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* SPI2 GPIO Configuration
    PB13     ------> SPI2_SCK
    PB14     ------> SPI2_MISO
    PB15     ------> SPI2_MOSI */
    GPIO_InitStruct.Pin       = FLASH_SCK_PIN  |
                                FLASH_MISO_PIN |
                                FLASH_MOSI_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init( FLASH_SCK_GPIO_PORT, &GPIO_InitStruct );
    }

} /* HAL_SPI_MspInit */


/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   *
*       HAL_SPI_MspDeInit                                                      *
*                                                                              *
* DESCRIPTION:                                                                 *
*       Deinitializes the SPI MSP                                              *
*                                                                              *
*******************************************************************************/
void HAL_SPI_MspDeInit
    (
    SPI_HandleTypeDef* hspi
    )
{

/* Flash SPI */
if( hspi->Instance == FLASH_SPI )
    {
    /* Peripheral clock disable */
    __HAL_RCC_SPI2_CLK_DISABLE();

    /* SPI2 GPIO Configuration
    PB13     ------> SPI2_SCK
    PB14     ------> SPI2_MISO
    PB15     ------> SPI2_MOSI */
    HAL_GPIO_DeInit( FLASH_SCK_GPIO_PORT, 
                     FLASH_SCK_PIN  |
                     FLASH_MISO_PIN |
                     FLASH_MOSI_PIN );
    }

} /* HAL_SPI_MspDeInit */


/*******************************************************************************
* END OF FILE                                                                  *
*******************************************************************************/