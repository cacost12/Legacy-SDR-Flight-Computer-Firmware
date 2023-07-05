/*******************************************************************************
*                                                                              *
* FILE:                                                                        *
* 		init.h                                                                 *
*                                                                              *
* DESCRIPTION:                                                                 *
* 		Contains initialization routines for MCU core and peripherals          *
*                                                                              *
*******************************************************************************/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef INIT_H 
#define INIT_H

#ifdef __cplusplus
extern "C" {
#endif


/*------------------------------------------------------------------------------
 Standard Includes                                                              
------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------
 Project Includes                                                               
------------------------------------------------------------------------------*/
#include "main.h"


/*------------------------------------------------------------------------------
 Function prototypes                                                          
------------------------------------------------------------------------------*/
void SystemClock_Config      ( void );    /* System clock config              */
void PeriphCommonClock_Config( void );    /* Common peripheral clock config   */
void Baro_I2C_Init           ( void );    /* Baro Sensor I2C config           */
#ifdef FULL_FLIGHT_COMPUTER
    void IMU_I2C_Init            ( void ); /* IMU I2C config                  */
#endif
void Flash_SPI_Init          ( void );    /* External flash SPI config        */
void GPIO_Init               ( void );    /* GPIO configs                     */
void USB_UART_Init           ( void );    /* USB_UART                         */


#ifdef __cplusplus
}
#endif
#endif /* INIT_H */


/*******************************************************************************
* END OF FILE                                                                  * 
*******************************************************************************/