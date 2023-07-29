/*******************************************************************************
*
* FILE: 
* 		command_handler.c
*
* DESCRIPTION: 
* 	    Processes commands sent to the flight computer to test hardware and 
*       software or exercise low level control over computer hardware 
*
*******************************************************************************/


/*------------------------------------------------------------------------------
 Includes                                                               
------------------------------------------------------------------------------*/

/* Standard */
#include <string.h>
#include <stdbool.h>

/* General */
#include "command_handler.h"
#include "commands.h"
#include "sensor.h"
#include "zav_error.h"

/* Hardware Modules */
#include "baro.h"
#include "buzzer.h"
#include "flash.h"
#include "ignition.h"
#include "imu.h"
#include "led.h"
#include "usb.h"


/*------------------------------------------------------------------------------
 Global Variables 
------------------------------------------------------------------------------*/

/* Hash table, key - command, value - boolean indicating if the command has a 
   subcommand */
static bool subcommand_commands[] = {
    SUBCOMMAND_NOT_REQUIRED,    /* Dummy Value */
    SUBCOMMAND_NOT_REQUIRED,    /* ping        */ 
    SUBCOMMAND_NOT_REQUIRED,    /* connect     */
    SUBCOMMAND_REQUIRED    ,    /* ignite      */
    SUBCOMMAND_REQUIRED    ,    /* flash       */
    SUBCOMMAND_REQUIRED         /* sensor      */
};


/*------------------------------------------------------------------------------
 Private Prototypes 
------------------------------------------------------------------------------*/

/* Determines whether a command requires a subcommand */
static inline bool is_subcommand_required
    (
    COMMAND_CODE command
    );

/* Converts a flash memory address in byte format to uint32_t format */
static inline uint32_t flash_bytes_to_address 
	(
	uint8_t address_bytes[3]
	);


/*------------------------------------------------------------------------------
 Procedures 
------------------------------------------------------------------------------*/


/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   *
* 		command_handler                                                        *
*                                                                              *
* DESCRIPTION:                                                                 *
* 		Processes commands and calls appropriate subroutines                   *
*                                                                              *
*******************************************************************************/
COMMAND_STATUS command_handler 
    (
    COMMAND_CODE command
    )
{
/*------------------------------------------------------------------------------
 Local Variables 
------------------------------------------------------------------------------*/
uint8_t    subcommand;    /* Subcommand from terminal interface */
USB_STATUS usb_status;    /* Return codes from USB interface    */


/*------------------------------------------------------------------------------
 Initializations 
------------------------------------------------------------------------------*/
subcommand = 0;
USB_STATUS = USB_OK;


/*------------------------------------------------------------------------------
 Implementation 
------------------------------------------------------------------------------*/

/* Get Subcommand */
if ( is_subcommand_required( command ) == SUBCOMMAND_REQUIRED )
    {
    usb_status = usb_receieve( &subcommand, sizeof( subcommand ), USB_DEFAULT_TIMEOUT );
    if ( usb_status != USB_OK )
        {
        return COMMAND_USB_ERROR;
        }
    }

/* Process command */
switch( command )
    {
    /*----------------------------- Ping Command -----------------------------*/
    case COMMAND_PING_CODE:
        {
        return ping();
        }

    /*--------------------------- Connect Command ----------------------------*/
    case COMMAND_CONNECT_CODE:
        {
        return connect();
        }

    /*---------------------------- Sensor Command ----------------------------*/
    case COMMAND_SENSOR_CODE:
        {
        return sensor_cmd_handler( subcommand );
        }

    /*---------------------------- Ignite Command ----------------------------*/
    case COMMAND_IGNITE_CODE:
        {
        return ignite_cmd_handler( subcommand );
        } /* IGNITE_OP */

    /*---------------------------- Flash Command ------------------------------*/
    case COMMAND_FLASH_CODE:
        {
        return flash_cmd_handler( subcommand );
        } /* FLASH_OP */

    /*------------------------ Unrecognized Command ---------------------------*/
    default:
        {
        /* Unsupported command code flash the red LED */
        return COMMAND_UNRECOGNIZED_CMD;
        }

    } /* case( command ) */

return COMMAND_UNKNOWN_ERROR;
} /* terminal_exec_cmd */


