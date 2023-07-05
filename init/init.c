/*******************************************************************************
*                                                                              *
* FILE:                                                                        *
* 		init.c                                                                 *
*                                                                              *
* DESCRIPTION:                                                                 *
* 		Contains initialization routines for MCU core and peripherals          *
*                                                                              *
*******************************************************************************/


/*------------------------------------------------------------------------------
 Standard Includes                                                              
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 Project Includes                                                               
------------------------------------------------------------------------------*/
#include "main.h"
#include "init.h"
#include "zav_pin_defines_A0002.h"
#include "zav_error.h"


/*------------------------------------------------------------------------------
 Global Variables 
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 Procedures 
------------------------------------------------------------------------------*/


/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   *
* 		SystemClock_Config                                                     *
*                                                                              *
* DESCRIPTION:                                                                 *
* 		Initializes the microcontroller clock. Enables peripheral clocks and   *
*       sets prescalers                                                        *
*                                                                              *
*******************************************************************************/
void SystemClock_Config
	(
	void
	)
{
RCC_OscInitTypeDef RCC_OscInitStruct = {0};
RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

/* Supply configuration update enable */
HAL_PWREx_ConfigSupply( PWR_LDO_SUPPLY );

/* Configure the main internal regulator output voltage */
__HAL_PWR_VOLTAGESCALING_CONFIG( PWR_REGULATOR_VOLTAGE_SCALE3 );
while( !__HAL_PWR_GET_FLAG( PWR_FLAG_VOSRDY ) ) 
	{
	}

/* Macro to configure the PLL clock source */
__HAL_RCC_PLL_PLLSOURCE_CONFIG( RCC_PLLSOURCE_HSE );

/* Initializes the RCC Oscillators according to the specified parameters
  in the RCC_OscInitTypeDef structure */
RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
RCC_OscInitStruct.PLL.PLLM       = 2;
RCC_OscInitStruct.PLL.PLLN       = 16;
RCC_OscInitStruct.PLL.PLLP       = 2;
RCC_OscInitStruct.PLL.PLLQ       = 2;
RCC_OscInitStruct.PLL.PLLR       = 2;
RCC_OscInitStruct.PLL.PLLRGE     = RCC_PLL1VCIRANGE_3;
RCC_OscInitStruct.PLL.PLLVCOSEL  = RCC_PLL1VCOWIDE;
RCC_OscInitStruct.PLL.PLLFRACN   = 0;
if ( HAL_RCC_OscConfig( &RCC_OscInitStruct ) != HAL_OK )
	{
	Error_Handler( ERROR_SYSCLOCK_CONFIG_ERROR );
	}

/* Initializes the CPU, AHB and APB buses clocks */
RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK    | RCC_CLOCKTYPE_SYSCLK
							     | RCC_CLOCKTYPE_PCLK1   | RCC_CLOCKTYPE_PCLK2
							     | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
RCC_ClkInitStruct.SYSCLKDivider  = RCC_SYSCLK_DIV1;
RCC_ClkInitStruct.AHBCLKDivider  = RCC_HCLK_DIV1;
RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

if ( HAL_RCC_ClockConfig( &RCC_ClkInitStruct, FLASH_LATENCY_2 ) != HAL_OK )
	{
	Error_Handler( ERROR_SYSCLOCK_CONFIG_ERROR );
	}

} /* SystemClock_Config */


