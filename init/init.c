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
#include "pin_defines_A0002.h"
#include "main.h"
#include "init.h"
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

/* Initializes the RCC Oscillators according to the specified parameters
  in the RCC_OscInitTypeDef structure */
RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
RCC_OscInitStruct.HSIState            = RCC_HSI_DIV1;
RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_NONE;
if ( HAL_RCC_OscConfig( &RCC_OscInitStruct ) != HAL_OK )
	{
	Error_Handler();
	}

/* Initializes the CPU, AHB and APB buses clocks */
RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK    | RCC_CLOCKTYPE_SYSCLK
							| RCC_CLOCKTYPE_PCLK1   | RCC_CLOCKTYPE_PCLK2
							| RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;
RCC_ClkInitStruct.SYSCLKDivider  = RCC_SYSCLK_DIV1;
RCC_ClkInitStruct.AHBCLKDivider  = RCC_HCLK_DIV1;
RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

if ( HAL_RCC_ClockConfig( &RCC_ClkInitStruct, FLASH_LATENCY_1 ) != HAL_OK )
	{
	Error_Handler();
	}

} /* SystemClock_Config */


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
__HAL_RCC_GPIOH_CLK_ENABLE();
__HAL_RCC_GPIOE_CLK_ENABLE();

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

} /* GPIO_Init */


/*******************************************************************************
* END OF FILE                                                                  * 
*******************************************************************************/