/*******************************************************************************
*
* FILE: 
* 		commands.h
*
* DESCRIPTION: 
* 		Contains general command functions common to all embedded controllers
*
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef COMMANDS_H
#define COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif


/*------------------------------------------------------------------------------
 Macros/Typedefs 
------------------------------------------------------------------------------*/

/* Board identifier code */
#if   defined( A0001_REV1 )
	#define BOARD_ID    ( 0x01 )
#elif defined( A0002_REV1 )
	#define BOARD_ID    ( 0x02 )
#endif

/* Firmware Identifier Code */
#if   defined( TERMINAL    )
    #define FIRMWARE_ID    ( 0x01 )
#elif defined( DATA_LOGGER )
    #define FIRMWARE_ID    ( 0x02 )
#elif defined( DUAL_DEPLOY )
    #define FIRMWARE_ID    ( 0x03 )
#endif

/* Return response codes */
typedef enum _COMMAND_STATUS 
    {
    COMMAND_OK               , /* Terminal command successful        */
    COMMAND_SENSOR_ERROR     , /* Terminal sensor command error      */
    COMMAND_IGN_ERROR        , /* Terminal ignition command error    */
    COMMAND_FLASH_ERROR      , /* Terminal flash command error       */
    COMMAND_UNRECOGNIZED_CMD , /* Terminal invalid command           */
    COMMAND_DUAL_DEPLOY_ERROR, /* Terminal dual deploy command error */
    COMMAND_DATA_LOG_ERROR   , /* Data logger error                  */
    COMMAND_USB_ERROR        , /* USB communication error            */
    COMMAND_UNKNOWN_ERROR    , /* Unknown error occured              */
    COMMAND_ERROR        
    } COMMAND_STATUS;


/*------------------------------------------------------------------------------
 Function Prototypes 
------------------------------------------------------------------------------*/

/* Sends a single response byte back to sender */
COMMAND_STATUS ping
	(
    void
	);

/* Establishes a connection with PC by sending board/firmware identifiers */
COMMAND_STATUS connect
    (
    void
    );

#ifdef __cplusplus
}
#endif

#endif /* COMMANDS_H */

/*******************************************************************************
* END OF FILE                                                                  *
*******************************************************************************/