#ifndef BLINK
/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   *
* 		PeriphCommonClock_Config                                               *
*                                                                              *
* DESCRIPTION:                                                                 *
* 		Initializes microcontroller clocks shared amongst several peripherals  *
*                                                                              *
*******************************************************************************/
void PeriphCommonClock_Config
	(
	void
	)
{
RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

/* Initializes the peripherals clock */
PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C2 | RCC_PERIPHCLK_I2C1;
PeriphClkInitStruct.PLL3.PLL3M           = 2;
PeriphClkInitStruct.PLL3.PLL3N           = 16;
PeriphClkInitStruct.PLL3.PLL3P           = 2;
PeriphClkInitStruct.PLL3.PLL3Q           = 2;
PeriphClkInitStruct.PLL3.PLL3R           = 4;
PeriphClkInitStruct.PLL3.PLL3RGE         = RCC_PLL3VCIRANGE_3;
PeriphClkInitStruct.PLL3.PLL3VCOSEL      = RCC_PLL3VCOWIDE;
PeriphClkInitStruct.PLL3.PLL3FRACN       = 0;
PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_PLL3;
if ( HAL_RCCEx_PeriphCLKConfig( &PeriphClkInitStruct ) != HAL_OK )
	{
	Error_Handler( ERROR_COMMON_CLOCK_CONFIG_ERROR );
	}
} /* PeriphCommonClock_Config */


/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   *
* 		Baro_I2C_Init                                                          *
*                                                                              *
* DESCRIPTION:                                                                 *
* 		Initializes the barometric pressure sensor's I2C MCU interface         *
*                                                                              *
*******************************************************************************/
void Baro_I2C_Init
	(
	void
	)
{
baro_hi2c.Instance              = BARO_I2C;
baro_hi2c.Init.Timing           = 0x20303E5D;
baro_hi2c.Init.OwnAddress1      = 0;
baro_hi2c.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
baro_hi2c.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
baro_hi2c.Init.OwnAddress2      = 0;
baro_hi2c.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
baro_hi2c.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
baro_hi2c.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;

if ( HAL_I2C_Init( &baro_hi2c ) != HAL_OK )
	{
	Error_Handler( ERROR_BARO_I2C_INIT_ERROR );
	}

/* Configure Analogue filter */
if ( HAL_I2CEx_ConfigAnalogFilter( &baro_hi2c, I2C_ANALOGFILTER_ENABLE ) != HAL_OK )
	{
	Error_Handler( ERROR_BARO_I2C_INIT_ERROR );
	}

/* Configure Digital filter */
if ( HAL_I2CEx_ConfigDigitalFilter( &baro_hi2c, 0 ) != HAL_OK )
	{
	Error_Handler( ERROR_BARO_I2C_INIT_ERROR );
	}

} /* Baro_I2C_Init */

#ifdef FULL_FLIGHT_COMPUTER
/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   *
* 		IMU_I2C_Init                                                           *
*                                                                              *
* DESCRIPTION:                                                                 *
* 		Initializes the IMU's I2C MCU interface                                *
*                                                                              *
*******************************************************************************/
void IMU_I2C_Init
	(
	void
	)
{
imu_hi2c.Instance              = IMU_I2C;
imu_hi2c.Init.Timing           = 0x20303E5D;
imu_hi2c.Init.OwnAddress1      = 0;
imu_hi2c.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
imu_hi2c.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
imu_hi2c.Init.OwnAddress2      = 0;
imu_hi2c.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
imu_hi2c.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
imu_hi2c.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
if ( HAL_I2C_Init( &imu_hi2c ) != HAL_OK )
	{
	Error_Handler( ERROR_IMU_I2C_INIT_ERROR );
	}

/* Configure Analogue filter */
if ( HAL_I2CEx_ConfigAnalogFilter( &imu_hi2c, I2C_ANALOGFILTER_ENABLE ) != HAL_OK )
	{
	Error_Handler( ERROR_IMU_I2C_INIT_ERROR );
	}

/* Configure Digital filter */
if ( HAL_I2CEx_ConfigDigitalFilter( &imu_hi2c, 0 ) != HAL_OK )
	{
	Error_Handler( ERROR_IMU_I2C_INIT_ERROR );
	}

} /* IMU_I2C_Init */
#endif /* #ifdef FULL_FLIGHT_COMPUTER */