/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   *
* 		ign_cmd_handler                                                        *
*                                                                              *
* DESCRIPTION:                                                                 *
* 		Receives an ignition subcommand over the serial port and calls the     *
*       appropriate driver function                                            *
*                                                                              *
*******************************************************************************/
COMMAND_STATUS ign_cmd_handler
	(
    IGN_SUBCOMMAND subcommand 
    )
{
/*------------------------------------------------------------------------------
 Local Variables  
------------------------------------------------------------------------------*/
IGN_STATUS      ign_status;
IGN_CONT_STATUS ign_cont_status;


/*------------------------------------------------------------------------------
 Initializations 
------------------------------------------------------------------------------*/
ign_status      = IGN_OK;
ign_cont_status = 0;


/*------------------------------------------------------------------------------
 Implementation 
------------------------------------------------------------------------------*/

/* Execute */
switch( subcommand )
	{
	case IGN_MAIN_DEPLOY_SUBCOMMAND:
		{
		ign_status = ign_deploy_main();
        break;
		}

	case IGN_DROGUE_DEPLOY_SUBCOMMAND:
		{
		ign_status = ign_deploy_drogue();
		}

	case IGN_CONT_SUBCOMMAND:
		{
		ign_cont_status = ign_get_cont_info();
        usb_transmit( &ign_cont_status, sizeof( ign_cont_status ), USB_DEFAULT_TIMEOUT );
        ign_status = IGN_OK;
		}

	default:
		{
		return IGN_UNRECOGNIZED_CMD;
		}
    } 

/* Send return code to PC */
usb_transmit( &ign_status, sizeof( ign_status ), USB_DEFAULT_TIMEOUT );
if ( ign_status != IGN_OK )
    {
    return COMMAND_IGN_ERROR;
    }
else
    {
    return COMMAND_OK;
    }

} /* ign_cmd_handler */


