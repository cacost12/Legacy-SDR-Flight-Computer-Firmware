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
#include "zav_pin_defines_A0003.h"
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
*       HAL_I2C_MspInit                                                        *
*                                                                              *
* DESCRIPTION:                                                                 *
*       Initializes the I2C MSP                                                *
*                                                                              *
*******************************************************************************/
void HAL_I2C_MspInit
    (
    I2C_HandleTypeDef* hi2c
    )
{
GPIO_InitTypeDef         GPIO_InitStruct     = {0};

/* Barometric Pressure Sensor I2C */
if( hi2c->Instance == BARO_I2C )
    {
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA */
    GPIO_InitStruct.Pin       = BARO_SCL_PIN | BARO_SDA_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init( BARO_SCL_GPIO_PORT, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();
    }

/* IMU I2C */
else if( hi2c->Instance == IMU_I2C )
    {
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* I2C2 GPIO Configuration
    PB10     ------> I2C2_SCL
    PB11     ------> I2C2_SDA */
    GPIO_InitStruct.Pin       = IMU_SCL_PIN | IMU_SDA_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init( IMU_SCL_GPIO_PORT, &GPIO_InitStruct );

    /* Peripheral clock enable */
    __HAL_RCC_I2C2_CLK_ENABLE();
    }

} /* HAL_I2C_MspInit */


/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   *
*       HAL_I2C_MspDeInit                                                      *
*                                                                              *
* DESCRIPTION:                                                                 *
*       Deinitializes the I2C MSP                                              *
*                                                                              *
*******************************************************************************/
void HAL_I2C_MspDeInit
    (
    I2C_HandleTypeDef* hi2c
    )
{

/* Barometric pressure sensor I2C */
if( hi2c->Instance == BARO_I2C )
    {
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /* I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA */
    HAL_GPIO_DeInit( BARO_SCL_GPIO_PORT, BARO_SCL_PIN );
    HAL_GPIO_DeInit( BARO_SDA_GPIO_PORT, BARO_SDA_PIN );
    }

/* IMU I2C */
else if( hi2c->Instance == IMU_I2C )
    {
    /* Peripheral clock disable */
    __HAL_RCC_I2C2_CLK_DISABLE();

    /* I2C2 GPIO Configuration
    PB10     ------> I2C2_SCL
    PB11     ------> I2C2_SDA */
    HAL_GPIO_DeInit( IMU_SCL_GPIO_PORT, IMU_SCL_PIN );
    HAL_GPIO_DeInit( IMU_SDA_GPIO_PORT, IMU_SDA_PIN );
    }

} /* HAL_I2C_MspDeInit */


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

/* Flash SPI */
if( hspi->Instance == FLASH_SPI )
    {
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

/* USB UART */
if( huart->Instance == USB_UART )
    {
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