/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   *
* 		Flash_SPI_Init                                                         *
*                                                                              *
* DESCRIPTION:                                                                 *
* 		Initializes the SPI peripheral to be used with the external flash chip *
*                                                                              *
*******************************************************************************/
void Flash_SPI_Init
	(
	void
	)
{
/* SPI2 parameter configuration*/
flash_hspi.Instance                        = FLASH_SPI;
flash_hspi.Init.Mode                       = SPI_MODE_MASTER;
flash_hspi.Init.Direction                  = SPI_DIRECTION_2LINES;
flash_hspi.Init.DataSize                   = SPI_DATASIZE_8BIT;
flash_hspi.Init.CLKPolarity                = SPI_POLARITY_LOW;
flash_hspi.Init.CLKPhase                   = SPI_PHASE_1EDGE;
flash_hspi.Init.NSS                        = SPI_NSS_SOFT;
flash_hspi.Init.BaudRatePrescaler          = SPI_BAUDRATEPRESCALER_2;
flash_hspi.Init.FirstBit                   = SPI_FIRSTBIT_MSB;
flash_hspi.Init.TIMode                     = SPI_TIMODE_DISABLE;
flash_hspi.Init.CRCCalculation             = SPI_CRCCALCULATION_DISABLE;
flash_hspi.Init.CRCPolynomial              = 0x0;
flash_hspi.Init.NSSPMode                   = SPI_NSS_PULSE_ENABLE;
flash_hspi.Init.NSSPolarity                = SPI_NSS_POLARITY_LOW;
flash_hspi.Init.FifoThreshold              = SPI_FIFO_THRESHOLD_01DATA;
flash_hspi.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
flash_hspi.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
flash_hspi.Init.MasterSSIdleness           = SPI_MASTER_SS_IDLENESS_00CYCLE;
flash_hspi.Init.MasterInterDataIdleness    = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
flash_hspi.Init.MasterReceiverAutoSusp     = SPI_MASTER_RX_AUTOSUSP_DISABLE;
flash_hspi.Init.MasterKeepIOState          = SPI_MASTER_KEEP_IO_STATE_DISABLE;
flash_hspi.Init.IOSwap                     = SPI_IO_SWAP_DISABLE;
if ( HAL_SPI_Init( &flash_hspi ) != HAL_OK )
	{
	Error_Handler( ERROR_FLASH_SPI_INIT_ERROR );
	}

} /* Flash_SPI_Init */
#endif /* #ifndef BLINK */


