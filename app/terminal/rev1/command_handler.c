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

/* Hardware Modules */
#include "baro.h"
#include "buzzer.h"
#include "flash.h"
#include "ignition.h"
#include "imu.h"
#include "led.h"
#include "usb.h"

/* General */
#include "commands.h"
#include "command_handler.h"
#include "sensor.h"
#include "zav_error.h"


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

/* Extract bytes for export from SENSOR_ID struct */
void static extract_sensor_bytes 
	(
	SENSOR_DATA* sensor_data_ptr      ,
	SENSOR_ID*   sensor_ids_ptr       ,
	uint8_t      num_sensors          ,
	uint8_t*     sensor_data_bytes_ptr,
	uint8_t*     num_sensor_bytes
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
usb_status = USB_OK;


/*------------------------------------------------------------------------------
 Implementation 
------------------------------------------------------------------------------*/

/* Get Subcommand */
if ( is_subcommand_required( command ) == SUBCOMMAND_REQUIRED )
    {
    usb_status = usb_receive( &subcommand, sizeof( subcommand ), USB_DEFAULT_TIMEOUT );
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
        return ign_cmd_handler( subcommand );
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
*       Parses and executes ignition module subcommands                        *
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
        break;
		}

	case IGN_CONT_SUBCOMMAND:
		{
		ign_cont_status = ign_get_cont_info();
        usb_transmit( &ign_cont_status, sizeof( ign_cont_status ), USB_DEFAULT_TIMEOUT );
        ign_status = IGN_OK;
        break;
		}

	default:
		{
		return IGN_UNRECOGNIZED_CMD;
		}
    } 

