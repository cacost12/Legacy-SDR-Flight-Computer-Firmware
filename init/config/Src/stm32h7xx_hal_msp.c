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
*                                                                              *
* PROCEDURE:                                                                   *
*       HAL_UART_MspInit                                                       *
*                                                                              *
* DESCRIPTION:                                                                 *
*       Initializes the UART MSP                                               *
*                                                                              *
*******************************************************************************/
void HAL_UART_MspInit
    (
    UART_HandleTypeDef* huart
    )
{
GPIO_InitTypeDef         GPIO_InitStruct     = {0};
RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

/* USB UART */
if( huart->Instance == USB_UART )
    {
    /* Initializes the peripherals clock */
    PeriphClkInitStruct.PeriphClockSelection  = RCC_PERIPHCLK_USART6;
    PeriphClkInitStruct.PLL2.PLL2M            = 2;
    PeriphClkInitStruct.PLL2.PLL2N            = 16;
    PeriphClkInitStruct.PLL2.PLL2P            = 2;
    PeriphClkInitStruct.PLL2.PLL2Q            = 4;
    PeriphClkInitStruct.PLL2.PLL2R            = 2;
    PeriphClkInitStruct.PLL2.PLL2RGE          = RCC_PLL2VCIRANGE_3;
    PeriphClkInitStruct.PLL2.PLL2VCOSEL       = RCC_PLL2VCOWIDE;
    PeriphClkInitStruct.PLL2.PLL2FRACN        = 0;
    PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_PLL2;
    if ( HAL_RCCEx_PeriphCLKConfig( &PeriphClkInitStruct ) != HAL_OK )
        {
        Error_Handler( ERROR_UART_HAL_MSP_ERROR );
        }

    /* Peripheral clock enable */
    __HAL_RCC_USART6_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* USART6 GPIO Configuration
    PC6     ------> USART6_TX
    PC7     ------> USART6_RX */
    GPIO_InitStruct.Pin       = USB_RX_PIN | USB_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART6;
    HAL_GPIO_Init( USB_RX_GPIO_PORT, &GPIO_InitStruct );
    }

} /* HAL_UART_MspInit */


/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   *
*       HAL_UART_MspDeInit                                                     *
*                                                                              *
* DESCRIPTION:                                                                 *
*       Deinitializes the UART MSP                                             *
*                                                                              *
*******************************************************************************/
void HAL_UART_MspDeInit
    (
    UART_HandleTypeDef* huart
    )
{

/* USB UART */
if( huart->Instance == USB_UART )
    {
    /* Peripheral clock disable */
    __HAL_RCC_USART6_CLK_DISABLE();

    /* USART6 GPIO Configuration
    PC6     ------> USART6_TX
    PC7     ------> USART6_RX */
    HAL_GPIO_DeInit(USB_RX_GPIO_PORT, USB_RX_PIN | USB_TX_PIN );
    }

} /* HAL_UART_MspDeInit */


/*******************************************************************************
* END OF FILE                                                                  *
*******************************************************************************/