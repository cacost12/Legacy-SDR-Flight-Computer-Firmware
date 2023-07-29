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
#ifdef VALVE_CONTROLLER
	VALVE_STATUS valve_status; /* status codes from valve API */
#endif

/*------------------------------------------------------------------------------
 Initializations  
------------------------------------------------------------------------------*/
usb_status      = USB_OK;
sensor_status   = SENSOR_OK;
#ifdef VALVE_CONTROLLER
	valve_status = VALVE_OK;
#endif
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
    case SENSOR_POLL_CODE:
		{
		/* Determine the number of sensors to poll */
		#ifndef VALVE_CONTROLLER 
			usb_status = usb_receive( &num_sensors, 
									sizeof( num_sensors ), 
									HAL_DEFAULT_TIMEOUT );
			if ( usb_status != USB_OK )
				{
				return SENSOR_USB_FAIL;
				}
		#else
			if ( cmd_source == CMD_SOURCE_USB )
				{
				usb_status = usb_receive( &num_sensors, 
										sizeof( num_sensors ), 
										HAL_DEFAULT_TIMEOUT );
				if ( usb_status != USB_OK )
					{
					return SENSOR_USB_FAIL;
					}
				}
			else
				{
				valve_status = valve_receive( &num_sensors, 
				                              sizeof( num_sensors ), 
											  HAL_DEFAULT_TIMEOUT );
				if ( valve_status != VALVE_OK )
					{
					return SENSOR_VALVE_UART_ERROR;
					}
				}
		#endif /* #ifdef VALVE_CONTROLLER */

		/* Determine which sensors to poll */
		#ifndef VALVE_CONTROLLER 
			usb_status = usb_receive( &poll_sensors[0],
									num_sensors     , 
									HAL_SENSOR_TIMEOUT );
			if ( usb_status != USB_OK )
				{
				return SENSOR_USB_FAIL;
				}
		#else
			if ( cmd_source == CMD_SOURCE_USB )
				{
				usb_status = usb_receive( &poll_sensors[0],
										num_sensors     , 
										HAL_SENSOR_TIMEOUT );
				if ( usb_status != USB_OK )
					{
					return SENSOR_USB_FAIL;
					}
				}
			else
				{
				valve_status = valve_receive( &poll_sensors[0],
											num_sensors     ,
											HAL_SENSOR_TIMEOUT );
				if ( valve_status != VALVE_OK )
					{
					return SENSOR_VALVE_UART_ERROR;
					}
				}
		#endif /* #ifndef VALVE_CONTROLLER */

		/* Receive initiating command code  */
		#ifndef VALVE_CONTROLLER
			usb_status = usb_receive( &sensor_poll_cmd,
									sizeof( sensor_poll_cmd ),
									HAL_DEFAULT_TIMEOUT );
			if      ( usb_status      != USB_OK            )
				{
				return SENSOR_USB_FAIL; /* USB error */
				}
			else if ( sensor_poll_cmd != SENSOR_POLL_START )
				{
				/* SDEC fails to initiate sensor poll */
				return SENSOR_POLL_FAIL_TO_START;
				}
		#else
			if ( cmd_source == CMD_SOURCE_USB )
				{
				usb_status = usb_receive( &sensor_poll_cmd,
										sizeof( sensor_poll_cmd ),
										HAL_DEFAULT_TIMEOUT );
				if      ( usb_status      != USB_OK            )
					{
					return SENSOR_USB_FAIL; /* USB error */
					}
				else if ( sensor_poll_cmd != SENSOR_POLL_START )
					{
					/* SDEC fails to initiate sensor poll */
					return SENSOR_POLL_FAIL_TO_START;
					}
				}
			else
				{
				valve_status = valve_receive( &sensor_poll_cmd, 
											sizeof( sensor_poll_cmd ), 
											HAL_DEFAULT_TIMEOUT );
				if ( valve_status != VALVE_OK )
					{
					return SENSOR_VALVE_UART_ERROR;
					}
				else if ( sensor_poll_cmd != SENSOR_POLL_START )
					{
					return SENSOR_POLL_FAIL_TO_START;
					}
				}
		#endif

		/* Start polling sensors */
		while ( sensor_poll_cmd != SENSOR_POLL_STOP )
			{
			/* Get command code */
			#ifndef VALVE_CONTROLLER 
				usb_status = usb_receive( &sensor_poll_cmd         ,
										sizeof( sensor_poll_cmd ),
										HAL_DEFAULT_TIMEOUT );
				if ( usb_status != USB_OK ) 
					{
					return SENSOR_USB_FAIL;
					}
			#else
				if ( cmd_source == CMD_SOURCE_USB )
					{
					usb_status = usb_receive( &sensor_poll_cmd         ,
											sizeof( sensor_poll_cmd ),
											HAL_DEFAULT_TIMEOUT );
					if ( usb_status != USB_OK ) 
						{
						return SENSOR_USB_FAIL;
						}
					}
				else
					{
					valve_status = valve_receive( &sensor_poll_cmd         , 
					                              sizeof( sensor_poll_cmd ), 
												  HAL_DEFAULT_TIMEOUT );
					if ( valve_status != VALVE_OK )
						{
						return SENSOR_VALVE_UART_ERROR;
						}
					}
			#endif /* #ifndef VALVE_CONTROLLER */
			
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
						return SENSOR_POLL_FAIL;
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
									  HAL_SENSOR_TIMEOUT );
								
						break;
						}
					} /* case SENSOR_POLL_REQUEST */

				/* STOP Executtion */
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
						#ifndef VALVE_CONTROLLER
							usb_receive( &sensor_poll_cmd, 
										sizeof( sensor_poll_cmd ),
										HAL_DEFAULT_TIMEOUT );
						#else
							if ( cmd_source == CMD_SOURCE_USB )
								{
								usb_receive( &sensor_poll_cmd, 
											sizeof( sensor_poll_cmd ),
											HAL_DEFAULT_TIMEOUT );
								}
							else
								{
								valve_receive( &sensor_poll_cmd         , 
								               sizeof( sensor_poll_cmd ), 
											   HAL_DEFAULT_TIMEOUT );
								}
						#endif
						}
					break;
					} /* case SENSOR_POLL_WAIT */

				/* Erroneous Command*/
				default:
					{
					return SENSOR_POLL_UNRECOGNIZED_CMD;
					}
				} /* switch( sensor_poll_cmd ) */

			} /* while( sensor_poll_cmd != SENSOR_POLL_STOP ) */
		
		return sensor_status ;
        } /* SENSOR_POLL_CODE */ 

	/*--------------------------------------------------------------------------
	 SENSOR DUMP 
	--------------------------------------------------------------------------*/
	case SENSOR_DUMP_CODE: 
		{
		/* Tell the PC how many bytes to expect */
		#ifndef VALVE_CONTROLLER 
			usb_transmit( &num_sensor_bytes,
						sizeof( num_sensor_bytes ), 
						HAL_DEFAULT_TIMEOUT );
		#else
			if ( cmd_source == CMD_SOURCE_USB )
				{
				usb_transmit( &num_sensor_bytes,
							sizeof( num_sensor_bytes ), 
							HAL_DEFAULT_TIMEOUT );
				}
			else
				{
				valve_transmit( &num_sensor_bytes, 
				                sizeof( num_sensor_bytes ), 
								HAL_DEFAULT_TIMEOUT );
				}
		#endif /* #ifndef VALVE_CONTROLLER */

		/* Get the sensor readings */
	    sensor_status = sensor_dump( &sensor_data );	

		/* Convert to byte array */
		memcpy( &(sensor_data_bytes[0]), &sensor_data, sizeof( sensor_data ) );

		/* Transmit sensor readings to PC */
		if ( sensor_status == SENSOR_OK )
			{
			#ifndef VALVE_CONTROLLER
				usb_transmit( &sensor_data_bytes[0]      , 
							sizeof( sensor_data_bytes ), 
							HAL_SENSOR_TIMEOUT );
			#else
				if ( cmd_source == CMD_SOURCE_USB )
					{
					usb_transmit( &sensor_data_bytes[0]      , 
								sizeof( sensor_data_bytes ), 
								HAL_SENSOR_TIMEOUT );
					}
				else
					{
					valve_transmit( &sensor_data_bytes[0],
					                sizeof( sensor_data_bytes ), 
									HAL_SENSOR_TIMEOUT );
					}
			#endif /* #ifndef VALVE_CONTROLLER */
			return ( sensor_status );
            }
		else
			{
			/* Sensor readings not recieved */
			return( SENSOR_FAIL );
            }
        } /* SENSOR_DUMP_CODE */

	/*--------------------------------------------------------------------------
	 UNRECOGNIZED SUBCOMMAND 
	--------------------------------------------------------------------------*/
	default:
		{
		return ( SENSOR_UNRECOGNIZED_OP );
        }
    }

} /* sensor_cmd_handler */


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