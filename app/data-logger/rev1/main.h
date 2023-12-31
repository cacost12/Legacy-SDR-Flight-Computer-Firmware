/*******************************************************************************
*
* FILE: 
* 		main.h
*
* DESCRIPTION: 
* 		Processes commands recieved from a host PC, provides fine control over 
*       flight computer hardware resources
*
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif


/*------------------------------------------------------------------------------
 Standard Includes                                                                    
------------------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"


/*------------------------------------------------------------------------------
 Project Includes  
------------------------------------------------------------------------------*/
//#include "sensor.h"
#include "flash.h"


/*------------------------------------------------------------------------------
 Macros  
------------------------------------------------------------------------------*/

/* General MCU HAL related macros */
#define DEF_BUFFER_SIZE        ( 16  )     /* Default size of buffer arrays   */
#define DEF_FLASH_BUFFER_SIZE  ( 32  )     /* Default size of flash buffers   */

/* Timeouts */
#ifndef ZAV_DEBUG
	#define HAL_DEFAULT_TIMEOUT    ( 10  ) /* Default timeout for polling 
	                                          operations                     */
	#define HAL_SENSOR_TIMEOUT     ( 40  ) /* Timeout for sensor polling      */
#else
	/* Disable timeouts when debugging */
	#define HAL_DEFAULT_TIMEOUT    ( 0xFFFFFFFF )  
	#define HAL_SENSOR_TIMEOUT     ( 0xFFFFFFFF ) 
#endif /* ZAV_DEBUG */

/* Sensor Data Frame Size */
#if   defined( FULL_FLIGHT_COMPUTER )
	#define SENSOR_FRAME_SIZE      ( 36 ) 

#elif defined( BASE_FLIGHT_COMPUTER )
	#define SENSOR_FRAME_SIZE      ( 16 )

#elif defined( LEGACY_FLIGHT_COMPUTER )
	#define SENSOR_FRAME_SIZE      ( 32 )

#elif defined( LEGACY_FLIGHT_COMPUTER_LITE )
	#define SENSOR_FRAME_SIZE      ( 12 )

#endif

/* Launch detection parameters */
#define LAUNCH_DETECT_THRESHOLD      ( 1000   ) /* 1kPa            */
#define LAUNCH_DETECT_TIMEOUT        ( 120000 ) /* ms -> 2 minutes */


/*------------------------------------------------------------------------------
 Function prototypes                                             
------------------------------------------------------------------------------*/

/* Store a frame of flight computer data in flash */
FLASH_STATUS store_frame 
	(
	FLASH_BUFFER  flash_handle   ,
	SENSOR_DATA*  sensor_data_ptr,
	uint32_t      time
	);


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */


/*******************************************************************************
* END OF FILE                                                                  *
*******************************************************************************/