/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   *
* 		GPIO_Init                                                              *
*                                                                              *
* DESCRIPTION:                                                                 *
* 		Initializes all GPIO pins and sets alternate functions                 *
*                                                                              *
*******************************************************************************/
void GPIO_Init
	(
 	void
	)
{
GPIO_InitTypeDef GPIO_InitStruct = {0};

/* GPIO Ports Clock Enable */
#ifndef BLINK
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
#endif
__HAL_RCC_GPIOE_CLK_ENABLE();
__HAL_RCC_GPIOH_CLK_ENABLE();

/*--------------------------- LED MCU PINS -----------------------------------*/

/* Configure GPIO pin Output Level */
HAL_GPIO_WritePin( STATUS_GPIO_PORT, 
                   STATUS_R_PIN | 
                   STATUS_B_PIN | 
                   STATUS_G_PIN    ,
                   GPIO_PIN_SET );

/* Configure GPIO pin : PE2 --> Status LED pin */
GPIO_InitStruct.Pin   = STATUS_R_PIN | 
                        STATUS_B_PIN | 
                        STATUS_G_PIN;
GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_OD;          
GPIO_InitStruct.Pull  = GPIO_NOPULL;                  
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;         
HAL_GPIO_Init( STATUS_GPIO_PORT, &GPIO_InitStruct );

/*-------------------------- FLASH MCU PINS ----------------------------------*/

#ifndef BLINK
	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin( FLASH_SS_GPIO_PORT  , FLASH_SS_PIN  , GPIO_PIN_SET   );
	HAL_GPIO_WritePin( FLASH_WP_GPIO_PORT  , FLASH_WP_PIN  , GPIO_PIN_RESET );
	HAL_GPIO_WritePin( FLASH_HOLD_GPIO_PORT, FLASH_HOLD_PIN, GPIO_PIN_SET   );

	/* Configure GPIO pin : FLASH_SS_Pin */
	GPIO_InitStruct.Pin   = FLASH_SS_PIN;
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init( FLASH_SS_GPIO_PORT, &GPIO_InitStruct );

	/* Configure GPIO pin : FLASH_WP_Pin */
	GPIO_InitStruct.Pin   = FLASH_WP_PIN;
	HAL_GPIO_Init( FLASH_WP_GPIO_PORT, &GPIO_InitStruct );

	/* Configure GPIO pin : FLASH_HOLD_Pin */
	GPIO_InitStruct.Pin   = FLASH_HOLD_PIN;
	HAL_GPIO_Init( FLASH_HOLD_GPIO_PORT, &GPIO_InitStruct );
#endif /* #ifndef BLINK */ 

/*--------------------------- USB MCU PINS -----------------------------------*/

#ifndef BLINK
	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin( USB_RST_GPIO_PORT, USB_RST_PIN, GPIO_PIN_SET );

	/*Configure GPIO pin : USB_DETECT_Pin */
	GPIO_InitStruct.Pin  = USB_DETECT_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init( USB_DETECT_GPIO_PORT, &GPIO_InitStruct );

	/*Configure GPIO pin : USB_SUSPEND_Pin */
	GPIO_InitStruct.Pin  = USB_SUSPEND_PIN;
	HAL_GPIO_Init( USB_SUSPEND_GPIO_PORT, &GPIO_InitStruct );

	/*Configure GPIO pin : USB_RST_Pin */
	GPIO_InitStruct.Pin   = USB_RST_PIN;
	GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull  = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init( USB_RST_GPIO_PORT, &GPIO_InitStruct );
#endif

} /* GPIO_Init */

#ifndef BLINK
/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   *
* 		USB_UART_Init                                                          *
*                                                                              *
* DESCRIPTION:                                                                 *
* 		Initializes the UART peripheral for use with the USB interface         *
*                                                                              *
*******************************************************************************/
void USB_UART_Init
	(
	void
	)
{
usb_huart.Instance                    = USB_UART;
usb_huart.Init.BaudRate               = 921600;
usb_huart.Init.WordLength             = UART_WORDLENGTH_8B;
usb_huart.Init.StopBits               = UART_STOPBITS_1;
usb_huart.Init.Parity                 = UART_PARITY_NONE;
usb_huart.Init.Mode                   = UART_MODE_TX_RX;
usb_huart.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
usb_huart.Init.OverSampling           = UART_OVERSAMPLING_16;
usb_huart.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
usb_huart.Init.ClockPrescaler         = UART_PRESCALER_DIV1;
usb_huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
if ( HAL_UART_Init( &usb_huart ) != HAL_OK )
	{
	Error_Handler( ERROR_USB_UART_INIT_ERROR );
	}
if ( HAL_UARTEx_SetTxFifoThreshold( &usb_huart, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK )
	{
	Error_Handler( ERROR_USB_UART_INIT_ERROR );
	}
if ( HAL_UARTEx_SetRxFifoThreshold( &usb_huart, UART_RXFIFO_THRESHOLD_1_8 ) != HAL_OK )
	{
	Error_Handler( ERROR_USB_UART_INIT_ERROR );
	}
if ( HAL_UARTEx_DisableFifoMode( &usb_huart ) != HAL_OK )
	{
	Error_Handler( ERROR_USB_UART_INIT_ERROR );
	}

} /* USB_UART_Init */
#endif /* #ifndef BLINK */


/*******************************************************************************
* END OF FILE                                                                  * 
*******************************************************************************/