/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   *
* 		flash_cmd_handler                                                      *
*                                                                              *
* DESCRIPTION:                                                                 *
* 		Executes a flash subcommand based on input from the sdec terminal      *
*                                                                              *
*******************************************************************************/
COMMAND_STATUS flash_cmd_handler
	(
    FLASH_SUBCOMMAND subcommand   
    )
{
/*------------------------------------------------------------------------------
 Local Variables 
------------------------------------------------------------------------------*/
FLASH_BUFFER flash_buffer;      /* Buffer to use with flash read/write calls  */
uint8_t      address_bytes[3];  /* flash address in byte form                 */
uint8_t      buffer[512];       /* buffer (flash extract)                     */
uint8_t      status_reg;        /* Flash status register contents             */
FLASH_STATUS flash_status;      /* Return value of flash API calls            */
USB_STATUS   usb_status;        /* Return value of USB API calls              */


/*------------------------------------------------------------------------------
 Pre-processing 
------------------------------------------------------------------------------*/
flash_buffer.buffer_size = 0;
flash_buffer.address     = 0;
flash_buffer.buffer_ptr  = buffer;
status_reg               = FLASH_STATUS_REG_RESET_VAL;
usb_status               = USB_OK;
memset( buffer       , 0     , sizeof( buffer        ) );
memset( address_bytes, 0     , sizeof( address_bytes ) );


/*------------------------------------------------------------------------------
 Implementation  
------------------------------------------------------------------------------*/

/* Get flash address, data size, and data from serial port */
if ( ( subcommand == FLASH_READ_SUBCOMMAND  ) || 
     ( subcommand == FLASH_WRITE_SUBCOMMAND ) )
    {
    usb_status = usb_receive( &( address_bytes[0] )  , 
                              sizeof( address_bytes ),
                              USB_DEFAULT_TIMEOUT );
    if ( usb_status != USB_OK )
        {
        return COMMAND_USB_ERROR;
        }
    flash_buffer.address = flash_bytes_to_address( address_bytes );

    usb_status = usb_receive( &( flash_buffer.buffer_size ), 
                              sizeof( uint8_t )            , 
                              USB_DEFAULT_TIMEOUT );
    if ( usb_status != USB_OK )
        {
        return COMMAND_USB_ERROR;
        }

    if ( subcommand == FLASH_WRITE_SUBCOMMAND )
        {
        usb_status = usb_recieve( flash_buffer.buffer_ptr , 
                                  flash_buffer.buffer_size, 
                                  flash_buffer.buffer_size*USB_DEFAULT_TIMEOUT );
        if ( usb_status != USB_OK )
            {
            return COMMAND_USB_ERROR;
            }
        }
    }

/* Execute command */
switch ( subcommand )
	{
    /*-------------------------------READ Subcommand------------------------------*/
    case FLASH_READ_SUBCOMMAND:
        {
        if ( flash_read( flash_buffer ) != FLASH_OK )
            {
            return COMMAND_FLASH_READ_ERROR;
            }

        usb_status = usb_transmit( &buffer[0]              , 
                                   flash_buffer.buffer_size, 
                                   flash_buffer.buffer_size*USB_DEFAULT_TIMEOUT );
        if ( usb_status != USB_OK )
            {
            return COMMAND_USB_ERROR;
            }

		return COMMAND_OK;
		} /* FLASH_READ_SUBCOMMAND */

    /*------------------------------ENABLE Subcommand-----------------------------*/
    case FLASH_ENABLE_SUBCOMMAND:
        {
		flash_write_enable();
		return COMMAND_OK;
        } /* FLASH_ENABLE_SUBCOMMAND */

    /*------------------------------DISABLE Subcommand----------------------------*/
    case FLASH_DISABLE_SUBCOMMAND:
        {
		flash_write_disable();
		return COMMAND_OK;
        } /* FLASH_DISABLE_SUBCOMMAND */

    /*------------------------------WRITE Subcommand------------------------------*/
    case FLASH_WRITE_SUBCOMMAND:
        {
        if ( flash_write( flash_buffer ) != FLASH_OK )
            {
            return COMMAND_FLASH_WRITE_ERROR;
            }
	    return COMMAND_OK;	
        } /* FLASH_WRITE_SUBCOMMAND */

    /*------------------------------ERASE Subcommand------------------------------*/
    case FLASH_ERASE_SUBCOMMAND:
        {
        if ( flash_erase() != FLASH_OK )
            {
            return COMMAND_FLASH_ERROR;
            }
		return COMMAND_OK;
        } /* FLASH_ERASE_SUBCOMMAND */

    /*------------------------------STATUS Subcommand-----------------------------*/
    case FLASH_STATUS_SUBCOMMAND:
        {
        if ( flash_get_status( &status_reg ) != FLASH_OK )
            {
            return COMMAND_FLASH_ERROR;
            }
		usb_status = usb_transmit( &status_reg, sizeof( status_reg ), USB_DEFAULT_TIMEOUT );
		if ( usb_status   != USB_OK   )
			{
			return COMMAND_USB_ERROR;
			}
        return COMMAND_OK;
        } /*  FLASH_STATUS_SUBCOMMAND */

    /*-----------------------------EXTRACT Subcommand-----------------------------*/
    case FLASH_EXTRACT_SUBCOMMAND:
        {
        flash_buffer.address     = 0;
        flash_buffer.buffer_size = sizeof( buffer );
        while ( flash_buffer.buffer_size <= FLASH_MAX_ADDR )
            {
            if ( flash_read( flash_buffer ) != FLASH_OK )
                {
                return COMMAND_FLASH_ERROR;
                }

            usb_status = usb_transmit( &buffer[0], sizeof( buffer ), HAL_FLASH_TIMEOUT );
            if ( usb_status != USB_OK )
                {
                return COMMAND_USB_ERROR;
                }
            
            flash_buffer.address == sizeof( buffer );
            }

		return COMMAND_OK;
        } /* FLASH_EXTRACT_SUBCOMMAND */

    /*---------------------------Unrecognized Subcommand--------------------------*/
	default:
        {
	    return COMMAND_UNRECOGNIZED_SUBCOMMAND;	
        }

    }
} /* flash_cmd_handler */


/*------------------------------------------------------------------------------
 Private Procedures 
------------------------------------------------------------------------------*/


/* Determines whether a command requires a subcommand */
static inline bool is_subcommand_required
    (
    COMMAND_CODE command
    )
{
return subcommand_commands[command];
} /* is_subcommand_required */


/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   * 
* 		flash_bytes_to_address                                                 *
*                                                                              *
* DESCRIPTION:                                                                 * 
* 		Converts a flash memory address in byte format to uint32_t format      *
*                                                                              *
*******************************************************************************/
static inline uint32_t flash_bytes_to_address 
	(
	uint8_t address_bytes[3]
	)
{
return ( (uint32_t) address_bytes[0] << 16 ) |
	   ( (uint32_t) address_bytes[1] << 8  ) |
	   ( (uint32_t) address_bytes[2] << 0  );
} /*  flash_bytes_to_address */



/*******************************************************************************
* END OF FILE                                                                  *
*******************************************************************************/