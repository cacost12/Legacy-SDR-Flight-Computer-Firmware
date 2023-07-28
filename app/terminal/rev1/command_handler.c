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
uint8_t          num_bytes;           /* Number of bytes on which to 
                                         operate                              */
uint8_t          address[3];          /* flash address in byte form           */
uint8_t*         pbuffer;             /* Position within flash buffer         */
uint8_t          buffer[512];         /* buffer (flash extract)               */
FLASH_STATUS     flash_status;        /* Return value of flash API calls      */
USB_STATUS       usb_status;          /* Return value of USB API calls        */


/*------------------------------------------------------------------------------
 Pre-processing 
------------------------------------------------------------------------------*/
num_bytes                  = 0;
pflash_handle -> num_bytes = num_bytes;
pbuffer                    = &buffer[0];
memset( pbuffer, 0, sizeof( buffer ) );
address_to_bytes( pflash_handle -> address, &address[0] );


/*------------------------------------------------------------------------------
 Call API function 
------------------------------------------------------------------------------*/
switch ( subcommand )
	{
    /*-----------------------------READ Subcommand----------------------------*/
    case FLASH_READ_SUBCOMMAND:
        {

		/* Get flash address and number of bytes to read */
		usb_status = usb_receive( &( address[0] )  , 
                                  sizeof( address ), 
                                  USB_DEFAULT_TIMEOUT );
		if ( usb_status != USB_OK )
			{
			return COMMAND_USB_ERROR;
			}
        usb_status = usb_receive( &num_bytes, 
                                  sizeof( num_bytes ), 
                                  USB_DEFAULT_TIMEOUT );
        if ( usb_status != USB_OK )
            {
            return COMMAND_USB_ERROR;
            }

        /* Read memory */
        pflash_handle -> address = bytes_to_address( address );
        flash_status = flash_read( pflash_handle, num_bytes );
        
        /* Check for flash error */
        if ( flash_status != FLASH_OK )
            {
            /* Bytes not read */
            return FLASH_FAIL;
            }

        /* Transmit bytes from pbuffer over USB */
        usb_status = usb_transmit( pflash_handle -> pbuffer,
                                    num_bytes               ,
                                    HAL_FLASH_TIMEOUT );

        if ( usb_status != USB_OK )
            {
            /* Bytes not transimitted */
            return FLASH_USB_ERROR;
            }

		/* Bytes read and transimitted back sucessfully */
		return FLASH_OK;

		} /* FLASH_SUBCMD_READ */

    /*------------------------------ENABLE Subcommand-----------------------------*/
    case FLASH_ENABLE_SUBCOMMAND:
        {
		flash_write_enable();
		return FLASH_OK;
        } /* FLASH_SUBCMD_ENABLE */

    /*------------------------------DISABLE Subcommand----------------------------*/
    case FLASH_DISABLE_SUBCOMMAND:
        {
		flash_write_disable();
		return FLASH_OK;
        } /* FLASH_SUBCMD_DISABLE */

    /*------------------------------WRITE Subcommand------------------------------*/
    case FLASH_WRITE_SUBCOMMAND:
        {
		/* Get Address bits */
		usb_status = usb_receive( &( address[0] )  ,
                                  sizeof( address ),
                                  HAL_DEFAULT_TIMEOUT );

		if ( usb_status != USB_OK )	
			{
			/* Address not recieved */
			return FLASH_USB_ERROR;
            }
		else
			{
			/* Convert flash address to uint32_t */
			pflash_handle -> address = bytes_to_address( address );

			/* Get bytes to be written to flash */
			for ( int i = 0; i < num_bytes; i++ )
				{
				pbuffer = ( pflash_handle -> pbuffer ) + i;
				flash_status = usb_receive( pbuffer          , 
                                            sizeof( uint8_t ),
                                            HAL_DEFAULT_TIMEOUT );

				/* Return if usb call failed */
				if ( usb_status != USB_OK )
					{
					/* Bytes not received */
				    return FLASH_USB_ERROR;	
                    }

				}
            }

		/* Call API function */
		flash_status = flash_write( pflash_handle );

	    return flash_status;	
        } /* FLASH_SUBCMD_WRITE */

    /*------------------------------ERASE Subcommand------------------------------*/
    case FLASH_ERASE_SUBCOMMAND:
        {
		/* Call API Function*/
		flash_status = flash_erase( pflash_handle );

		return flash_status;
        } /* FLASH_SUBCMD_ERASE */

    /*------------------------------STATUS Subcommand-----------------------------*/
    case FLASH_STATUS_SUBCOMMAND:
        {
		/* Call API function */
		flash_status = flash_get_status( pflash_handle );

		/* Send status register contents back to PC */
		usb_status = usb_transmit( &( pflash_handle -> status_register ),
                                   sizeof( uint8_t )                    ,
                                   HAL_DEFAULT_TIMEOUT );

		/* Return status code */
		if      ( usb_status   != USB_OK   )
			{
			return FLASH_USB_ERROR;
			}
		else if ( flash_status != FLASH_OK )
			{
			return FLASH_SPI_ERROR;
			}
		else
			{
			return FLASH_OK;	
			}
        } /* FLASH_SUBCMD_STATUS */

    /*-----------------------------EXTRACT Subcommand-----------------------------*/
    case FLASH_EXTRACT_SUBCOMMAND:
        {
		/* Extracts the entire flash chip, flash chip address from 0 to 0x7FFFF */
		pflash_handle->pbuffer = &buffer[0];
		pflash_handle->address = 0;
		while( pflash_handle->address <= FLASH_MAX_ADDR )
			{
			flash_status = flash_read( pflash_handle, sizeof( buffer ) );
			if( flash_status == FLASH_OK )
				{
				usb_transmit( &buffer, sizeof( buffer ), HAL_FLASH_TIMEOUT );
				}
			else
				{
				/* Extract Failed */
				return FLASH_EXTRACT_ERROR;
				}

			/* Read from next address */
			(pflash_handle->address) += sizeof( buffer ) ;
			}

		return FLASH_OK;
        } /* FLASH_SUBCMD_EXTRACT */

    /*---------------------------Unrecognized Subcommand--------------------------*/
	default:
        {
	    return FLASH_UNRECOGNIZED_OP;	
        }

    }
} /* flash_cmd_execute */


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
* END OF FILE                                                                  *
*******************************************************************************/