/* Report status to PC */
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
*       Parses and executes flash module subcommands                           *
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
        usb_status = usb_receive( flash_buffer.buffer_ptr , 
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
        while ( flash_buffer.address <= FLASH_MAX_ADDR )
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
            
            flash_buffer.address += sizeof( buffer );
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


/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   *
* 		sensor_cmd_handler                                                     *
*                                                                              *
* DESCRIPTION:                                                                 *
*       Parses and executes sensor module subcommands                          *
*                                                                              *
*******************************************************************************/
COMMAND_STATUS sensor_cmd_handler 
	(
    SENSOR_SUBCOMMAND subcommand 
    )
{

/*------------------------------------------------------------------------------
 Local Variables  
------------------------------------------------------------------------------*/
SENSOR_STATUS sensor_status;                         /* Status indicating if 
                                                       subcommand function 
                                                       returned properly      */
USB_STATUS    usb_status;                            /* USB return codes      */
SENSOR_DATA   sensor_data;                           /* Struct with all sensor 
                                                        data                  */
uint8_t       sensor_data_bytes[ SENSOR_DATA_SIZE ]; /* Byte array with sensor 
                                                       readouts               */
uint8_t       num_sensor_bytes = SENSOR_DATA_SIZE;   /* Size of data in bytes */
uint8_t       num_sensors;                           /* Number of sensors to 
                                                        use for polling       */
uint8_t       poll_sensors[ SENSOR_MAX_NUM_POLL ];   /* Codes for sensors to
                                                        be polled             */
uint8_t       sensor_poll_cmd;                       /* Command codes used by 
                                                        sensor poll           */


/*------------------------------------------------------------------------------
 Initializations  
------------------------------------------------------------------------------*/
usb_status      = USB_OK;
sensor_status   = SENSOR_OK;
num_sensors     = 0;
sensor_poll_cmd = 0;
memset( &sensor_data_bytes[0], 0, sizeof( sensor_data_bytes ) );
memset( &sensor_data         , 0, sizeof( sensor_data       ) );
memset( &poll_sensors[0]     , 0, sizeof( poll_sensors      ) );


/*------------------------------------------------------------------------------
 Implementation 
------------------------------------------------------------------------------*/
switch ( subcommand )
	{
	/*--------------------------------------------------------------------------
	 SENSOR POLL 
	--------------------------------------------------------------------------*/
    case SENSOR_POLL_SUBCOMMAND:
		{
		/* Determine the which sensors to poll */
        usb_status = usb_receive( &num_sensors         , 
                                  sizeof( num_sensors ), 
                                  USB_DEFAULT_TIMEOUT );
        if ( usb_status != USB_OK )
            {
            return COMMAND_USB_ERROR;
            }
        usb_status = usb_receive( &poll_sensors[0],
                                  num_sensors     , 
                                  USB_DEFAULT_TIMEOUT );
        if ( usb_status != USB_OK )
            {
            return COMMAND_USB_ERROR;
            }

		/* Receive initiating command code  */
        usb_status = usb_receive( &sensor_poll_cmd,
                                  sizeof( sensor_poll_cmd ),
                                  USB_DEFAULT_TIMEOUT );
        if      ( usb_status      != USB_OK            )
            {
            return COMMAND_USB_ERROR; /* USB error */
            }
        else if ( sensor_poll_cmd != SENSOR_POLL_START )
            {
            /* Fail to initiate sensor poll */
            return COMMAND_SENSOR_POLL_ERROR;
            }

		/* Start polling sensors */
		while ( sensor_poll_cmd != SENSOR_POLL_STOP )
			{
			/* Get command code */
            usb_status = usb_receive( &sensor_poll_cmd         ,
                                      sizeof( sensor_poll_cmd ),
                                      SENSOR_POLL_CMD_TIMEOUT );
            if ( usb_status != USB_OK ) 
                {
                return COMMAND_USB_ERROR;
                }
			
			/* Execute command */
			switch ( sensor_poll_cmd )
				{

				/* Poll Sensors */
				case SENSOR_POLL_REQUEST:
					{
					sensor_status = sensor_poll( &sensor_data    , 
												 &poll_sensors[0],
												 num_sensors );
					if ( sensor_status != SENSOR_OK )
						{
						return COMMAND_SENSOR_POLL_ERROR;
						}
					else
						{
						/* Copy over sensor data into buffer */
						extract_sensor_bytes( &sensor_data, 
						                      &poll_sensors[0],
											  num_sensors     ,
											  &sensor_data_bytes[0],
											  &num_sensor_bytes );

						/* Transmit sensor bytes back to SDEC */
						usb_transmit( &sensor_data_bytes[0],
						              num_sensor_bytes     ,
									  USB_DEFAULT_TIMEOUT );
								
						break;
						}
					} /* case SENSOR_POLL_REQUEST */

				/* STOP Execution */
				case SENSOR_POLL_STOP:
					{
					/* Do nothing */
					break;
					} /* case SENSOR_POLL_STOP */

				/* WAIT, Pause execution */
				case SENSOR_POLL_WAIT:
					{
					/* Poll USB port until resume signal arrives */
					while( sensor_poll_cmd != SENSOR_POLL_RESUME )
						{
                        usb_status = usb_receive( &sensor_poll_cmd         , 
                                                  sizeof( sensor_poll_cmd ),
                                                  USB_DEFAULT_TIMEOUT );
                        if ( ( usb_status != USB_OK ) && ( usb_status != USB_TIMEOUT ) )
                            {
                            return COMMAND_USB_ERROR;
                            }
						}
					break;
					} /* case SENSOR_POLL_WAIT */

				/* Erroneous Command*/
				default:
					{
					return COMMAND_SENSOR_POLL_UNRECOGNIZED_CMD;
					}
				} /* switch( sensor_poll_cmd ) */

			} /* while( sensor_poll_cmd != SENSOR_POLL_STOP ) */
		
		return COMMAND_OK;
        } /* SENSOR_POLL_CODE */ 

	/*--------------------------------------------------------------------------
	 SENSOR DUMP 
	--------------------------------------------------------------------------*/
	case SENSOR_DUMP_SUBCOMMAND: 
		{
		/* Tell the PC how many bytes to expect */
        usb_transmit( &num_sensor_bytes,
                      sizeof( num_sensor_bytes ), 
                      USB_DEFAULT_TIMEOUT );

		/* Get the sensor readings */
	    if ( sensor_dump( &sensor_data ) != SENSOR_OK )	
            {
            return COMMAND_SENSOR_DUMP_ERROR;
            }

		/* Convert to byte array */
		memcpy( &(sensor_data_bytes[0]), &sensor_data, sizeof( sensor_data ) );

		/* Transmit sensor readings to PC */
        usb_transmit( &sensor_data_bytes[0]      , 
                      sizeof( sensor_data_bytes ), 
                      USB_DEFAULT_TIMEOUT );
        
        return COMMAND_OK;
        } /* SENSOR_DUMP_CODE */

	/*--------------------------------------------------------------------------
	 UNRECOGNIZED SUBCOMMAND 
	--------------------------------------------------------------------------*/
	default:
		{
		return ( COMMAND_UNRECOGNIZED_SUBCOMMAND );
        }
    }

} /* sensor_cmd_handler */


/*------------------------------------------------------------------------------
 Private Procedures 
------------------------------------------------------------------------------*/


/*******************************************************************************
*                                                                              *
* PROCEDURE:                                                                   *
* 		is_subcommand_required                                                 *
*                                                                              *
* DESCRIPTION:                                                                 *
*       Determines whether a command requires a subcommand                     *
*                                                                              *
*******************************************************************************/
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
*                                                                              *
* PROCEDURE:                                                                   *
* 		extract_sensor_bytes                                                   *
*                                                                              *
* DESCRIPTION:                                                                 *
*       Extract bytes for export from SENSOR_ID struct                         *
*                                                                              *
*******************************************************************************/
void static extract_sensor_bytes 
	(
	SENSOR_DATA* sensor_data_ptr      , /* In:  Sensor data in struct         */
	SENSOR_ID*   sensor_ids_ptr       , /* In:  Sensor ids                    */
	uint8_t      num_sensors          , /* In:  Number of sensors polled      */
	uint8_t*     sensor_data_bytes_ptr, /* Out: Sensor data in bytes          */
	uint8_t*     num_sensor_bytes       /* Out: Size of output data           */
	)
{
/*------------------------------------------------------------------------------
 Local Variables  
------------------------------------------------------------------------------*/
uint8_t*   output_ptr;    /* Pointer to data export output                    */
uint8_t*   input_ptr;     /* Pointer to data within SENSOR_ID struct          */
size_t     sensor_size;   /* Size in bytes of current sensor readout          */
SENSOR_ID  sensor_id;     /* Current Sensor ID                                */
SENSOR_ID* sensor_id_ptr; /* Pointer to current sensor ID                     */


/*------------------------------------------------------------------------------
 Initializations  
------------------------------------------------------------------------------*/
output_ptr        = sensor_data_bytes_ptr;
sensor_id_ptr     = sensor_ids_ptr;
sensor_id         = *(sensor_id_ptr);
*num_sensor_bytes = 0;


/*------------------------------------------------------------------------------
 Implementation 
------------------------------------------------------------------------------*/
for ( uint8_t i = 0; i < num_sensors; ++i )
	{
	/* Get position info of sensor readout */
	sensor_map( sensor_data_ptr, 
	            sensor_id      ,
				&input_ptr     ,
				&sensor_size );

	/* Copy data into output buffer */
	memcpy( output_ptr, input_ptr, sensor_size );

	/* Update size of output */
	*num_sensor_bytes += (uint8_t) sensor_size;

	/* Go to next sensor */ 
	if ( i != ( num_sensors-1) )
		{
		sensor_id_ptr++;
		sensor_id = *(sensor_id_ptr);
		output_ptr += sensor_size;
		}
	}

} /* extract_sensor_bytes */


/*******************************************************************************
* END OF FILE                                                                  *
*******************************************************************************/