/*******************************************************************************
*
* FILE: 
* 		main.c
*
* DESCRIPTION: 
* 		Data logger - Logs sensor data during flight to flash memory	
*
*******************************************************************************/


/*------------------------------------------------------------------------------
 Standard Includes                                                                     
------------------------------------------------------------------------------*/
#include <stdbool.h>
#include <string.h>
#include "zav_pin_defines_A0002.h"
#include "zav_error.h"


/*------------------------------------------------------------------------------
 Project Includes                                                                     
------------------------------------------------------------------------------*/

/* Drivers */
#include "baro.h"
#include "flash.h"
#include "ignition.h"
#include "imu.h"
#include "led.h"
#include "usb.h"

/* Modules */
#include "commands.h"
#include "sensor.h"
#include "command_handler.h"

/* Application Layer */
#include "main.h"
#include "init.h"


/*------------------------------------------------------------------------------
 MCU Peripheral Handles                                                         
------------------------------------------------------------------------------*/
I2C_HandleTypeDef  baro_hi2c;    /* Baro sensor    */
I2C_HandleTypeDef  imu_hi2c;     /* IMU            */
SPI_HandleTypeDef  flash_hspi;   /* External flash */
UART_HandleTypeDef usb_huart;    /* USB            */


/*------------------------------------------------------------------------------
 Application entry point                                                      
------------------------------------------------------------------------------*/
int main
	(
 	void
	)
{
/*------------------------------------------------------------------------------
 Local Variables                                                                  
------------------------------------------------------------------------------*/

/* USB */
COMMAND_CODE  command;   /* Command code for USB mode */
USB_STATUS    usb_status; /* Return codes from USB driver */

/* FLASH */
FLASH_CONFIG  flash_config;                    /* Flash chip configuration    */
FLASH_BUFFER  flash_handle;                    /* Flash buffer API handle     */
uint8_t       flash_buffer[ DEF_FLASH_BUFFER_SIZE ]; /* Flash Data buffer     */

/* Sensors */
SENSOR_DATA   sensor_data;                     /* All sensor data             */
BARO_STATUS   baro_status;
BARO_CONFIG   baro_configs;                    /* Baro sensor config settings */
IMU_CONFIG    imu_configs;                     /* IMU config settings         */
SENSOR_STATUS sensor_status;                   /* Sensor module return codes  */

/* Time */
uint32_t      start_time;
uint32_t      time;

/* Ground pressure calibration/timeout */
float         ground_pressure;
float         temp_pressure;


/*------------------------------------------------------------------------------
 Variable Initializations                                                               
------------------------------------------------------------------------------*/

/* FLASH */
flash_config.write_protected   = FLASH_WP_WRITE_ENABLED;
flash_config.bpl_bits          = FLASH_BPL_NONE;
flash_config.bpl_write_protect = FLASH_BPL_READ_WRITE;

flash_handle.buffer_size       = 0;
flash_handle.buffer_ptr        = &flash_buffer[0];
flash_handle.address           = 0;

/* Baro sensor configurations */
baro_configs.enable            = BARO_PRESS_TEMP_ENABLED;
baro_configs.mode              = BARO_NORMAL_MODE;
baro_configs.press_OSR_setting = BARO_PRESS_OSR_X8;
baro_configs.temp_OSR_setting  = BARO_TEMP_OSR_X1;
baro_configs.ODR_setting       = BARO_ODR_50HZ;
baro_configs.IIR_setting       = BARO_IIR_COEF_0;


/* IMU Configurations */
imu_configs.sensor_enable      = IMU_ENABLE_GYRO_ACC_TEMP;
imu_configs.acc_odr            = IMU_ODR_100;
imu_configs.gyro_odr           = IMU_ODR_100;
imu_configs.mag_odr            = MAG_ODR_10HZ;
imu_configs.acc_filter         = IMU_FILTER_NORM_AVG4;
imu_configs.gyro_filter        = IMU_FILTER_NORM_AVG4;
imu_configs.acc_filter_mode    = IMU_FILTER_FILTER_MODE;
imu_configs.gyro_filter_mode   = IMU_FILTER_FILTER_MODE;
imu_configs.acc_range          = IMU_ACC_RANGE_16G;
imu_configs.gyro_range         = IMU_GYRO_RANGE_500;
imu_configs.mag_op_mode        = MAG_NORMAL_MODE;
imu_configs.mag_xy_repititions = 9; /* BMM150 Regular Preset Recomendation */
imu_configs.mag_z_repititions  = 15;

/* Module return codes */
baro_status                   = BARO_OK;
sensor_status                 = SENSOR_OK;
usb_status                    = USB_OK;


/*------------------------------------------------------------------------------
 MCU/HAL Initialization                                                                  
------------------------------------------------------------------------------*/
HAL_Init                (); /* Reset peripherals, initialize flash interface 
                               and Systick.                                   */
SystemClock_Config      (); /* System clock                                   */
GPIO_Init               (); /* GPIO                                           */
USB_UART_Init           (); /* USB UART                                       */
Baro_I2C_Init           (); /* Barometric pressure sensor                     */
IMU_I2C_Init            (); /* IMU                                            */
Flash_SPI_Init          (); /* External flash chip                            */


/*------------------------------------------------------------------------------
External Hardware Initializations 
------------------------------------------------------------------------------*/

/* Flash Chip */
if ( flash_init( flash_config ) != FLASH_OK )
	{
	Error_Handler( ERROR_FLASH_INIT_ERROR );
	}

/* Barometric pressure sensor */
if ( baro_init( &baro_configs ) != BARO_OK )
	{
	Error_Handler( ERROR_BARO_INIT_ERROR );
	}

/* IMU */
if ( imu_init( &imu_configs ) != IMU_OK )
	{
	Error_Handler( ERROR_IMU_INIT_ERROR );
	}


/*------------------------------------------------------------------------------
 Setup safety checks 
------------------------------------------------------------------------------*/

/* Check switch pin */
if ( ign_switch_cont() )
	{
	Error_Handler( ERROR_DATA_HAZARD_ERROR );
	}
else
	{
	led_set_color( LED_GREEN );
 	}


/*------------------------------------------------------------------------------
 Event Loop                                                                  
------------------------------------------------------------------------------*/
while (1)
	{
	/*--------------------------------------------------------------------------
	 USB MODE 
	--------------------------------------------------------------------------*/
	if ( usb_detect() )
		{
		/* Get command from USB port */
		usb_status = usb_receive( &command, sizeof( command ), USB_DEFAULT_TIMEOUT );
		if ( usb_status == USB_OK )
			{
			command_handler( command );
			}
		} 

	/*--------------------------------------------------------------------------
	 DATA LOGGER MODE 
	--------------------------------------------------------------------------*/

	/* Poll switch */
	if ( ign_switch_cont() ) /* Enter data logger mode */
		{
		/*----------------------------------------------------------------------
		 Setup	
		----------------------------------------------------------------------*/
		led_set_color( LED_CYAN );

		/* Calibrate the ground pressure */
		for ( uint8_t i = 0; i < 10; ++i )
			{
			baro_status = baro_get_pressure( &temp_pressure );
			if ( baro_status != BARO_OK )
				{
				Error_Handler( ERROR_BARO_CAL_ERROR );
				}
			ground_pressure += temp_pressure;
			}
		ground_pressure /= 10;

		/* Erase flash chip */
		if ( flash_erase() != FLASH_OK )
			{
			Error_Handler( ERROR_FLASH_ERROR );
			}

		/* Wait until erase is complete */
		while ( flash_is_flash_busy() == FLASH_BUSY ){}

		/* Record data for 2 minutes, reset flash if launch has not been 
		   detected */
		start_time = HAL_GetTick();
		while ( temp_pressure > ( ground_pressure - LAUNCH_DETECT_THRESHOLD ) )
			{
			time = HAL_GetTick() - start_time;

			/* Poll sensors */
			sensor_status = sensor_dump( &sensor_data );
			temp_pressure = sensor_data.baro_pressure;
			if ( sensor_status != SENSOR_OK )
				{
				Error_Handler( ERROR_SENSOR_CMD_ERROR );
				}

			/* Write to flash */
			while( flash_is_flash_busy() == FLASH_BUSY ){}
			store_frame( flash_handle, &sensor_data, time );

			/* Update memory pointer */
			flash_handle.address += SENSOR_FRAME_SIZE;

			/* Timeout detection */
			if ( time >= LAUNCH_DETECT_TIMEOUT )
				{
				/* Erase the flash      */
				flash_erase();
				while ( flash_is_flash_busy() == FLASH_BUSY ){};

				/* Reset the timer      */
				start_time = HAL_GetTick();

				/* Reset memory pointer */
				flash_handle.address = 0;
				} /* if ( time >= LAUNCH_DETECT_TIMEOUT ) */
			} /* while ( temp_pressure ) */

		/*----------------------------------------------------------------------
		 Main Loop 
		----------------------------------------------------------------------*/
		while ( 1 )
			{
			/* Poll sensors */
			time =  HAL_GetTick() - start_time;
			sensor_status = sensor_dump( &sensor_data );
			if ( sensor_status != SENSOR_OK )
				{
				Error_Handler( ERROR_SENSOR_CMD_ERROR );
				}

			/* Write to flash */
			while( flash_is_flash_busy() == FLASH_BUSY ){}
			store_frame( flash_handle, &sensor_data, time );

			/* Update memory pointer */
			flash_handle.address += SENSOR_FRAME_SIZE;

			/* Check if flash memory if full */
			if ( flash_handle.address + SENSOR_FRAME_SIZE > FLASH_MAX_ADDR )
				{
				/* Idle */
				led_set_color( LED_BLUE );
				while ( !usb_detect() ) {}
				break;
				}
			} /* while (1) Main Loop */
		} /* if ( ign_switch_cont() )*/

	} /* while (1) Entire Program Loop */
} /* main */


/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   * 
* 		store_frame                                                            *
*                                                                              *
* DESCRIPTION:                                                                 * 
*       Store a frame of flight computer data in flash                         *
*                                                                              *
*******************************************************************************/
FLASH_STATUS store_frame 
	(
	FLASH_BUFFER  flash_handle   ,
	SENSOR_DATA*  sensor_data_ptr,
	uint32_t      time
	)
{
/*------------------------------------------------------------------------------
Local variables 
------------------------------------------------------------------------------*/
uint8_t      buffer[ SENSOR_FRAME_SIZE ];   /* Sensor data in byte form */


/*------------------------------------------------------------------------------
 Store Data 
------------------------------------------------------------------------------*/

/* Put data into buffer for flash write */
memcpy( &buffer[0], &time          , sizeof( uint32_t    ) );
memcpy( &buffer[4], sensor_data_ptr, sizeof( SENSOR_DATA ) );

/* Set buffer pointer */
flash_handle.buffer_ptr  = &buffer[0];
flash_handle.buffer_size = SENSOR_FRAME_SIZE;

/* Write to flash */
return flash_write( flash_handle );
} /* store_frame */


/*******************************************************************************
* END OF FILE                                                                  * 
*******************************************************************************/