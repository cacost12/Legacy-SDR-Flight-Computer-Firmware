/*******************************************************************************
*
* FILE: 
* 		commands.c
*
* DESCRIPTION: 
* 		Contains general command functions common to all embedded controllers
*
*******************************************************************************/


/*------------------------------------------------------------------------------
 Includes                                                               
------------------------------------------------------------------------------*/
#include "main.h"
#include "commands.h"
#include "usb.h"


/*------------------------------------------------------------------------------
 Procedures 
------------------------------------------------------------------------------*/


/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   *
* 		ping                                                                   *
*                                                                              *
* DESCRIPTION:                                                                 *
* 		Sends a 1 byte response back to host PC to signal a functioning        *
*       serial connection                                                      *
*                                                                              *
*******************************************************************************/
COMMAND_STATUS ping
    (
    void
    )
{
uint8_t board_id = BOARD_ID; /* Code specific to board and revision */
usb_transmit( &board_id, sizeof( board_id ), HAL_DEFAULT_TIMEOUT );
return COMMAND_OK;
} /* ping */


/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   *
* 		connect                                                                *
*                                                                              *
* DESCRIPTION:                                                                 *
*       Establishes a connection with PC by sending board/firmware identifiers *
*                                                                              *
*******************************************************************************/
COMMAND_STATUS connect
    (
    void
    )
{
uint8_t board_id    = BOARD_ID;
uint8_t firmware_id = FIRMWARE_ID;
usb_transmit( &board_id   , sizeof( board_id    ), HAL_DEFAULT_TIMEOUT );
usb_transmit( &firmware_id, sizeof( firmware_id ), HAL_DEFAULT_TIMEOUT );
return COMMAND_OK;
} /* connect */


/*******************************************************************************
* END OF FILE                                                                  * 
*******************************************************************************/